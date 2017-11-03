#ifndef LEMONDB_QUERY_H
#define LEMONDB_QUERY_H

#include <string>
#include <memory>
#include "../uexception.h"
#include "../db_table.h"
#include "../query_results.h"
#include "../task_results.h"

class Task;

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
public:
    bool evalCondition(const std::vector<QueryCondition> &conditions,
                       const Table::Object &object);
};

class ComplexQuery : public ConditionedQuery {
protected:
    std::string targetTable;
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;
    std::vector<std::shared_ptr<Task> > tasks;
public:
    typedef std::shared_ptr<ComplexQuery> Ptr;

    ComplexQuery(const std::string &targetTable,
                 const std::vector<std::string> &operands,
                 const std::vector<QueryCondition> &condition)
            : targetTable(targetTable),
              operands(operands),
              condition(condition) {}

    const std::vector<QueryCondition> &getCondition() {
        return condition;
    }

    void combine();
};

class Task {
    Task() = default;
protected:
    std::shared_ptr<ComplexQuery> query;
    Table &table;
    /** Count affected rows in this task */
    Table::SizeType counter = 0;
    Table::Iterator begin, end;
public:
    typedef std::shared_ptr<Task> Ptr;
    explicit Task(const std::shared_ptr<ComplexQuery> &query, Table &table) :
            query(query), table(table) {};
    virtual void execute() = 0;
    virtual ~Task() = default;
};


#endif //LEMONDB_QUERY_H
