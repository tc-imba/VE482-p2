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
#include <atomic>
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <exception>

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
    typedef std::string FieldNameType;
    typedef size_t FieldIndex;
    typedef int ValueType;
    static constexpr const ValueType ValueTypeMax = INT32_MAX;
    static constexpr const ValueType ValueTypeMin = INT32_MIN;
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

    /** Whether the table can be used now */
    bool initialized = false;

    /** The fields, ordered as defined in fieldMap */
    std::vector<FieldNameType> fields;
    /** Map field name into index */
    std::unordered_map<FieldNameType, FieldIndex> fieldMap;

    /** The rows are saved in a vector, which is unsorted */
    std::vector<Datum> data;
    /** Saves data in a new vector after delete or duplicate */
    std::vector<Datum> dataNew;
    /** mutex to protect dataNew */
    std::mutex dataNewMutex;
    /** Used to keep the keys unique and provide O(1) access with key */
    std::unordered_map<KeyType, SizeType> keyMap;
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

        /** Not const because key can be updated */
        Table *table;
        Iterator it;

    public:
        typedef std::unique_ptr<ObjectImpl> Ptr;

        ObjectImpl(Iterator datumIt, const Table *t)
                : it(datumIt), table(const_cast<Table *>(t)) {}

        ObjectImpl(const ObjectImpl &) = default;

        ObjectImpl(ObjectImpl &&) noexcept = default;

        ObjectImpl &operator=(const ObjectImpl &) = default;

        ObjectImpl &operator=(ObjectImpl &&) noexcept = default;

        ~ObjectImpl() = default;

        KeyType key() const { return it->key; }

        void setKey(KeyType key) {
            table->dataNewMutex.lock();
            auto keyMapIt = table->keyMap.find(it->key);
            auto dataIt = std::move(keyMapIt->second);
            table->keyMap.erase(keyMapIt);
            table->keyMap.emplace(key, dataIt);
            it->key = std::move(key);
            table->dataNewMutex.unlock();
        }

        // Accessing by index should be, at least as fast as
        // accessing by field name. Clients should perfer
        // accessing by index if the same field is accessed frequently
        // (the implement is improved so that index is actually faster now)
        VType &operator[](const FieldNameType &field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }

        VType &operator[](const FieldIndex &index) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index);
        }

        VType &get(const FieldNameType &field) const {
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
            it += n;
            return *this;
        }

        IteratorImpl &operator-=(int n) {
            it -= n;
            return *this;
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
    Table() = delete;

    explicit Table(const std::string &name) : tableName(name) {}

    // Accept any container that contains string.
    template<class FieldIDContainer>
    Table(const std::string &name, const FieldIDContainer &_fields);

    template<class FieldIDContainer>
    void init(const FieldIDContainer &fields);

    Table(std::string name, const Table &origin) :
            fields(origin.fields), tableName(std::move(name)),
            keyMap(origin.keyMap), data(origin.data) {}

    void copy(const Table &origin) {
        fields = origin.fields;
        fieldMap = origin.fieldMap;
        data = origin.data;
        keyMap = origin.keyMap;
        queryQueueMutex.lock();
        initialized = true;
        queryQueueMutex.unlock();
    }

    void drop() {
        queryQueueCounter = 0;
        fields.clear();
        fieldMap.clear();
        data.clear();
        keyMap.clear();
        queryQueueMutex.lock();
        initialized = false;
        queryQueueMutex.unlock();
    }

    bool isInited() const { return initialized; }

    FieldIndex getFieldIndex(const FieldNameType &field) const {
        try {
            return this->fieldMap.at(field);
        } catch (const std::out_of_range &e) {
            throw TableFieldNotFound(
                    R"(Field name "?" doesn't exists.)"_f % (field)
            );
        }
    }

    template<class ValueTypeContainer>
    void insertByIndex(KeyType key, const ValueTypeContainer &data);

    /**
     * Access the value according to the key
     * @param key
     * @return the Object that KEY = key, or nullptr if key doesn't exist
     */
    Object::Ptr operator[](const KeyType &key) {
        auto it = keyMap.find(key);
        if (it == keyMap.end()) {
            // not found
            return nullptr;
        } else {
            return createProxy(data.begin() + it->second, this);
        }
    }

    /**
     * not thread safe function
     * Remove only one datum
     * only used when delete by key
     * @param it
     */
    void eraseUnique(Object::Ptr &&object) {
        auto pair = keyMap.find(object->it->key);
        data.erase(data.begin() + pair->second);
        keyMap.erase(pair);
    }

    /**
     * thread safe function
     * Erase the key in the table
     * Caution: this function only erases the key in keyMap, leaves data unchanged
     * no other operation related to keyMap can be applied before swapData is called
     * @param it
     */
    void erase(const Iterator &it) {
        dataNewMutex.lock();
        keyMap.erase(it.it->key);
        dataNewMutex.unlock();
    }

    /**
     * thread safe function
     * Move datum from data to dataNew
     * Caution: iterator it can't be accessed again after move() is called
     * no other operation related to keyMap can be applied before swapData is called
     * @param it
     */
    void move(Iterator &it) {
        dataNewMutex.lock();
        keyMap.at(it.it->key) = dataNew.size();
        dataNew.emplace_back(std::move(*(it.it)));
        dataNewMutex.unlock();
    }

    /**
     * not thread safe function
     * Swap data and newData
     * vector::clear ensures that the capacity of dataNew unchanged
     * so push_back to dataNew is efficient
     */
    void swapData() {
        std::swap(data, dataNew);
        dataNew.clear();
    }

    /**
     * Duplicate it and put it into dataNew
     * if {key}_copy exists, nothing happens
     * this function is used only in duplicate query
     */
    bool duplicate(Iterator &it) {
        auto key = it->key() + "_copy";
        dataNewMutex.lock();
        if (keyMap.find(key) != keyMap.end()) {
            dataNewMutex.unlock();
            return false;
        }
        dataNew.emplace_back(key, it->it->datum);
        dataNewMutex.unlock();
        return true;
    }

    /**
     * insert dataNew to the end of data
     * then dataNew is cleared for future query
     * this function is used only in duplicate query
     */
    void mergeData() {
        std::for_each(dataNew.begin(), dataNew.end(), [this](Datum &datum) {
            keyMap.emplace(datum.key, data.size());
            data.emplace_back(std::move(datum));
        });
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
    const std::vector<FieldNameType> &field() const { return this->fields; }

    /**
     * Clear all content in the table
     * @return rows affected
     */
    size_t clear() {
        auto result = keyMap.size();
        data.clear();
        keyMap.clear();
        return result;
    }

    /**
     * Get a begin iterator similar to the standard iterator
     * @return begin iterator
     */
    Iterator begin() { return Iterator(data.begin(), this); }

    /**
     * Get a end iterator similar to the standard iterator
     * @return end iterator
     */
    Iterator end() { return Iterator(data.end(), this); }

    /**
     * Get a const begin iterator similar to the standard iterator
     * @return const begin iterator
     */
    ConstIterator begin() const { return ConstIterator(data.cbegin(), this); }

    /**
     * Get a const end iterator similar to the standard iterator
     * @return const end iterator
     */
    ConstIterator end() const { return ConstIterator(data.cend(), this); }

    /**
     * Overload the << operator for complete print of the table
     * @param os
     * @param table
     * @return the origin ostream
     */
    friend std::ostream &operator<<(std::ostream &os, const Table &table);

private:
    /**
     * (protected by @see queryQueueMutex)
     * A reader/writer queue implemented with std::list
     * only push_back, pop_front and splice with forward iterator should be used
     * std::queue is not used because it doesn't support iterator
     */
    std::list<Query *> queryQueue;
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
    void addQuery(Query *query);

    void completeQuery();
};

template<class FieldIDContainer>
Table::Table(const std::string &name, const FieldIDContainer &_fields)
        : tableName(name),
          fields(_fields.cbegin(), _fields.cend()) {
    size_t i = 0;
    for (const auto &field : fields) {
        if (field == "KEY")
            throw MultipleKey(
                    "Error creating table \"" + name + "\": Multiple KEY field."
            );
        fieldMap.insert({{field},
                         {i++}});
    }
}

template<class FieldIDContainer>
void Table::init(const FieldIDContainer &fields) {
    size_t i = 0;
    for (const auto &field : fields) {
        if (field == "KEY")
            throw MultipleKey(
                    "Error creating table \"" + tableName + "\": Multiple KEY field."
            );
        fieldMap.emplace(field, i++);
        this->fields.emplace_back(std::move(field));
        //fieldIndexMap.emplace_back(i++);
    }
    queryQueueMutex.lock();
    initialized = true;
    queryQueueMutex.unlock();
}

/*template<class AssocContainer>
    void Table::insert(KeyType key, const AssocContainer& data) {
    if (this->keySet.find(key) != this->keySet.end()) {
        std::string err = "In Table \"" + this->tableName
                          + "\" : Key \"" + key + "\" already exists!";
        throw ConflictingKey(err);
    }
    Datum d(blankDatum);
    d.key = key;
    for (const auto& field : this->fields) {
        auto it = data.find(field);
        d.datum[field] = (it == data.end() ? ValueType() : it->second);
    }
    this->data.push_back(d);
    this->keySet.insert(key);
}*/

template<class ValueTypeContainer>
void Table::insertByIndex(KeyType key, const ValueTypeContainer &data) {
    if (this->keyMap.find(key) != this->keyMap.end()) {
        std::string err = "In Table \"" + this->tableName
                          + "\" : Key \"" + key + "\" already exists!";
        throw ConflictingKey(err);
    }
    this->keyMap.emplace(key, this->data.size());
    this->data.emplace_back(key, data);
    //this->keySet.insert(key);
}

Table &loadTableFromStream(std::istream &is, std::string source = "");

std::ostream &operator<<(std::ostream &os, const Table &table);

#endif //SRC_DB_TABLE_H
