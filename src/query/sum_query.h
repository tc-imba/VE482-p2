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
    SumTask *getTask(int index) {
        return (SumTask*) tasks[index].get();
    }
};

class SumTask : public Task {
private:
    SumQuery *getQuery() {
        return (SumQuery *) query.get();
    }
    std::vector<int> fieldSums;
public:
    SumTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
    std::vector<int> getFieldSums() { return fieldSums; }
};

#endif //LEMONDB_SUM_QUERY_H
