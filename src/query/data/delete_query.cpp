//
// Created by admin on 2017/11/2.
//

#include "delete_query.h"

constexpr const char *DeleteQuery::qname;

QueryResult::Ptr DeleteQuery::execute() {
    start();
    using namespace std;
    if (!this->operands.empty())
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    auto &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        if (condition.empty()) {
            auto counter = table.clear();
            complete(std::make_unique<RecordCountResult>(counter));
            return make_unique<SuccessMsgResult>(qname);
        }
        addIterationTask<DeleteTask>(db, table);
        return make_unique<SuccessMsgResult>(qname);
    } catch (const TableNameNotFound &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const IllFormedQueryCondition &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                e.what()
        );
    } catch (const std::invalid_argument &e) {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unknown error '?'"_f % e.what()
        );
    } catch (const std::exception &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unknown error '?'."_f % e.what()
        );
    }
}

std::string DeleteQuery::toString() {
    return "QUERY = DELETE " + this->targetTable + "\"";
}

QueryResult::Ptr DeleteQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    Database &db = Database::getInstance();
    auto &table = db[this->targetTable];
    Table::SizeType counter = 0;
    for (const auto &task:tasks) {
        counter += task->getCounter();
    }
    table.swapData();
    return std::make_unique<RecordCountResult>(counter);
}

void DeleteTask::execute() {
    auto query = getQuery();
    try {
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                table->erase(it);
                counter++;
            } else {
                table->move(it);
            }
        }
        Task::execute();
    } catch (const IllFormedQueryCondition &e) {
        errorResult = std::make_unique<ErrorMsgResult>(
                query->qname, table->name().c_str(), e.what()
        );
        return;
    }
}

