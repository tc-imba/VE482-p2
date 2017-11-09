//
// Created by 38569 on 2017/11/7.
//

#ifndef LEMONDB_LOAD_QUERY_H
#define LEMONDB_LOAD_QUERY_H

#include "query.h"
#include "task.h"

class LoadTableQuery : public Query {
    static constexpr const char *qname = "LOAD";
    const std::string fileName;
public:
    explicit LoadTableQuery(std::string file) : fileName(std::move(file)) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
    friend class LoadTableTask;
};

class LoadTableTask : public Task {
protected:
    LoadTableQuery *getQuery() override { return dynamic_cast<LoadTableQuery*>(query.get()); }
public:
    explicit LoadTableTask(const std::shared_ptr<Query> &query) : Task(query) {};
    void execute() override;
};

#endif //LEMONDB_LOAD_QUERY_H
