//
// Created by linzhi on 2017/11/7.
//

#include "sum_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"

constexpr const char *SumQuery::qname;

QueryResult::Ptr SumQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    for (auto operand : this->operands) {
        if (operand == "KEY") return make_unique<ErrorMsgResult>(
                    qname, this->targetTable.c_str(),
                    "Cannot sum KEY!"
            );
    }
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
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

QueryResult::Ptr SumQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    size_t counter = 0;
    /*std::vector<int> totalFieldSums(std::move(this->getTask(0)->getFieldSums()));
    for (int i = 1; i < tasks.size(); ++i) {
        SumTask *thisTask = this->getTask(i);
        for (int j = 0; j < operands.size(); ++j) {
            totalFieldSums[j] += thisTask->getFieldSums()[j];
        }
    }*/
    return make_unique<RecordCountResult>(counter);
}

void SumTask::execute() {
    auto query = getQuery();
    try {
        int numFields = this->getQuery()->getOperands().size();
        fieldSums.insert(fieldSums.end(), numFields, 0);
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                for (int i = 0; i < numFields; ++i) {
                    fieldSums[i] += (*it)[this->getQuery()->getOperands()[i]];
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

