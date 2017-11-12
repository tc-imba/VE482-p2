//
// Created by liu on 17-11-12.
//

#ifndef LEMONDB_COPY_TABLE_QUERY_H
#define LEMONDB_COPY_TABLE_QUERY_H

#include "../query.h"
#include "../task.h"

class CopyTableDestQuery;

class CopyTableQuery : public TaskQuery {
    static constexpr const char *qname = "COPYTABLE";
    const std::string destTableName;
    CopyTableDestQuery *destQuery = nullptr;
public:
    LEMONDB_QUERY_WRITER(false);
    CopyTableQuery(std::string table, std::string newTable)
            : TaskQuery(std::move(table)), destTableName(std::move(newTable)) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
    Query::Ptr createDestQuery();
};

class CopyTableDestQuery : public TaskQuery {
    static constexpr const char *qname = "COPYTABLE";
    CopyTableQuery *srcQuery;
    bool ready = false;
public:
    LEMONDB_QUERY_WRITER(true);
    LEMONDB_QUERY_INSTANT(true);
    CopyTableDestQuery(std::string table, CopyTableQuery *srcQuery)
            : TaskQuery(std::move(table)), srcQuery(srcQuery) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;

    friend class CopyTableQuery;
};

#endif //LEMONDB_COPY_TABLE_QUERY_H
