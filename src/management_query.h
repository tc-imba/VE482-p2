#ifndef SRC_MANAGEMENT_QUERY_H
#define SRC_MANAGEMENT_QUERY_H

#include "query/query.h"


class ListTableQuery : public Query {
    static constexpr const char *qname = "LIST";
public:
    LEMONDB_QUERY_WRITER(false)
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class PrintTableQuery : public Query {
    static constexpr const char *qname = "SHOWTABLE";
    const std::string tableName;
public:
    LEMONDB_QUERY_WRITER(false)
    PrintTableQuery(std::string table)
            : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
};





#endif //SRC_MANAGEMENT_QUERY_H
