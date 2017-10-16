#include <iostream>
#include "db.h"
#include "uexception.h"

std::unique_ptr<Database> Database::instance = nullptr;


void Database::registerTable(Table::Ptr &&table) {
    auto name = table->name();
    if (this->tables.find(name) != this->tables.end())
        throw DuplicatedTableName(
                "Error when inserting table \"" + name + "\". Name already exists."
        );
    this->tables[name] = std::move(table);
}

Table &Database::operator[](std::string tableName) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    return *(it->second);
}

const Table &Database::operator[](std::string tableName) const {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    return *(it->second);
}

void Database::dropTable(std::string name) {
    auto it = this->tables.find(name);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error when trying to drop table \"" + name + "\". Table not found."
        );
    this->tables.erase(it);
}

void Database::printAllTable() {
    std::cout << "Database overview:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << std::setw(15) << "Table name";
    std::cout << std::setw(15) << "# of fields";
    std::cout << std::setw(15) << "# of entries" << std::endl;
    for (const auto& table : this->tables) {
        std::cout << std::setw(15) << table.first;
        std::cout << std::setw(15) << (*table.second).field().size() + 1;
        std::cout << std::setw(15) << (*table.second).size() << std::endl;
    }
    std::cout << "Total " << this->tables.size() << " tables." << std::endl;
    std::cout << "=========================" << std::endl;
}
