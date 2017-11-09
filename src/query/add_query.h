#ifndef LEMONDB_ADD_QUERY_H
#define LEMONDB_ADD_QUERY_H

#include "query.h"
#include "task.h"

class AddQuery : public ComplexQuery {
    static constexpr const char *qname = "ADD";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    std::vector<std::string> getOperands() { return operands; }
};

class AddTask : public Task {
protected:
    LEMONDB_QUERY_PTR(AddQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_ADD_QUERY_H