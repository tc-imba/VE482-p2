//
// Created by linzhi on 2017/11/7.
//

#ifndef LEMONDB_SWAP_QUERY_H
#define LEMONDB_SWAP_QUERY_H
#include "../query.h"
#include "../task.h"

class SwapTask;

class SwapQuery : public ComplexQuery {
    static constexpr const char *qname = "SWAP";
    Table::FieldIndex operand1;
    Table::FieldIndex operand2;
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class SwapTask;
//    std::string firstField() { return this->operands[0]; }
//    std::string secondField() { return this->operands[1]; }
};

class SwapTask : public Task {
protected:
    LEMONDB_QUERY_PTR(SwapQuery);
public:
    using Task::Task;
    void execute() override;
    friend class SwapQuery;
};

#endif //LEMONDB_SWAP_QUERY_H
