#ifndef SRC_DB_TABLE_H
#define SRC_DB_TABLE_H

#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <list>
#include <iterator>
#include <ostream>
#include <unordered_set>
#include <mutex>

#include "../uexception.h"
#include "../formatter.h"
#include "../query/query_base.h"

#define _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field)\
do {\
    try {\
        auto &index = table->fieldMap.at(field);\
        return it->datum.at(index);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field name "?" doesn't exists.)"_f % (field)\
        );\
    }\
} while(0)

#define _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index)\
do {\
    try {\
        return it->datum.at(index);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field index ? out of range.)"_f % (index)\
        );\
    }\
} while(0)

class Table;

class Table {
public:
    typedef std::string KeyType;
    typedef std::string FieldID;
    typedef size_t FieldIndex;
    typedef int ValueType;
    typedef size_t SizeType;

private:
    /** A row in the table */
    struct Datum {
        /** Unique key of this datum */
        KeyType key;
        /** The values in the order of fields  */
        std::vector<ValueType> datum;

        Datum() = default;
        Datum(const Datum &) = default;

        explicit Datum(const SizeType &size) {
            datum = std::vector<ValueType>(size, ValueType());
        }

        template<class ValueTypeContainer>
        explicit Datum(const KeyType &key, const ValueTypeContainer &datum) {
            this->key = key;
            this->datum = datum;
        }

        explicit Datum(const KeyType &key, std::vector<ValueType> &&datum) {
            this->key = key;
            this->datum = datum;
        }
    };

    typedef std::vector<Datum>::iterator DataIterator;
    typedef std::vector<Datum>::const_iterator ConstDataIterator;

    /** The fields, ordered as defined in fieldMap */
    std::vector<FieldID> fields;
    /** Map field name into index */
    std::unordered_map<FieldID, SizeType> fieldMap;
    /** Defined by tripack, seems to be used to speed up processing */
    const Datum blankDatum;

    /** The rows are saved in a vector, which is unsorted */
    std::vector<Datum> data;
    /** Saves data in a new vector after delete or duplicate */
    std::vector<Datum> dataNew;
    /** Used to keep the keys unique */
    std::unordered_set<KeyType> keySet;

    /** The name of table */
    std::string tableName;

public:
    typedef std::unique_ptr<Table> Ptr;

    /**
     * A proxy class that provides abstraction on internal
     * Implementation. Allows independent variation on the
     * Representation for a table object
     *
     * @tparam Iterator
     * @tparam VType
     */
    template<class Iterator, class VType>
    class ObjectImpl {
        //
        friend class Table;

        const Table *table;
        Iterator it;

    public:
        typedef std::unique_ptr<ObjectImpl> Ptr;

        ObjectImpl(Iterator datumIt, const Table *t)
                : it(datumIt), table(t) {}

        ObjectImpl(const ObjectImpl &) = default;
        ObjectImpl(ObjectImpl &&) noexcept = default;
        ObjectImpl &operator=(const ObjectImpl &) = default;
        ObjectImpl &operator=(ObjectImpl &&) noexcept = default;
        ~ObjectImpl() = default;

        KeyType key() const { return it->key; }

        // Accessing by index should be, at least as fast as
        // accessing by field name. Clients should perfer
        // accessing by index if the same field is accessed frequently
        // (the implement is improved so that index is actually faster now)
        VType &operator[](const FieldID &field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }

        VType &operator[](const FieldIndex &index) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index);
        }

        VType &get(const FieldID &field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }

        VType &get(const FieldIndex &index) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index);
        }
    };

    typedef ObjectImpl<DataIterator, ValueType> Object;
    typedef ObjectImpl<ConstDataIterator, const ValueType> ConstObject;

    template<typename ObjType, typename DatumIterator>
    class IteratorImpl {
        using difference_type   = std::ptrdiff_t;
        using value_type        = ObjType;
        using pointer           = typename ObjType::Ptr;
        using reference         = ObjType;
        using iterator_category = std::random_access_iterator_tag;
        // See https://stackoverflow.com/questions/37031805/

        friend class Table;

        const Table *table = nullptr;
        DatumIterator it;

    public:
        IteratorImpl(DatumIterator datumIt, const Table *t)
                : it(datumIt), table(t) {}

        IteratorImpl() = default;
        IteratorImpl(const IteratorImpl &) = default;
        IteratorImpl(IteratorImpl &&) noexcept = default;
        IteratorImpl &operator=(const IteratorImpl &)     = default;
        IteratorImpl &operator=(IteratorImpl &&) noexcept = default;
        ~IteratorImpl() = default;

        pointer operator->() { return createProxy(it, table); }

        reference operator*() { return *createProxy(it, table); }

        IteratorImpl &operator++() {
            ++it;
            return *this;
        }

        IteratorImpl &operator--() {
            --it;
            return *this;
        }

        IteratorImpl operator++(int) {
            auto retVal = IteratorImpl(*this);
            ++it;
            return retVal;
        }

        IteratorImpl operator--(int) {
            auto retVal = IteratorImpl(*this);
            --it;
            return retVal;
        }

        bool operator==(const IteratorImpl &other) { return this->it == other.it; }

        bool operator!=(const IteratorImpl &other) { return this->it != other.it; }

        bool operator<=(const IteratorImpl &other) { return this->it <= other.it; }

        bool operator>=(const IteratorImpl &other) { return this->it >= other.it; }

        bool operator<(const IteratorImpl &other) { return this->it < other.it; }

        bool operator>(const IteratorImpl &other) { return this->it > other.it; }

        IteratorImpl operator+(int n) {
            return IteratorImpl(it + n, table);
        }

        IteratorImpl operator-(int n) {
            return IteratorImpl(it - n, table);
        }

        IteratorImpl &operator+=(int n) {
            it += n; return *this;
        }

        IteratorImpl &operator-=(int n) {
            it -= n; return *this;
        }
    };

    typedef IteratorImpl<Object, decltype(data.begin())> Iterator;
    typedef IteratorImpl<ConstObject, decltype(data.cbegin())> ConstIterator;

