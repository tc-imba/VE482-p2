#ifndef LEMONDB_QUERY_H
#define LEMONDB_QUERY_H

#include <string>
#include <memory>
#include <mutex>
#include "query_base.h"
#include "../uexception.h"
#include "../db/db_table.h"
#include "../query_results.h"
#include "../db/db.h"

struct QueryCondition {
    std::string field;
    std::string op;
    std::string value;
};

class NopQuery : public Query {
public:
    QueryResult::Ptr execute() override {
        return std::make_unique<NullQueryResult>();
    }

    std::string toString() override {
        return "QUERY = NOOP";
    }
};

class TaskQuery : public Query {
protected:
    std::vector<std::unique_ptr<Task> > tasks;
    std::mutex tasksMutex;
    /**
     * Count the completed tasks
     * atomic is used because only ++ and < is applied
     * spin lock will be faster than mutex
     */
    std::atomic_int taskComplete{0};
public:
    Task *getTask(size_t index) const { return tasks[index].get(); }
    Task *getTask(const std::vector<std::unique_ptr<Task> >::iterator &it) const { return it->get(); }

    void complete();

    template<class TaskType>
    void addIterationTask(Database &db, Table &table) {
        auto begin = table.begin();
        decltype(begin) end;
        auto size = table.size();
        tasksMutex.lock();
        while (size > 0) {
            if (size >= 100000) {
                size -= 100000;
                end = begin + 100000;
            } else {
                size = 0;
                end = table.end();
            }
            auto task = std::unique_ptr<Task>(new TaskType(this, &table, begin, end));
            db.addTask(task.get());
            tasks.emplace_back(std::move(task));
        }
        tasksMutex.unlock();
    }

    template<class TaskType>
    void addUniqueTask(Database &db, Table *table = nullptr) {
        auto task = std::unique_ptr<Task>(new TaskType(this, table));
        db.addTask(task.get());
        tasks.emplace_back(std::move(task));
    }
};

class ComplexQuery : public TaskQuery {
protected:
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;

public:
    bool evalCondition(const std::vector<QueryCondition> &conditions,
                       const Table::Object &object);


    typedef std::unique_ptr<ComplexQuery> Ptr;

    ComplexQuery(std::string targetTable,
                 std::vector<std::string> operands,
                 std::vector<QueryCondition> condition)
            : operands(std::move(operands)),
              condition(std::move(condition)) {
        this->targetTable = std::move(targetTable);
    }

    const std::vector<std::string> &getOperands() const { return operands; }
    const std::vector<QueryCondition> &getCondition() { return condition; }
};


#endif //LEMONDB_QUERY_H
