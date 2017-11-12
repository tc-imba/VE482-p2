//
// Created by 59889 on 2017/11/12.
//

#include "drop_query.h"

constexpr const char* DropTableQuery::qname;

std::string DropTableQuery::toString() {
    return "QUERY = DROP, Table = \"" + targetTable + "\"";
}

QueryResult::Ptr DropTableQuery::execute() {
    start();
    using namespace std;
    Database& db = Database::getInstance();
    try{
        auto &table = db[targetTable];
        table.drop();
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
