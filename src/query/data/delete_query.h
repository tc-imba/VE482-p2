//
// Created by admin on 2017/11/2.
//

#ifndef LEMONDB_DELETE_QUERY_H
#define LEMONDB_DELETE_QUERY_H

#include "../query.h"
#include "../task.h"

class DeleteQuery : public ComplexQuery {
    static constexpr const char *qname = "DELETE";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class DeleteTask;
};

class DeleteTask : public Task {
protected:
    LEMONDB_QUERY_PTR(DeleteQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_DELETE_QUERY_H
