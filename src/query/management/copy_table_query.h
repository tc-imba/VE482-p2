//
// Created by liu on 17-11-12.
//

#ifndef LEMONDB_COPY_TABLE_QUERY_H
#define LEMONDB_COPY_TABLE_QUERY_H

#include "../query.h"
#include "../task.h"

class CopyTableQuery : public TaskQuery {
    static constexpr const char *qname = "COPYTABLE";
    const std::string newTableName;
public:
    LEMONDB_QUERY_WRITER(false)
    CopyTableQuery(std::string table, std::string newTable)
            : TaskQuery(std::move(table)), newTableName(std::move(newTable)) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //LEMONDB_COPY_TABLE_QUERY_H
