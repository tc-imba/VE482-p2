#include "add_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

LEMONDB_TASK_PTR_IMPL(AddQuery, AddTask);
constexpr const char *AddQuery::qname;

QueryResult::Ptr AddQuery::execute() {
    start();
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        for (const auto &operand : this->operands) {
            if (operand == "KEY") {
                return make_unique<ErrorMsgResult>(
                        qname, this->targetTable.c_str(),
                        "Cannot add KEY!"
                );
            } else {
                fieldsId.emplace_back(table.getFieldIndex(operand));
            }
        }
        /*if (testKeyCondition(table, [&table, this](bool flag, Table::Object::Ptr &&object) {
            if (flag) {
                auto itId = fieldsId.begin();
                auto result = (*object)[*itId];
                for (++itId; itId != fieldsId.end() - 1; ++itId) {
                    result += (*object)[*itId];
                }
                auto destId = fieldsId.back();
                (*object)[destId] = result;
                table.eraseUnique(std::move(object));
                complete(std::make_unique<RecordCountResult>(1));
            } else {
                complete(std::make_unique<RecordCountResult>(0));
            }
        })) {
            return make_unique<NullQueryResult>();
        }*/
        auto result = initConditionFast(table);
        if (!result.second) {
            complete(std::make_unique<RecordCountResult>(0));
            return make_unique<NullQueryResult>();
        }
        addIterationTask<AddTask>(db, table);
        return make_unique<SuccessMsgResult>(qname);
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

std::string AddQuery::toString() {
    return "QUERY = ADD" + this->targetTable + "\"";
}

QueryResult::Ptr AddQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize - 1) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    Database &db = Database::getInstance();
    auto &table = db[this->targetTable];
    Table::SizeType counter = 0;
    for (auto &task:tasks) {
        counter += task->getCounter();
    }
    return make_unique<RecordCountResult>(counter);
}


void AddTask::execute() {
    auto query = getQuery();
    try {
        auto destId = query->fieldsId.back();
        for (auto it = begin; it != end; ++it) {
            if (query->evalConditionFast(*it)) {
                auto itId = query->fieldsId.begin();
                auto result = (*it)[*itId];
                for (++itId; itId != query->fieldsId.end() - 1; ++itId) {
                    result += (*it)[*itId];
                }
                (*it)[destId] = result;
                ++counter;
            }
        }
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