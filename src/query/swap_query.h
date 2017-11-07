//
// Created by 59889 on 2017/11/7.
//

#ifndef LEMONDB_SWAP_QUERY_H
#define LEMONDB_SWAP_QUERY_H
#include "query.h"
#include "task.h"

class SwapQuery : public ComplexQuery {
    static constexpr const char *qname = "SWAP";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;

    std::string firstField() { return this->operands[0]; }
    std::string secondField() { return this->operands[1]; }
};

class SwapTask : public Task {
private:
    SwapQuery *getQuery() {
        return (SwapQuery *) query.get();
    }
public:
    SwapTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};

#endif //LEMONDB_SWAP_QUERY_H
