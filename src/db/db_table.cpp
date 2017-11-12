#include <algorithm>
#include <iostream>
#include "db_table.h"
#include "db.h"
#include "../uexception.h"
#include "../formatter.h"
#include "../query/management/load_table_query.h"

constexpr const Table::ValueType Table::ValueTypeMax;
constexpr const Table::ValueType Table::ValueTypeMin;

Table &loadTableFromStream(std::istream &infile, std::string source) {
    auto &db = Database::getInstance();
    std::string errString =
            !source.empty() ?
            R"(Invalid table (from "?") format: )"_f % source :
            "Invalid table format: ";

    std::string tableName;
    int fieldCount;
    std::vector<Table::KeyType> fields;

    std::string line;
    std::stringstream sstream;
    if (!std::getline(infile, line))
        throw LoadFromStreamException(
                errString + "Failed to read table metadata line."
        );

    sstream.str(line);
    sstream >> tableName >> fieldCount;
    if (!sstream) {
        throw LoadFromStreamException(
                errString + "Failed to parse table metadata."
        );
    }

    if (!(std::getline(infile, line)))
        throw LoadFromStreamException(
                errString + "Failed to load field names."
        );

    sstream.str("");
    sstream.str(line);
    sstream.clear();
    for (int i = 0; i < fieldCount; ++i) {
        std::string field;
        if (!(sstream >> field))
            throw LoadFromStreamException(
                    errString + "Failed to load field names."
            );
        else
            fields.push_back(field);
    }

    if (fields.front() != "KEY")
        throw LoadFromStreamException(
                errString + "Missing or invalid KEY field."
        );

    fields.erase(fields.begin()); // Remove leading key
    //Table::Ptr table = std::make_unique<Table>(tableName, fields);
    auto &table = db.ensureTable(tableName);

    int count = 2;
    while (std::getline(infile, line)) {
        if (line.empty()) break; // Read to an empty line
        count++;
        sstream.str("");
        sstream.clear();
        sstream.str(line);
        std::string key;
        if (!(sstream >> key))
            throw LoadFromStreamException(
                    errString + "Missing or invalid KEY field."
            );
        std::vector<int> tuple;
        for (int i = 1; i < fieldCount; ++i) {
            int value;
            if (!(sstream >> value))
                throw LoadFromStreamException(
                        errString + "Invalid row on LINE " + std::to_string(count)
                );
            tuple.push_back(value);
        }
        table.insertByIndex(key, tuple);
    }

    table.init(fields);

    return table;
}

std::ostream &operator<<(std::ostream &os, const Table &table) {
    const int width = 10;
    std::stringstream buffer;
    buffer << table.tableName << "\t" << (table.fields.size() + 1) << "\n";
    buffer << std::setw(width) << "KEY";
    for (const auto &field : table.fields) {
        buffer << std::setw(width) << field;
    }
    buffer << "\n";
    auto numFields = table.fields.size();
    for (const auto &datum : table.data) {
        buffer << std::setw(width) << datum.key;
        for (int i = 0; i < numFields; ++i) {
            buffer << std::setw(width) << datum.datum[i];
        }
        buffer << "\n";
    }
    return os << buffer.str();
}

void Table::addQuery(Query *query) {
    queryQueueMutex.lock();
    if (query->isInstant() && !initialized && queryQueue.empty()) {
        queryQueueMutex.unlock();
        query->execute();
        return;
    }
    if (queryQueueCounter < 0 || !initialized) {
        // writing, push back the query
        //fprintf(stderr, "Enqueue %d\n", query->getId());
        queryQueue.push_back(query);
        queryQueueMutex.unlock();
        return;
    }
    // idle/reading
    if (query->isWriter()) {
        // add a writer or execute it if idle
        if (queryQueueCounter == 0 && queryQueue.empty()) {
            queryQueueCounter = -1;
            queryQueueMutex.unlock();
            query->execute();
        } else {
            //fprintf(stderr, "Enqueue %d\n", query->getId());
            queryQueue.push_back(query);
            queryQueueMutex.unlock();
        }
    } else {
        if (queryQueueCounter >= 0 && queryQueue.empty()) {
            // add a reader and execute it at once if queue is empty
            ++queryQueueCounter;
            queryQueueMutex.unlock();
            query->execute();
        } else {
            //fprintf(stderr, "Enqueue %d\n", query->getId());
            queryQueue.push_back(query);
            queryQueueMutex.unlock();
        }
    }

}

void Table::completeQuery() {
    queryQueueMutex.lock();
    if (!queryQueue.empty() && !initialized && queryQueue.front()->isInstant()  ) {
        queryQueueMutex.unlock();
        auto query = queryQueue.front();
        queryQueue.pop_front();
        queryQueueMutex.unlock();
        query->execute();
        return;
    }
    if (!initialized) {
        queryQueueMutex.unlock();
        return;
    }
    if (queryQueueCounter <= 0) {
        // writing or idle (should not happen), reset the counter
        queryQueueCounter = 0;
    } else {
        --queryQueueCounter;
    }
    if (queryQueue.empty()) {
        queryQueueMutex.unlock();
        return;
    }
    if (queryQueueCounter == 0 && !queryQueue.empty() && queryQueue.front()->isWriter()) {
        // if idle with a write query, execute it
        queryQueueCounter = -1;
        auto query = queryQueue.front();
        queryQueue.pop_front();
        queryQueueMutex.unlock();
        query->execute();
    } else {
        // if reading, execute all read query before next write query
        decltype(queryQueue) list;
        //std::cerr << queryQueue.size() << std::endl;
        // STL may be a bit faster ?
        auto it = std::find_if(queryQueue.begin(), queryQueue.end(), [](const Query *query) {
            return query->isWriter();
        });
        //auto it = queryQueue.begin();
        //for (; it != queryQueue.end() && !(*it)->isWriter(); ++it) {}
        list.splice(list.begin(), queryQueue, queryQueue.begin(), it);
        queryQueueCounter += list.size();
        queryQueueMutex.unlock();
        for (auto &item:list) {
            item->execute();
        }
    }
}

