#ifndef LEMONDB_SUB_QUERY_H
#define LEMONDB_SUB_QUERY_H

#include "query.h"
#include "task.h"

class SubQuery : public ComplexQuery {
    static constexpr const char *qname = "SUB";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    std::vector<std::string> getOperands() { return operands; }
};

class SubTask : public Task {
protected:
    LEMONDB_QUERY_PTR(SubQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_SUB_QUERY_H