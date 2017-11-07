//
// Created by 38569 on 2017/11/7.
//

#include "load_table_query.h"

constexpr const char *LoadTableQuery::qname;

std::string LoadTableQuery::toString() {
    return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}

QueryResult::Ptr LoadTableQuery::execute() {
    std::ifstream infile(this->fileName);
    if (!infile.is_open()) {
        return std::make_unique<ErrorMsgResult>(
                qname, "Cannot open file '?'"_f % this->fileName
        );
    }
    Table::Ptr table = nullptr;
    try {
        table = loadTableFromStream(infile, this->fileName);
        Database &db = Database::getInstance();
        db.registerTable(std::move(table));
    } catch (const std::exception &e) {
        return std::make_unique<ErrorMsgResult>(qname, e.what());
    }
//    std::cerr << "info: Loaded table " << this->fileName << std::endl;
    return std::make_unique<SuccessMsgResult>(qname);
}

void LoadTableTask::execute() {
    try {

        Database &db = Database::getInstance();

    } catch (const std::exception &e) {
        return;
        //return std::make_unique<ErrorMsgResult>(qname, e.what());
    }
}
