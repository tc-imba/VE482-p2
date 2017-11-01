#ifndef SRC_QUERY_H
#define SRC_QUERY_H

#include <string>
#include <memory>
#include "uexception.h"
#include "db_table.h"
#include "query_results.h"

class Query {
public:
    typedef std::unique_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string      toString() = 0;
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
    bool evalCondition(const std::vector<QueryCondition>& conditions,
                       const Table::Object& object);
};



#endif //SRC_QUERY_H
