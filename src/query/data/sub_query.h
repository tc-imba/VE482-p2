#ifndef LEMONDB_SUB_QUERY_H
#define LEMONDB_SUB_QUERY_H

#include "../query.h"
#include "../task.h"

class SubTask;

class SubQuery : public ComplexQuery {
    static constexpr const char *qname = "SUB";
    std::vector<Table::FieldIndex> fieldsId;
protected:
    LEMONDB_TASK_PTR_DEF(SubTask);
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class SubTask;
};

class SubTask : public Task {
protected:
    LEMONDB_QUERY_PTR(SubQuery);
public:
    using Task::Task;
    void execute() override;
    friend class SubQuery;
};

#endif //LEMONDB_SUB_QUERY_H