#ifndef SRC_MANAGEMENT_QUERY_H
#define SRC_MANAGEMENT_QUERY_H

#include "query/query.h"


class DropTableQuery : public Query {
    static constexpr const char *qname = "DROP";
    const std::string tableName;
public:
    LEMONDB_QUERY_WRITER(true)
    DropTableQuery(std::string table) : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class DumpTableQuery : public Query {
    static constexpr const char *qname = "DUMP";
    const std::string tableName;
    const std::string fileName;
public:
    LEMONDB_QUERY_WRITER(false)
    DumpTableQuery(std::string table, std::string file)
            : tableName(table), fileName(file) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

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
