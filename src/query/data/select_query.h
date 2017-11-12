//
// Created by linzhi on 11/6/17.
//

#ifndef LEMONDB_SELECT_QUERY_H
#define LEMONDB_SELECT_QUERY_H

#include "../query.h"
#include "../task.h"

class SelectTask;

class SelectQuery : public ComplexQuery {
    static constexpr const char *qname = "SELECT";
    std::vector<Table::FieldIndex> fieldsId;
protected:
    LEMONDB_TASK_PTR_DEF(SelectTask);
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class SelectTask;
};

class SelectTask : public Task {
    std::vector<std::pair<std::string, std::vector<Table::ValueType> > > results;
protected:
    LEMONDB_QUERY_PTR(SelectQuery);
public:
    using Task::Task;
    void execute() override;
    friend class SelectQuery;
};

#endif //LEMONDB_SELECT_QUERY_H
