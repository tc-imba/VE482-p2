//
// Created by 38569 on 2017/11/7.
//

#include "load_table_query.h"
#include <iostream>

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

QueryResult::Ptr LoadTableQuery::combine() {
    //std::cerr << "Complete " << toString() << std::endl;
    return std::make_unique<SuccessMsgResult>(qname);
}

void LoadTableQuery::setTargetTable(std::string tableName) {
    targetTable = std::move(tableName);
}

void LoadTableTask::execute() {
    auto query = getQuery();
    try {
        std::ifstream infile(query->fileName);
        if (!infile.is_open()) {
            errorResult = std::make_unique<ErrorMsgResult>(query->qname, "Cannot open file '?'"_f % query->fileName);
            return;
        }
        auto &db = Database::getInstance();
        auto &table = loadTableFromStream(infile, query->fileName);
        query->setTargetTable(table.name());
        infile.close();
        Task::execute();
    } catch (const std::exception &e) {
        errorResult = std::make_unique<ErrorMsgResult>(query->qname, e.what());
        return;
    }
}
