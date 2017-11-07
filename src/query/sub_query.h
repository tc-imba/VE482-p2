#ifndef LEMONDB_SUB_QUERY_H
#define LEMONDB_SUB_QUERY_H

#include "query.h"
#include "task.h"

class SubQuery : public ComplexQuery {
    static constexpr const char *qname = "SUB";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    std::vector<std::string> getOperands() { return operands; }
};

class SubTask : public Task {
private:
    SubQuery *getQuery() {
        return (SubQuery *) query.get();
    }
public:
    SubTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};

#endif //LEMONDB_SUB_QUERY_H