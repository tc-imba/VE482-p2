//
// Created by liu on 17-11-12.
//

#include "quit_query.h"

constexpr const char *QuitQuery::qname;

std::string QuitQuery::toString() {
    return "QUERY = Quit";
}

QueryResult::Ptr QuitQuery::execute() {
    start();
    auto &db = Database::getInstance();
    db.endQuery();
    complete(std::make_unique<SuccessMsgResult>(qname));
}