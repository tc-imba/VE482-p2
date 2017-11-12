#include "max_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

LEMONDB_TASK_PTR_IMPL(MaxQuery, MaxTask);
constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
    start();
    using namespace std;
    if (this->operands.empty())
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
                        "Cannot compare KEY!"
                );
            } else {
                fieldsId.emplace_back(table.getFieldIndex(operand));
            }
        }
/*        if (testKeyCondition(table, [&table, this](bool flag, Table::Object::Ptr &&object) {
            if (flag) {
                std::vector<Table::ValueType> fieldsMax(fieldsId.size());
                for (int i = 0; i < fieldsId.size(); ++i) {
                    fieldsMax[i] = (*object)[fieldsId[i]];
                }
                complete(std::make_unique<AnswerResult>(std::move(fieldsMax)));
            } else {
                complete(std::make_unique<NullQueryResult>());
            }
        })) {
            return make_unique<NullQueryResult>();
        }*/
        auto result = initConditionFast(table);
        if (!result.second) {
            complete(std::make_unique<NullQueryResult>());
            return make_unique<NullQueryResult>();
        }
        addIterationTask<MaxTask>(db, table);
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e) {
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

std::string MaxQuery::toString() {
    return "QUERY = MAX " + this->targetTable + "\"";
}

QueryResult::Ptr MaxQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize - 1) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    auto it = tasks.begin();
    std::vector<Table::ValueType> fieldsMax(std::move(getTask(it)->fieldsMax));
    Table::SizeType counter = (*it)->getCounter();
    for (++it; it != tasks.end(); ++it) {
        auto numFields = fieldsId.size();
        for (int i = 0; i < numFields; ++i) {
            fieldsMax[i] = std::max(fieldsMax[i], this->getTask(it)->fieldsMax[i]);
        }
        counter += (*it)->getCounter();
    }
    if (counter == 0) {
        return make_unique<NullQueryResult>();
    }
    return make_unique<AnswerResult>(std::move(fieldsMax));
}

void MaxTask::execute() {
    auto query = getQuery();
    try {
        auto numFields = query->fieldsId.size();
        fieldsMax.insert(fieldsMax.end(), numFields, Table::ValueTypeMin);
        for (auto it = begin; it != end; ++it) {
            if (query->evalConditionFast(*it)) {
                for (int i = 0; i < numFields; ++i) {
                    fieldsMax[i] = std::max(fieldsMax[i], (*it)[query->fieldsId[i]]);
                }
                counter = 1;
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

