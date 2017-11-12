//
// Created by linzhi on 2017/11/7.
//

#ifndef LEMONDB_SUM_QUERY_H
#define LEMONDB_SUM_QUERY_H

#include "../query.h"
#include "../task.h"

class SumTask;

class SumQuery : public ComplexQuery {
    static constexpr const char *qname = "SUM";
    std::vector<Table::FieldIndex> fieldsId;
protected:
    LEMONDB_TASK_PTR_DEF(SumTask);
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class SumTask;
};

class SumTask : public Task {
    std::vector<Table::ValueType> fieldsSum;
protected:
    LEMONDB_QUERY_PTR(SumQuery);
public:
    using Task::Task;
    void execute() override;
    friend class SumQuery;
    //std::vector<int> getFieldSums() { return fieldSums; }
};

#endif //LEMONDB_SUM_QUERY_H
