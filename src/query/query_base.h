//
// Created by 38569 on 2017/11/5.
//

#ifndef LEMONDB_QUERY_BASE_H
#define LEMONDB_QUERY_BASE_H

#include <string>
#include <memory>
#include "../query_results.h"

#define LEMONDB_QUERY_WRITER(flag) bool isWriter() override { return flag; }
//#define LEMONDB_QUERY_CAST(type) (type *)getQuery() override { return dynamic_cast<(type) *>(query.get()); }

class Query {
protected:
    std::string targetTable;
public:
    typedef std::shared_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string toString() = 0;
    virtual ~Query() = default;
    virtual bool isWriter() = 0;

    const std::string &getTableName() { return targetTable; }
};

class Task;

class ComplexQuery;

#endif //LEMONDB_QUERY_BASE_H
