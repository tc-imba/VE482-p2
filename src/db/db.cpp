#include <iostream>
#include "db.h"
#include "../uexception.h"
#include "../query/management/copy_table_query.h"
#include "../query/management/quit_query.h"

std::unique_ptr<Database> Database::instance = nullptr;

void Database::threadWork(Database *db) {
    //fprintf(stderr, "Start thread %lu\n", std::this_thread::get_id());
    while (true) {
        db->tasksMutex.lock();
        if (db->tasks.empty()) {
            db->tasksMutex.unlock();
            if (db->readyExit) {
                //fprintf(stderr, "Terminate thread %lu\n", std::this_thread::get_id());
                //std::terminate();
                return;
            }
            std::this_thread::yield();
        } else {
            auto task = db->tasks.front();
            db->tasks.pop();
            db->tasksMutex.unlock();
            try {
                task->execute();
            } catch (std::exception &e) {
                fprintf(stderr, "%s %s", e.what(), task->query->toString());
            }
        }
    }
}

void Database::registerTable(Table::Ptr &&table) {
    auto name = table->name();
    if (this->tables.find(name) != this->tables.end())
        throw DuplicatedTableName(
                "Error when inserting table \"" + name + "\". Name already exists."
        );
    this->tables[name] = std::move(table);
}

Table &Database::ensureTable(const std::string &tableName) {
    tablesMutex.lock();
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        // table doesn't exist, add the table
        it = tables.emplace(std::make_pair(tableName, std::make_unique<Table>(tableName))).first;
    }
    tablesMutex.unlock();
    return *(it->second);
}

Table &Database::operator[](std::string tableName) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    return *(it->second);
}

const Table &Database::operator[](std::string tableName) const {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    return *(it->second);
}

void Database::dropTable(std::string name) {
    auto it = this->tables.find(name);
    if (it == this->tables.end())
        throw TableNameNotFound(
                "Error when trying to drop table \"" + name + "\". Table not found."
        );
    this->tables.erase(it);
}

void Database::printAllTable() {
    std::cout << "Database overview:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << std::setw(15) << "Table name";
    std::cout << std::setw(15) << "# of fields";
    std::cout << std::setw(15) << "# of entries" << std::endl;
    for (const auto &table : this->tables) {
        std::cout << std::setw(15) << table.first;
        std::cout << std::setw(15) << (*table.second).field().size() + 1;
        std::cout << std::setw(15) << (*table.second).size() << std::endl;
    }
    std::cout << "Total " << this->tables.size() << " tables." << std::endl;
    std::cout << "=========================" << std::endl;
}

void Database::updateFileTableName(const std::string &fileName, const std::string &tableName) {
    fileTableNameMap[fileName] = tableName;
}

std::string Database::getFileTableName(const std::string &fileName) {
    auto it = fileTableNameMap.find(fileName);
    if (it == fileTableNameMap.end()) {
        std::ifstream infile(fileName);
        if (!infile.is_open()) {
            return "";
        }
        std::string tableName;
        infile >> tableName;
        infile.close();
        fileTableNameMap.emplace(fileName, tableName);
        return tableName;
    } else {
        return it->second;
    }
}

void Database::addQuery(Query::Ptr &&query) {
    auto q = query.get();
    resultsMutex.lock();
    query->initId((int) results.size());
    results.emplace_back(std::move(query), nullptr);
    resultsMutex.unlock();
    const auto &tableName = q->getTableName();
    if (tableName.empty()) {
        // no-target query (only exit)
        if (typeid(*q) == typeid(QuitQuery)) {
            q->execute();
        } else {
            std::cerr << "Query with no targetTable: " << q->toString() << std::endl;
            endQuery();
        }
        return;
    }
    try {
        auto &table = ensureTable(tableName);
        table.addQuery(q);
        if (typeid(*q) == typeid(CopyTableQuery)) {
            addQuery(dynamic_cast<CopyTableQuery *>(q)->createDestQuery());
        }
    } catch (std::exception &e) {
        std::cerr << "Uncaught error: " << e.what() << std::endl;
        endQuery();
    }
}

void Database::addTask(Task *task) {
    tasksMutex.lock();
    tasks.push(task);
    tasksMutex.unlock();
}

void Database::addResult(Query *query, QueryResult::Ptr &&result) {
    resultsMutex.lock();
    auto id = query->getId();
    if (id >= 0) {
        auto &pair = results.at((size_t) id);
        if (pair.second != nullptr) {
            fprintf(stderr, "Multiple result: %d %s\n", query->getId(), query->toString().c_str());
        }
        pair.second = std::move(result);
    }
    resultsMutex.unlock();
}

void Database::completeQuery() {
    outputMutex.lock();
    for (auto it = results.begin() + resultNow; it != results.end() && it->second != nullptr; ++it) {
        // SuccessMsgResult should not output
        if (typeid(*(it->second)) == typeid(SuccessMsgResult) || typeid(*(it->second)) == typeid(NullQueryResult)) {
            //std::cout.flush();
            //std::cerr << it->second->toString() << std::endl;
        } else {
            //std::cout << it->first->getId() << " " << it->first->toString() << " ";
            std::cout << it->second->toString() << std::endl;
        }
        // Delete the query after output
        //it->first.reset();
        ++resultNow;
    }
    if (endInput && resultNow == results.size()) {
        readyExit = true;
    }
    outputMutex.unlock();
}
