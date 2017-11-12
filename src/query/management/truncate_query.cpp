//
// Created by liu on 17-11-12.
//

#include "truncate_query.h"

constexpr const char* TruncateTableQuery::qname;

std::string TruncateTableQuery::toString() {
    return "QUERY = TRUNCATE, Table = \"" + targetTable + "\"";
}

QueryResult::Ptr TruncateTableQuery::execute() {
    start();
    using namespace std;
    Database& db = Database::getInstance();
    try{
        auto &table = db[targetTable];
        table.clear();
        complete(make_unique<SuccessMsgResult>(qname, targetTable.c_str()));
        return make_unique<SuccessMsgResult>(qname);
    } catch (const TableNameNotFound& e) {
        return make_unique<ErrorMsgResult>(
                qname, targetTable.c_str(),
                "No such table."s
        );
    }  catch (const exception& e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}
