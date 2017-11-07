//
// Created by linzhi on 11/6/17.
//

#ifndef LEMONDB_SELECT_QUERY_H_H
#define LEMONDB_SELECT_QUERY_H_H

#include "query.h"
#include "task.h"

class SelectQuery : public ComplexQuery {
    static constexpr const char *qname = "INSERT";
private:
    std::vector<Table::resultArray> taskResults;
    std::unordered_map<Task::Ptr, int> taskToIndex;
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;

    int whichTask (const Task::Ptr &ptr) {return taskToIndex[ptr];}

    std::vector<std::string> &getOperands () {return operands;}

    void pushToResultArray (int index, const Table::KeyType &key, std::vector<Table::ValueType> &&tuple) { taskResults[index].push_back(key, std::move(tuple)); }

    void sortResultArray (int index) {
        std::sort(taskResults[index].begin(), taskResults[index].end(), taskResults[index].comp);
    }
};

class SelectTask : public Task {
private:
    SelectQuery *getQuery() {
        return (SelectQuery *) query.get();
    }
public:
    SelectTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override ;
};

#endif //LEMONDB_SELECT_QUERY_H_H
