#ifndef SRC_DATA_QUERY_H
#define SRC_DATA_QUERY_H

#include "query.h"

class ComplexQuery : public ConditionedQuery {
protected:
    std::string targetTable;
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;
public:
    ComplexQuery(const std::string& targetTable,
                 const std::vector<std::string>& operands,
                 const std::vector<QueryCondition>& condition)
            : targetTable(targetTable),
              operands(operands),
              condition(condition) { }
};

// Here follows the basic CURD Queries:
// C - Create (insert) an entry
// U - Update (existing query)
// R - Read   (read existsing queries, implemented as dump)
// D - Delete (delete existing query)

class UpdateQuery : public ComplexQuery {
    static constexpr const char* qname = "UPDATE";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class DeleteQuery : public ComplexQuery {
    static constexpr const char* qname = "DELETE";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};


class InsertQuery : public ComplexQuery {
    static constexpr const char* qname = "INSERT";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class SelectQuery : public ComplexQuery {
    static constexpr const char* qname = "SELECT";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};


#endif //SRC_DATA_QUERY_H
