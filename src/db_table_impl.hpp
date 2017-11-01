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
    Table::Table(std::string name, const FieldIDContainer& fields_)
    : tableName(name),
      fields(fields_.cbegin(), fields_.cend()),
      blankDatum(fields_) {
    for (const auto& field : fields)
        if (field == "KEY")
            throw MultipleKey(
                    "Error creating table \"" + name + "\": Multiple KEY field."
            );
}

template<class AssocContainer>
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
}

template<class ValueTypeContainer>
    void Table::insertByIndex(KeyType key, const ValueTypeContainer& data) {
    if (this->keySet.find(key) != this->keySet.end()) {
        std::string err = "In Table \"" + this->tableName
                          + "\" : Key \"" + key + "\" already exists!";
        throw ConflictingKey(err);
    }
    Datum d(blankDatum);
    d.key = key;
    auto it = data.begin();
    for (const auto& field : this->fields) {
        d.datum[field] = *it ;
        ++it;
    }
    this->data.push_back(d);
    this->keySet.insert(key);
}

std::ostream &operator<<(std::ostream &os, const Table &table);

#endif //SRC_DB_TABLE_IMPL_H
