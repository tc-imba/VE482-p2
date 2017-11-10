#ifndef SRC_DB_H
#define SRC_DB_H

#include "db_table.h"
#include "../query/query_base.h"
#include "../query/task.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

class Database {
private:
    static std::unique_ptr<Database> instance;

    std::unordered_map<std::string, Table::Ptr> tables;
    std::mutex tablesMutex;

    std::queue<Task *> tasks;
    std::mutex tasksMutex;

    std::vector<std::pair<Query::Ptr, QueryResult::Ptr> > results;
    std::mutex resultsMutex;
    size_t resultNow = 0;
    //std::vector<std::pair<Query::Ptr, QueryResult::Ptr> >::iterator resultNow = results.begin();
    std::mutex outputMutex;

    std::vector<std::thread> threads;
    static const int threadNum = 8;

    bool readyExit = false;

    Database() = default;
    static void threadWork(Database *db);

public:
    void registerTable(Table::Ptr &&table);
    /**
     * get the table if it already exists
     * create a table if tableName not found
     * use tablesMutex, call it when needed
     * if tableName must exist, use operator[]
     * @param tableName
     * @return
     */
    Table &ensureTable(const std::string &tableName);
    void dropTable(std::string tableName);
    void printAllTable();

    Table &operator[](std::string tableName);
    const Table &operator[](std::string tableName) const;

    Database &operator=(const Database &) = delete;
    Database &operator=(Database &&)      = delete;
    Database(const Database &) = delete;
    Database(Database &&) = delete;
    ~Database() = default;

    static Database &getInstance() {
        if (Database::instance == nullptr) {
            instance = std::unique_ptr<Database>(new Database);
            for (int i = 0; i < threadNum; i++) {
                instance->threads.emplace(instance->threads.end(), threadWork, instance.get());
            }
        }
        return *instance;
    }

    /**
     * Add a parsed query after reading it
     * dispatch the query according to its target table
     * @param query
     */
    void addQuery(Query::Ptr &&query);

    /**
     * Add a generated task after a query has been executed by a table
     * idle working threads are waiting for the task
     * @param task
     */
    void addTask(Task *task);

    void addResult(Query *query, QueryResult::Ptr &&result);

    /**
     * try to output the query result in order
     */
    void completeQuery();

    bool isBusy() const {
        return !readyExit;
    }
};

#endif //SRC_DB_H
