//
// Created by liu on 2017/10/27.
//

#ifndef LEMONDB_ARITHMETICS_QUERY_H
#define LEMONDB_ARITHMETICS_QUERY_H

#include "data_query.h"

class CountQuery: public ComplexQuery {
    static constexpr const char* qname = "COUNT";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class AddQuery: public ComplexQuery {
    static constexpr const char* qname = "ADD";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class SubQuery: public ComplexQuery {
    static constexpr const char* qname = "SUB";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class SumQuery: public ComplexQuery {
    static constexpr const char* qname = "SUM";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class MinQuery: public ComplexQuery {
    static constexpr const char* qname = "MIN";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class MaxQuery: public ComplexQuery {
    static constexpr const char* qname = "MAX";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};



#endif //LEMONDB_ARITHMETICS_QUERY_H
