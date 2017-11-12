//
// Created by 59889 on 2017/11/12.
//

#ifndef LEMONDB_DROP_QUERY_H
#define LEMONDB_DROP_QUERY_H

#include "../query.h"
#include "../task.h"

class DropTableQuery : public TaskQuery {
    static constexpr const char *qname = "DROP";
public:
    LEMONDB_QUERY_WRITER(true)
    using TaskQuery::TaskQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //LEMONDB_DROP_QUERY_H
