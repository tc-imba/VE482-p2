//
// Created by liu on 2017/11/5.
//

#ifndef LEMONDB_QUERY_BASE_H
#define LEMONDB_QUERY_BASE_H

#include <string>
#include <memory>
#include <atomic>
#include "../query_results.h"

#define LEMONDB_QUERY_WRITER(flag) bool isWriter() const override { return flag; }
#define LEMONDB_QUERY_PTR(T) T* getQuery() override { return dynamic_cast<T*>(query); }

class Query {
protected:
    std::string targetTable;
    int id = -1;
public:
    typedef std::unique_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string toString() = 0;

    virtual QueryResult::Ptr combine() {
        return nullptr;
    };

    virtual ~Query() = default;
    virtual bool isWriter() const = 0;

    const std::string &getTableName() { return targetTable; }

    /**
     * get the unique id of this query
     * @return
     */
    int getId() const { return id; }

    /**
     * will only work when first init the query id
     * @param id
     * @return
     */
    int initId(int id) {
        if (this->id < 0 && id >= 0) this->id = id;
    }
};

class Task;

class NopQuery;

class TaskQuery;

class ComplexQuery;

#endif //LEMONDB_QUERY_BASE_H
