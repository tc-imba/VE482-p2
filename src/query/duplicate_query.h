//
// Created by 59889 on 2017/11/7.
//

#ifndef LEMONDB_DUPLICATE_QUERY_H
#define LEMONDB_DUPLICATE_QUERY_H

#include "query.h"
#include "task.h"

class DuplicateQuery : public ComplexQuery {
    static constexpr const char *qname = "DUPLICATE";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class DuplicateTask;
};

class DuplicateTask : public Task {
protected:
    LEMONDB_QUERY_PTR(DuplicateQuery);
public:
    using Task::Task;
    void execute() override;
//    friend class DuplicateQuery;
};

#endif //LEMONDB_DUPLICATE_QUERY_H
