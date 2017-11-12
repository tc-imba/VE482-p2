//
// Created by 59889 on 2017/11/12.
//

#include "dump_query.h"
#include <iostream>

constexpr const char *DumpTableQuery::qname;

std::string DumpTableQuery::toString() {
    return "QUERY = Dump TABLE, FILE = \"" + this->fileName + "\"";
}

QueryResult::Ptr DumpTableQuery::execute() {
    start();
    try {
        auto &db = Database::getInstance();
        addUniqueTask<DumpTableTask>(db);
    } catch (const std::exception &e) {
        return std::make_unique<ErrorMsgResult>(qname, e.what());
    }
    return std::make_unique<SuccessMsgResult>(qname);
}

QueryResult::Ptr DumpTableQuery::combine() {
    //std::cerr << "Complete " << toString() << std::endl;
    return std::make_unique<SuccessMsgResult>(qname);
}

void DumpTableTask::execute() {
    auto query = getQuery();
    try {
        std::ofstream outfile(query->fileName);
        if (!outfile.is_open()) {
            errorResult = std::make_unique<ErrorMsgResult>(query->qname, "Cannot open file '?'"_f % query->fileName);
            return;
        }
        auto &db = Database::getInstance();
        outfile << db[query->getTableName()];
        outfile.close();
        Task::execute();
    } catch (const std::exception &e) {
        errorResult = std::make_unique<ErrorMsgResult>(query->qname, e.what());
        return;
    }
}
