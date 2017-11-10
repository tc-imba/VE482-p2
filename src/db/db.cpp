#include <iostream>
#include "db.h"
#include "../uexception.h"

std::unique_ptr<Database> Database::instance = nullptr;

void Database::threadWork(Database *db) {
    fprintf(stderr, "Start thread %lu\n", std::this_thread::get_id());
    while (true) {
        db->tasksMutex.lock();
        if (db->tasks.empty()) {
            db->tasksMutex.unlock();
            if (db->readyExit) return;
            std::this_thread::yield();
        } else {
            auto task = std::move(db->tasks.front());
            db->tasks.pop();
            db->tasksMutex.unlock();
            task->execute();
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
        it = tables.emplace(std::make_pair(tableName, std::make_unique<Table>())).first;
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


void Database::addQuery(Query::Ptr &&query) {
    auto q = query.get();
    resultsMutex.lock();
    query->initId((int) results.size());
    results.emplace_back(std::move(query), nullptr);
    resultsMutex.unlock();
    const auto &tableName = q->getTableName();
    if (tableName.empty()) {
        // no-target query
        q->execute();
        std::cerr << q->toString() << std::endl;
        return;
    }
    try {
        auto &table = ensureTable(tableName);
        table.addQuery(q);
    } catch (std::exception &e) {
        std::cerr << "Uncaught error: " << e.what() << std::endl;
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
        pair.second = std::move(result);
    }
    resultsMutex.unlock();
}

void Database::completeQuery() {
    outputMutex.lock();
    for (auto it = results.begin() + resultNow; it != results.end() && it->second != nullptr; ++it) {
        std::cout << it->second->toString() << std::endl;
        ++resultNow;
    }
    outputMutex.unlock();
}
