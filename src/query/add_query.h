#ifndef LEMONDB_ADD_QUERY_H
#define LEMONDB_ADD_QUERY_H

#include "query.h"
#include "task.h"

class AddQuery : public ComplexQuery {
    static constexpr const char *qname = "ADD";
private:
    bool writer = true;
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    std::vector<std::string> getOperands() { return operands; }
};

class AddTask : public Task {
private:
    AddQuery *getQuery() {
        return (AddQuery *) query.get();
    }
public:
    AddTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};

#endif //LEMONDB_ADD_QUERY_H