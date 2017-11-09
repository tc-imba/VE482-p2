//
// Created by linzhi on 11/6/17.
//

#include <algorithm>
#include "select_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"

constexpr const char *SelectQuery::qname;

QueryResult::Ptr SelectQuery::execute() {
    using namespace std;
    if (this->operands.empty() || this->operands[0] != "KEY")
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands) or without leading KEY."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        addIterationTask<SelectTask>(db, table);
        for (int i = 0; i < tasks.size(); ++i) {
            taskResults.emplace_back();
            taskToIndex.insert({tasks[i].get(),i});
        }
        return make_unique<SuccessMsgResult>("");
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const IllFormedQueryCondition &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                e.what()
        );
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unknown error '?'"_f % e.what()
        );
    } catch (const exception &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unkonwn error '?'."_f % e.what()
        );
    }
}

std::string SelectQuery::toString() {
    return "QUERY = SELECT " + this->targetTable + "\"";
}

QueryResult::Ptr SelectQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    for (int i = 1; i < tasks.size(); ++i) {
        auto it = taskResults[0].insert(taskResults[0].end(), taskResults[i].begin(),taskResults[i].end());
        std::inplace_merge(taskResults[0].begin(), it, taskResults[0].end(), taskResults[0].comp);
    }
    //TODO: output taskResults[0] somehow
    return make_unique<SuccessMsgResult>("");
}

void SelectTask::execute() {
    auto query = getQuery();
    try {
        int index = this->getQuery()->whichTask(this);
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                std::vector<int> tuple;
                for (auto &operand : query->getOperands()) {
                    tuple.push_back((*it)[operand]);
                }
                this->getQuery()->pushToResultArray(index, it->key(), std::move(tuple));
            }
        }
        this->getQuery()->sortResultArray(index);
        Task::execute();
    } catch (const IllFormedQueryCondition &e) {
        return;
        // @TODO manage query exceptions later
        /*return std::make_unique<ErrorMsgResult>(
                query->qname, table.name(),
                e.what()
        );*/
    }
}