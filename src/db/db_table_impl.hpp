//
// Created by tripack on 2017/8/26.
//

#ifndef SRC_DB_TABLE_IMPL_HPP
#define SRC_DB_TABLE_IMPL_HPP

#include "db_table.h"

#include <sstream>
#include <iomanip>
#include <exception>

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
        //
        this->fields.emplace_back(std::move(field));
        //
    }
    initialized = true;
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
    auto size = this->data.size();
    this->data.emplace_back(key, data);
    //this->keySet.insert(key);
    this->keyMap.emplace(key, size);
}

std::ostream &operator<<(std::ostream &os, const Table &table);

#endif //SRC_DB_TABLE_IMPL_H
