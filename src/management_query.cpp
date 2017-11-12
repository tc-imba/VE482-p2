#include "management_query.h"
#include "db/db.h"
#include "query_results.h"

#include <iostream>

constexpr const char *QuitQuery::qname;
constexpr const char *DropTableQuery::qname;
constexpr const char *DumpTableQuery::qname;
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

std::string DropTableQuery::toString() {
    return "QUERY = Drop TABLE, Table = \"" + this->tableName + "\"";
}

QueryResult::Ptr DropTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        db.dropTable(this->tableName);
    } catch (const std::invalid_argument &e) {
        return make_unique<ErrorMsgResult>(
                qname, string(e.what())
        );
    }
    return make_unique<SuccessMsgResult>(qname);
}

std::string DumpTableQuery::toString() {
    return "QUERY = Dump TABLE, "
                   "Table = \"" + this->tableName + "\", "
                   "ToFile = \"" + this->fileName + "\"";
}

QueryResult::Ptr DumpTableQuery::execute() {
    std::ofstream ofile(this->fileName);
    if (!ofile.is_open()) {
        return std::make_unique<ErrorMsgResult>(
                qname, tableName.c_str(),
                R"(Cannot open File "?".)"_f % this->fileName
        );
    }
    Database &db = Database::getInstance();
    try {
        ofile << (db[this->tableName]);
    } catch (const std::exception &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, tableName.c_str()
        );
    }
    ofile.close();
    return std::make_unique<SuccessMsgResult>(
            qname, tableName.c_str()
    );
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

