#ifndef LEMONDB_UPDATE_QUERY_H
#define LEMONDB_UPDATE_QUERY_H

#include "query.h"
#include "task.h"

class UpdateQuery : public ComplexQuery {
    static constexpr const char *qname = "UPDATE";
    Table::ValueType fieldValue = (operands[0]=="KEY")? 0 :std::stoi(operands[1]);
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    Table::ValueType getFieldValue() { return fieldValue; } 
};

class UpdateTask : public Task {
private:
    UpdateQuery *getQuery() {
        return (UpdateQuery *) query.get();
    }
public:
    friend class ComplexQuery;
    UpdateTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};


#endif