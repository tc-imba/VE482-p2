#ifndef LEMONDB_ADD_QUERY_H
#define LEMONDB_ADD_QUERY_H

#include "../query.h"
#include "../task.h"

class AddTask;

class AddQuery : public ComplexQuery {
    static constexpr const char *qname = "ADD";
    std::vector<Table::FieldIndex> fieldsId;
protected:
    LEMONDB_TASK_PTR_DEF(AddTask);
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine(int taskComplete) override;
    friend class AddTask;
};

class AddTask : public Task {
protected:
    LEMONDB_QUERY_PTR(AddQuery);
public:
    using Task::Task;
    void execute() override;
    friend class AddQuery;
};

#endif //LEMONDB_ADD_QUERY_H