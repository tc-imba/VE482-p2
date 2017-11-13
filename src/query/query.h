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
    size_t fieldId;
    std::string op;
    std::function<bool(const Table::ValueType &, const Table::ValueType &)> comp;
    std::string value;
    Table::ValueType valueParsed;
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
    size_t tasksSize = 1;
    std::mutex tasksMutex;
    /**
     * Count the completed tasks
     * atomic is used because only ++ and < is applied
     * spin lock will be faster than mutex
     * (have bug, use int now)
     */
    int taskComplete = 0;
public:
    TaskQuery() = default;
    explicit TaskQuery(std::string targetTable) { this->targetTable = std::move(targetTable); }

    Task *getTask(size_t index) const { return tasks[index].get(); }
    Task *getTask(const std::vector<std::unique_ptr<Task> >::iterator &it) const { return it->get(); }

    void start();
    void complete();
    void complete(QueryResult::Ptr &&result);

    template<class TaskType>
    void addIterationTask(Database &db, Table &table) {
        auto begin = table.begin();
        decltype(begin) end;
        auto size = table.size();
        tasksMutex.lock();
        const int part = 10000;
        if (size == 0) {
            auto task = std::unique_ptr<TaskType>(new TaskType(this, &table, begin, begin));
            auto t = task.get();
            tasks.emplace_back(std::move(task));
            db.addTask(t);
        } else {
            tasksSize = (size - 1) / part + 1;
            while (size > 0) {
                if (size >= part) {
                    size -= part;
                    end = begin + part;
                } else {
                    size = 0;
                    end = table.end();
                }
                auto task = std::unique_ptr<TaskType>(new TaskType(this, &table, begin, end));
                begin = end;
                auto t = task.get();
                tasks.emplace_back(std::move(task));
                db.addTask(t);
            }
        }
        tasksMutex.unlock();
    }

    template<class TaskType>
    void addUniqueTask(Database &db, Table *table = nullptr) {
        auto task = std::unique_ptr<TaskType>(new TaskType(this, table));
        auto t = task.get();
        tasks.emplace_back(std::move(task));
        db.addTask(t);
    }
};

class ComplexQuery : public TaskQuery {
protected:
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;

public:
    bool evalCondition(const std::vector<QueryCondition> &conditions,
                       const Table::Object &object);

    /**
     * init a fast condition according to the table
     * note that the condition is only effective if the table fields are not changed
     * @param table
     * @param conditions
     * @return a pair of the key and a flag
     * if flag is false, the condition is always false
     * in this situation, the condition may not be fully initialized to save time
     */
    std::pair<std::string, bool> initConditionFast(const Table &table);

    /**
     * skip the evaluation of KEY
     * (which should be done after initConditionFast is called)
     * @param conditions
     * @param object
     * @return
     */
    bool evalConditionFast(const Table::Object &object);

    bool testKeyCondition(Table &table, std::function<void(bool, Table::Object::Ptr &&)> function);

    typedef std::unique_ptr<ComplexQuery> Ptr;

    ComplexQuery(std::string targetTable,
                 std::vector<std::string> operands,
                 std::vector<QueryCondition> condition)
            : TaskQuery(std::move(targetTable)),
              operands(std::move(operands)),
              condition(std::move(condition)) {
    }

    const std::vector<std::string> &getOperands() const { return operands; }
    const std::vector<QueryCondition> &getCondition() { return condition; }
};


#endif //LEMONDB_QUERY_H
