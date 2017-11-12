//
// Created by liu on 17-11-12.
//

#ifndef LEMONDB_TRUNCATE_QUERY_H
#define LEMONDB_TRUNCATE_QUERY_H

#include "../query.h"
#include "../task.h"

class TruncateTableQuery : public TaskQuery {
    static constexpr const char *qname = "TRUNCATE";
public:
    LEMONDB_QUERY_WRITER(true)
    using TaskQuery::TaskQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //LEMONDB_TRUNCATE_QUERY_H
