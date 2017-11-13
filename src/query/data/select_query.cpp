//
// Created by linzhi on 11/6/17.
//

#include <algorithm>
#include "select_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

LEMONDB_TASK_PTR_IMPL(SelectQuery, SelectTask);
constexpr const char *SelectQuery::qname;

QueryResult::Ptr SelectQuery::execute() {
    start();
    using namespace std;
    if (this->operands.empty() || this->operands[0] != "KEY")
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands) or without leading KEY."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        for (auto it = ++operands.begin(); it != operands.end(); ++it) {
            fieldsId.emplace_back(table.getFieldIndex(*it));
        }
        auto result = initConditionFast(table);
        if (!result.second) {
            complete(std::make_unique<NullQueryResult>());
            return make_unique<NullQueryResult>();
        }
        addIterationTask<SelectTask>(db, table);
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

QueryResult::Ptr SelectQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    auto it = tasks.begin();
    std::vector<std::pair<std::string, std::vector<Table::ValueType> > > results(std::move(getTask(it)->results));
    for (++it; it != tasks.end(); ++it) {
        auto taskResults = getTask(it)->results;
        auto mid = results.insert(results.end(), taskResults.begin(), taskResults.end());
        std::inplace_merge(results.begin(), mid, results.end(),
                           [](const std::pair<std::string, std::vector<Table::ValueType> > &a,
                              const std::pair<std::string, std::vector<Table::ValueType> > &b) {
                               return a.first < b.first;
                           });
    }
    if (results.empty()) {
        return make_unique<NullQueryResult>();
    }
    return make_unique<SelectResult>(std::move(results));
}

void SelectTask::execute() {
    auto query = getQuery();
    try {
        for (auto it = begin; it != end; ++it) {
            if (query->evalConditionFast(*it)) {
                std::vector<Table::ValueType> tuple;
                for (auto &index:query->fieldsId) {
                    tuple.emplace_back((*it)[index]);
                }
                results.emplace_back(it->key(), std::move(tuple));
            }
        }
        std::sort(results.begin(), results.end(),
                  [](const std::pair<std::string, std::vector<Table::ValueType> > &a,
                     const std::pair<std::string, std::vector<Table::ValueType> > &b) {
                      return a.first < b.first;
                  });
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