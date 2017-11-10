//
// Created by 38569 on 2017/11/7.
//

#include "load_table_query.h"

constexpr const char *LoadTableQuery::qname;

std::string LoadTableQuery::toString() {
    return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}

QueryResult::Ptr LoadTableQuery::execute() {
    try {
        auto &db = Database::getInstance();
        addUniqueTask<LoadTableTask>(db);
    } catch (const std::exception &e) {
        return std::make_unique<ErrorMsgResult>(qname, e.what());
    }
    return std::make_unique<SuccessMsgResult>(qname);
}

void LoadTableTask::execute() {
    auto query = getQuery();
    try {
        std::ifstream infile(query->fileName);
        if (!infile.is_open()) {
            errorResult = std::make_unique<ErrorMsgResult>(query->qname, "Cannot open file '?'"_f % query->fileName);
            return;
        }
        auto &table = loadTableFromStream(infile, query->fileName);
        infile.close();
        Task::execute();
    } catch (const std::exception &e) {
        errorResult = std::make_unique<ErrorMsgResult>(query->qname, e.what());
        return;
    }
}
