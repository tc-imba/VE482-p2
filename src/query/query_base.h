//
// Created by 38569 on 2017/11/5.
//

#ifndef LEMONDB_QUERY_BASE_H
#define LEMONDB_QUERY_BASE_H

#include <string>
#include <memory>
#include "../query_results.h"

class Query {
private:
    bool writer = false;
protected:
    std::string targetTable;
public:
    typedef std::shared_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string toString() = 0;
    virtual ~Query() = default;
    bool isWriter() { return writer; }
    const std::string &getTableName() { return targetTable; }
};

#endif //LEMONDB_QUERY_BASE_H
