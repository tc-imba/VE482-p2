//
// Created by linzhi on 2017/11/7.
//

#ifndef LEMONDB_SUM_QUERY_H
#define LEMONDB_SUM_QUERY_H
#include "query.h"
#include "task.h"

class SumQuery : public ComplexQuery {
    static constexpr const char *qname = "SUM";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    std::vector<std::string> getOperands() { return operands; }
};

class SumTask : public Task {
    std::vector<int> fieldSums;
protected:
    LEMONDB_QUERY_PTR(SumQuery);
public:
    using Task::Task;
    void execute() override;
    std::vector<int> getFieldSums() { return fieldSums; }
};

#endif //LEMONDB_SUM_QUERY_H
