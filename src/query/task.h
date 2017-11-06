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
    std::shared_ptr<ComplexQuery> query;
    Table &table;
    /** Count affected rows in this task */
    Table::SizeType counter = 0;
    Table::Iterator begin, end;
    QueryResult::Ptr errorResult = nullptr;
public:
    typedef std::shared_ptr<Task> Ptr;

    Task() = delete;

    Task(const std::shared_ptr<ComplexQuery> &query,
         Table &table, Table::Iterator begin, Table::Iterator end) :
            query(query), table(table) {};

    virtual ~Task() = default;

    virtual void execute();

    Table::SizeType getCounter() {
        return counter;
    };
};

#endif //LEMONDB_TASK_H
