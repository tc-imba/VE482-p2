#ifndef SRC_DB_TABLE_H
#define SRC_DB_TABLE_H

#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <list>
#include <vector>
#include <iterator>
#include <ostream>
#include <unordered_set>

#include "uexception.h"
#include "formatter.h"


#define _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field)\
do {\
    try {\
        return it->datum.at(field);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field name "?" doesn't exists.)"_f % field\
        );\
    }\
} while(0)

#define _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index)\
do {\
    try {\
        auto fieldName = table->fields.at(index);\
        return it->datum.at(fieldName);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field index ? out of range.)"_f % index\
        );\
    }\
} while(0)

class Table;
class Table {
public:
    typedef std::string KeyType;
    typedef std::string FieldID;
    typedef int         FieldIndex;
    typedef int         ValueType;

private:
    struct Datum {
        KeyType key;
        std::unordered_map<FieldID, ValueType> datum;

        Datum()             = default;
        Datum(const Datum&) = default;
        template<class FieldIDContainer>
            Datum(const FieldIDContainer& fields) {
                for (const auto& FieldID: fields) {
                    datum.emplace(FieldID, ValueType());
                }
            }
    };

    typedef std::list<Datum>::iterator       DataIterator;
    typedef std::list<Datum>::const_iterator ConstDataIterator;

    const std::vector<std::string> fields;
    const Datum blankDatum; // Used to speed up processesing

    std::list<Datum> data;
    std::string tableName;
    std::unordered_set<KeyType> keySet;

public:
    typedef std::unique_ptr<Table> Ptr;

    template<class Iterator, class VType>
    class ObjectImpl {
        // A proxy class that provides abstraction on internal
        // Implementation. Allows independent variation on the
        // Representation for a table object
        friend class Table;

        const Table* table;
        Iterator it;

    public:
        typedef std::unique_ptr<ObjectImpl> Ptr;

        ObjectImpl(Iterator datumIt, const Table* t)
                : it(datumIt), table(t) {}

        ObjectImpl(const ObjectImpl&)            = default;
        ObjectImpl(ObjectImpl&&)                 = default;
        ObjectImpl& operator=(const ObjectImpl&) = default;
        ObjectImpl& operator=(ObjectImpl&&)      = default;
        ~ObjectImpl() = default;

        KeyType           key() const {return it->key;}

        // Accessing by index should be, at least as fast as 
        // accessing by field name. Clients should perfer 
        // accessinb by index if the same field is accessed frequently
        VType&  operator[](const FieldID& field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }
        VType&  operator[](const FieldIndex& field) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(field);
        }
        VType&  get(const FieldID& field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }
        VType&  get(const FieldIndex& field) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(field);
        }
    };

    typedef ObjectImpl<DataIterator, ValueType>             Object;
    typedef ObjectImpl<ConstDataIterator, const ValueType>  ConstObject;

    template<typename ObjType, typename DatumIterator>
    class IteratorImpl {
        using difference_type   = std::ptrdiff_t;
        using value_type        = ObjType;
        using pointer           = typename ObjType::Ptr;
        using reference         = ObjType;
        using iterator_category = std::bidirectional_iterator_tag;
        // See https://stackoverflow.com/questions/37031805/

        friend class Table;

        const Table* table;
        DatumIterator it;

    public:
        IteratorImpl(DatumIterator datumIt, const Table* t)
                : it(datumIt), table(t) {}

        IteratorImpl()                               = default;
        IteratorImpl(const IteratorImpl&)            = default;
        IteratorImpl(IteratorImpl&&)                 = default;
        IteratorImpl& operator=(const IteratorImpl&) = default;
        IteratorImpl& operator=(IteratorImpl&&)      = default;
        ~IteratorImpl() = default;

        pointer                 operator->()    {return createProxy(it, table);}
        reference               operator* ()    {return *createProxy(it, table);}
        IteratorImpl&           operator++()    {++it; return *this;}
        IteratorImpl&           operator--()    {--it; return *this;}
        IteratorImpl            operator++(int) {
            auto retVal = it; ++it; return IteratorImpl(retVal);
        }
        IteratorImpl            operator--(int) {
            auto retVal = it; --it; return IteratorImpl(retVal);
        }
        bool operator==(const IteratorImpl& other) {return this->it == other.it;}
        bool operator!=(const IteratorImpl& other) {return this->it != other.it;}
    };

    typedef IteratorImpl<Object,       decltype(data.begin())>  Iterator;
    typedef IteratorImpl<ConstObject,  decltype(data.cbegin())> ConstIterator;


private:

    static ConstObject::Ptr createProxy(ConstDataIterator it, const Table* table) {
        return std::make_unique<ConstObject>(it, table);
    }

    static Object::Ptr createProxy(DataIterator it, const Table* table) {
        return std::make_unique<Object>(it, table);
    }

public:
    // Accept any container that contains string.
    template<class FieldIDContainer>
        Table(std::string name, const FieldIDContainer& fields);

    template<class AssocContainer>
        void insert(KeyType key,const AssocContainer& data);

    template<class ValueTypeContainer>
        void insertByIndex(KeyType key,const ValueTypeContainer& data);

    Iterator erase(Iterator it) {
        this->keySet.erase(it.it->key);
        return Iterator(this->data.erase(it.it), it.table);
    }
    void setName(std::string name) {this->tableName = name;}
    std::string name()  const {return this->tableName; }
    bool        empty() const {return this->data.empty();}
    size_t      size()  const {return this->data.size();}

    const std::vector<FieldID>& field() const {
        return this->fields;
    }

    size_t clear() {
        auto result = keySet.size();
        data.clear();
        keySet.clear();
        return result;
    }

    Iterator        begin()       {return Iterator(data.begin(), this); }
    Iterator        end()         {return Iterator(data.end(), this);   }
    ConstIterator   begin() const {return ConstIterator(data.cbegin(), this);}
    ConstIterator   end()   const {return ConstIterator(data.cend(), this);  }

    friend std::ostream &operator<<(std::ostream &os, const Table &table);
};

Table::Ptr loadTableFromStream(std::istream& is, std::string source = "");

#include "db_table_impl.hpp"

#endif //SRC_DB_TABLE_H
