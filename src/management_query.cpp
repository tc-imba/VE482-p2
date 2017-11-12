#include "management_query.h"
#include "db/db.h"
#include "query_results.h"

#include <iostream>

constexpr const char *QuitQuery::qname;
constexpr const char *ListTableQuery::qname;
constexpr const char *PrintTableQuery::qname;

std::string QuitQuery::toString() {
    return "QUERY = Quit";
}

QueryResult::Ptr QuitQuery::execute() {
    auto &db = Database::getInstance();
    db.endQuery();
    db.addResult(this, std::make_unique<SuccessMsgResult>(qname));
}

QueryResult::Ptr ListTableQuery::execute() {
    Database &db = Database::getInstance();
    db.printAllTable();
    return std::make_unique<SuccessMsgResult>(qname);
}

std::string ListTableQuery::toString() {
    return "QUERY = LIST";
}

QueryResult::Ptr PrintTableQuery::execute() {
    using namespace std;
    try {
        Database &db = Database::getInstance();
        auto &table = db[this->tableName];
        cout << "================\n";
        cout << "TABLE = ";
        cout << table;
        cout << "================\n" << endl;
        return make_unique<SuccessMsgResult>(
                qname, tableName.c_str()
        );
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->tableName.c_str(),
                "No such table."s
        );
    }
}

std::string PrintTableQuery::toString() {
    return "QUERY = SHOWTABLE, Table = \"" + this->tableName + "\"";
}

