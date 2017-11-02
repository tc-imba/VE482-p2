#ifndef SRC_DATA_QUERY_H
#define SRC_DATA_QUERY_H

#include "query/query.h"


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

class DuplicateQuery : public ComplexQuery {
    static constexpr const char* qname = "DUPLICATE";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class SwapQuery : public ComplexQuery {
    static constexpr const char* qname = "SWAP";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //SRC_DATA_QUERY_H
