//
// Created by linzhi on 2017/11/7.
//

#include <algorithm>
#include "sum_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

LEMONDB_TASK_PTR_IMPL(SumQuery, SumTask);
constexpr const char *SumQuery::qname;

QueryResult::Ptr SumQuery::execute() {
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
                        "Cannot sum KEY!"
                );
            } else {
                fieldsId.emplace_back(table.getFieldIndex(operand));
            }
        }
/*        if (testKeyCondition(table, [&table, this](bool flag, Table::Object::Ptr &&object) {
            std::vector<Table::ValueType> fieldsSum(fieldsId.size(), 0);
            if (flag) {
                for (int i = 0; i < fieldsId.size(); ++i) {
                    fieldsSum[i] = (*object)[fieldsId[i]];
                }
            }
            complete(std::make_unique<AnswerResult>(std::move(fieldsSum)));
        })) {
            return make_unique<NullQueryResult>();
        }*/
        auto result = initCondition(table);
        if (!result.second) {
            std::vector<Table::ValueType> fieldsSum(fieldsId.size(), 0);
            complete(std::make_unique<AnswerResult>(std::move(fieldsSum)));
            return make_unique<NullQueryResult>();
        }
        addIterationTask<SumTask>(db, table);
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

std::string SumQuery::toString() {
    return "QUERY = SUM " + this->targetTable + "\"";
}

QueryResult::Ptr SumQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    auto it = tasks.begin();
    std::vector<Table::ValueType> fieldsSum(std::move(getTask(it)->fieldsSum));
    for (++it; it != tasks.end(); ++it) {
        auto numFields = fieldsId.size();
        for (int i = 0; i < numFields; ++i) {
            //fprintf(stderr, "%d ", this->getTask(it)->fieldsSum[i]);
            fieldsSum[i] += this->getTask(it)->fieldsSum[i];
        }
        //fprintf(stderr, "\n");
    }
    return make_unique<AnswerResult>(std::move(fieldsSum));
}

void SumTask::execute() {
    auto query = getQuery();
    try {
        auto numFields = query->fieldsId.size();
        fieldsSum.insert(fieldsSum.end(), numFields, 0);
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(*it)) {
                for (int i = 0; i < numFields; ++i) {
                    fieldsSum[i] += (*it)[query->fieldsId[i]];
                }
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

