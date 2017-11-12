//
// Created by liu on 17-11-12.
//

#ifndef LEMONDB_QUIT_QUERY_H
#define LEMONDB_QUIT_QUERY_H

#include "../query.h"

class QuitQuery : public TaskQuery {
    static constexpr const char *qname = "QUIT";
public:
    LEMONDB_QUERY_WRITER(false)
    QuitQuery() = default;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //LEMONDB_QUIT_QUERY_H
