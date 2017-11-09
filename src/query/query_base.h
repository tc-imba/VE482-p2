//
// Created by liu on 2017/11/5.
//

#ifndef LEMONDB_QUERY_BASE_H
#define LEMONDB_QUERY_BASE_H

#include <string>
#include <memory>
#include <atomic>
#include "../query_results.h"

#define LEMONDB_QUERY_WRITER(flag) bool isWriter() override { return flag; }
#define LEMONDB_QUERY_PTR(T) T* getQuery() override { return dynamic_cast<T*>(query.get()); }

class Query {
protected:
    std::string targetTable;
public:
    typedef std::shared_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string toString() = 0;
    /**
     * Count the completed tasks
     * atomic is used because only ++ and < is applied
     * spin lock will be faster than mutex
     */
    std::atomic_int taskComplete{0};

    void complete() {
        /**
         * @TODO add the complete query to the result vector here
         * should add a unique id for each query
         * should add a function to print results in correct order
         */
        ++taskComplete;
        auto result = combine();
        if (result != nullptr) {

        }
    }

    virtual QueryResult::Ptr combine() {
        return nullptr;
    };

    virtual ~Query() = default;
    virtual bool isWriter() = 0;
    const std::string &getTableName() { return targetTable; }
};

class Task;

class ComplexQuery;

#endif //LEMONDB_QUERY_BASE_H
