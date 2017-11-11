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
    Query *query;
    Table *table = nullptr;
    /** Count affected rows in this task */
    Table::SizeType counter = 0;
    Table::Iterator begin, end;
    QueryResult::Ptr errorResult = nullptr;
    virtual TaskQuery *getQuery() const;
public:
    typedef std::unique_ptr<Task> Ptr;

    //Task() = delete;

    explicit Task(Query *query, Table *table = nullptr) :
            query(query), table(table) {};

    Task(Query *query,
         Table *table, Table::Iterator begin, Table::Iterator end) :
            query(query), table(table), begin(begin), end(end) {};

    virtual ~Task() = default;

    virtual void execute();

    Table::SizeType getCounter() {
        return counter;
    };
};

#endif //LEMONDB_TASK_H
