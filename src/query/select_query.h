//
// Created by linzhi on 11/6/17.
//

#ifndef LEMONDB_SELECT_QUERY_H
#define LEMONDB_SELECT_QUERY_H

#include <algorithm>
#include "query.h"
#include "task.h"

class SelectQuery : public ComplexQuery {
    static constexpr const char *qname = "SELECT";
    std::vector<Table::resultArray> taskResults;
    std::unordered_map<Task*, int> taskToIndex;
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;

    int whichTask (Task *ptr) {return taskToIndex[ptr];}

    std::vector<std::string> &getOperands () {return operands;}

    void pushToResultArray (int index, const Table::KeyType &key, std::vector<Table::ValueType> &&tuple) { taskResults[index].push_back(key, std::move(tuple)); }

    void sortResultArray (int index) {
        std::sort(taskResults[index].begin(), taskResults[index].end(), taskResults[index].comp);
    }
};

class SelectTask : public Task {
protected:
    LEMONDB_QUERY_PTR(SelectQuery);
public:
    using Task::Task;
    void execute() override ;
};

#endif //LEMONDB_SELECT_QUERY_H
