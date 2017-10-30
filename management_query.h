#ifndef SRC_MANAGEMENT_QUERY_H
#define SRC_MANAGEMENT_QUERY_H

#include "query.h"

class LoadTableQuery : public Query {
    static constexpr const char* qname = "LOAD";
    const std::string fileName;
public:
    LoadTableQuery(std::string file) : fileName(file) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
};

class DropTableQuery : public Query {
    static constexpr const char* qname = "DROP";
    const std::string tableName;
public:
    DropTableQuery(std::string table) : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
};

class DumpTableQuery : public Query {
    static constexpr const char* qname = "DUMP";
    const std::string tableName;
    const std::string fileName;
public:
    DumpTableQuery(std::string table, std::string file)
            : tableName(table), fileName(file) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
};

class ListTableQuery : public Query {
    static constexpr const char* qname = "LIST";
public:
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

class PrintTableQuery : public Query {
    static constexpr const char* qname = "SHOWTABLE";
    const std::string tableName;
public:
    PrintTableQuery(std::string table)
            : tableName(table) {}
    QueryResult::Ptr execute()  override;
    std::string      toString() override;
};

class TruncateTableQuery : public Query {
    static constexpr const char* qname = "TRUNCATE";
    const std::string tableName;
public:
    TruncateTableQuery(std::string table) : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
};

class CopytableTableQuery : public Query {
    static constexpr const char* qname = "COPYTABLE";
    const std::string tableName;
    const std::string newTableName;
public:
    CopytableTableQuery(std::string table, std::string newTable)
            : tableName(table), newTableName(newTable) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
};
#endif //SRC_MANAGEMENT_QUERY_H
