#ifndef LEMONDB_INSERT_QUERY_H
#define LEMONDB_INSERT_QUERY_H

#include "../query.h"
#include "../task.h"

class InsertQuery : public ComplexQuery {
    static constexpr const char *qname = "INSERT";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine(int taskComplete) override;
	friend class InsertTask;
};

class InsertTask : public Task {
protected:
    LEMONDB_QUERY_PTR(InsertQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_INSERT_QUERY_H