private:

    static ConstObject::Ptr createProxy(ConstDataIterator it, const Table *table) {
        return std::make_unique<ConstObject>(it, table);
    }

    static Object::Ptr createProxy(DataIterator it, const Table *table) {
        return std::make_unique<Object>(it, table);
    }

public:
    // Accept any container that contains string.
    template<class FieldIDContainer>
    Table(const std::string &name, const FieldIDContainer &_fields);

    Table(std::string name, const Table &origin) :
            fields(origin.fields), tableName(std::move(name)), keySet(origin.keySet), data(origin.data) {}

    FieldIndex getFieldIndex(const FieldID &field) const {
        try {
            return this->fieldMap.at(field);
        } catch (const std::out_of_range &e) {
            throw TableFieldNotFound(
                    R"(Field name "?" doesn't exists.)"_f % (field)
            );
        }
    }

    template<class AssocContainer>
    void insert(KeyType key, const AssocContainer &data) = delete;

    template<class ValueTypeContainer>
    void insertByIndex(KeyType key, const ValueTypeContainer &data);

    /**
     * Erase the key in the table
     * Caution: this function only erases the key in keySet, leaves data unchanged
     * @param it
     */
    void erase(const Iterator &it) {
        this->keySet.erase(it.it->key);
    }

    /**
     * Move datum from data to dataNew
     * Caution: iterator it can't be accessed again after move() is called
     * @param it
     */
    void move(Iterator &it) {
        this->dataNew.push_back(std::move(*(it.it)));
    }

    /**
     * Swap data and newData
     * vector::clear ensures that the capacity of dataNew unchanged
     * so push_back to dataNew is efficient
     */
    void swapData() {
        std::swap(this->data, this->dataNew);
        dataNew.clear();
    }

    /**
     * Set the name of the table
     * @param name
     */
    void setName(std::string name) { this->tableName = std::move(name); }

    /**
     * Get the name of the table
     * @return
     */
    const std::string &name() const { return this->tableName; }

    /**
     * Return whether the table is empty
     * @return
     */
    bool empty() const { return this->data.empty(); }

    /**
     * Return the num of data stored in the table
     * @return
     */
    size_t size() const { return this->data.size(); }

    /**
     * Return the fields in the table
     * @return
     */
    const std::vector<FieldID> &field() const { return this->fields; }

    /**
     * Clear all content in the table
     * @return rows affected
     */
    size_t clear() {
        auto result = keySet.size();
        data.clear();
        keySet.clear();
        return result;
    }

    /**
     * Get a begin iterator similar to the standard iterator
     * @return
     */
    Iterator begin() { return Iterator(data.begin(), this); }

    /**
     * Get a end iterator similar to the standard iterator
     * @return
     */
    Iterator end() { return Iterator(data.end(), this); }

    /**
     * Get a const begin iterator similar to the standard iterator
     * @return
     */
    ConstIterator begin() const { return ConstIterator(data.cbegin(), this); }

    /**
     * Get a const end iterator similar to the standard iterator
     * @return
     */
    ConstIterator end() const { return ConstIterator(data.cend(), this); }

    /**
     * Overload the << operator for complete print of the table
     * @param os
     * @param table
     * @return
     */
    friend std::ostream &operator<<(std::ostream &os, const Table &table);

private:
    /**
     * (protected by @see queryQueueMutex)
     * A reader/writer queue implemented with std::list
     * only push_back, pop_front and splice with forward iterator should be used
     * std::queue is not used because it doesn't support iterator
     */
    std::list<Query::Ptr> queryQueue;
    /**
     * (protected by @see queryQueueMutex)
     * The value of queryQueueCounter have different meanings
     * -1 : writing
     * 0  : idle
     * 1+ : reading (the number of readers)
     */
    int queryQueueCounter = 0;
    /** queryQueue and queryQueueCounter must be locked for multi-thread */
    std::mutex queryQueueMutex;
public:
    void addQuery(Query::Ptr &query);
    void refreshQuery();
};

Table::Ptr loadTableFromStream(std::istream &is, std::string source = "");

#include "db_table_impl.hpp"

#endif //SRC_DB_TABLE_H
