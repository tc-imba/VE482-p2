#ifndef SRC_DB_H
#define SRC_DB_H

#include "db_table.h"
#include <string>
#include <memory>
#include <unordered_map>

class Database {
    static std::unique_ptr<Database> instance;

    std::unordered_map<std::string, std::unique_ptr<Table>> tables;
    
    Database() = default;

public:
    void         registerTable(Table::Ptr &&table);
    void         dropTable(std::string tableName);
    void         printAllTable();

    Table&       operator[](std::string tableName);
    const Table& operator[](std::string tableName) const;

    Database&    operator=(const Database&) = delete;
    Database&    operator=(Database&&)      = delete;
                 Database(const Database&)  = delete;
                 Database(Database&&)       = delete;
    ~Database() = default;

    static
    Database&    getInstance() {
        if (Database::instance == nullptr) {
            instance = std::unique_ptr<Database>(new Database);
        }
        return *instance;
    }
};

#endif //SRC_DB_H
