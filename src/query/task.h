//
// Created by liu on 17-11-6.
//

#ifndef LEMONDB_TASK_H
#define LEMONDB_TASK_H

#include "query_base.h"
#include "../db/db_table.h"
#include "../query_results.h"

class Task {
protected:
    std::shared_ptr<Query> query;
    Table *table = nullptr;
    /** Count affected rows in this task */
    Table::SizeType counter = 0;
    Table::Iterator begin, end;
    QueryResult::Ptr errorResult = nullptr;
    virtual Query *getQuery() { return query.get(); }

    template<class T>
    class GetPtr {
    private:
        Task *task;
    public:
        explicit GetPtr(Task *task) : task(task) {};
        T *operator()() { return dynamic_cast<T *>(task->query.get()); }
    };

public:
    typedef std::shared_ptr<Task> Ptr;

    //Task() = delete;

    explicit Task(std::shared_ptr<Query> query, Table *table = nullptr) :
            query(std::move(query)), table(table) {};

    Task(std::shared_ptr<Query> query,
         Table *table, Table::Iterator begin, Table::Iterator end) :
            query(std::move(query)), table(table), begin(begin), end(end) {};

    virtual ~Task() = default;

    virtual void execute();

    Table::SizeType getCounter() {
        return counter;
    };
};

#endif //LEMONDB_TASK_H
