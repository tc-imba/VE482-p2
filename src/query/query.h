#ifndef LEMONDB_QUERY_H
#define LEMONDB_QUERY_H

#include <string>
#include <memory>
#include "../uexception.h"
#include "../db_table.h"
#include "../query_results.h"
#include "../task_results.h"

class Query {
public:
    typedef std::shared_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string toString() = 0;
    virtual ~Query() = default;
};

struct QueryCondition {
    std::string field;
    std::string op;
    std::string value;
};

class NopQuery : public Query {
public:
    QueryResult::Ptr execute() override {
        return std::make_unique<NullQueryResult>();
    }

    std::string toString() override {
        return "QUERY = NOOP";
    }
};


class ConditionedQuery : public Query {
protected:
    bool evalCondition(const std::vector<QueryCondition> &conditions,
                       const Table::Object &object);
};

class ComplexQuery : public ConditionedQuery {
protected:
    std::string targetTable;
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;
public:
    ComplexQuery(const std::string &targetTable,
                 const std::vector<std::string> &operands,
                 const std::vector<QueryCondition> &condition)
            : targetTable(targetTable),
              operands(operands),
              condition(condition) {}
};

class Task {
    Query::Ptr query;
    virtual QueryResult::Ptr execute() = 0;
    virtual ~Task() = default;
};


#endif //LEMONDB_QUERY_H
