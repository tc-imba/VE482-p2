//
// Created by linzhi on 2017/11/7.
//

#include "swap_query.h"

#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

constexpr const char *SwapQuery::qname;

QueryResult::Ptr SwapQuery::execute() {
    start();
    using namespace std;
    if (this->operands.size() != 2 || this->operands[0] == "KEY" || this->operands[1] == "KEY")
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        this->operand1 = table.getFieldIndex(this->operands[0]);
        this->operand2 = table.getFieldIndex(this->operands[1]);
        /*if (condition.empty()) {
            auto counter = table.size();
            table.swapField(this->operands[0], this->operands[1]);
            complete(std::make_unique<RecordCountResult>(counter));
            return make_unique<SuccessMsgResult>(qname);
        }*/
        auto result = initConditionFast(table);
        if (!result.second) {
            complete(std::make_unique<RecordCountResult>(0));
            return make_unique<NullQueryResult>();
        }
        addIterationTask<SwapTask>(db, table);
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

std::string SwapQuery::toString() {
    return "QUERY = SWAP " + this->targetTable + "\"";
}

QueryResult::Ptr SwapQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize - 1) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    Table::SizeType counter = 0;
    for (auto &task:tasks) {
        counter += task->getCounter();
    }
    return make_unique<RecordCountResult>(counter);
}

#include <iostream>

void SwapTask::execute() {
    auto query = getQuery();
    try {
        for (auto it = begin; it != end; ++it) {
            if (query->evalConditionFast(*it)) {
                //std::cerr << (*it)[query->operand1] << " " << (*it)[query->operand2] << std::endl;
                std::swap((*it)[query->operand1], (*it)[query->operand2]);
                /*Table::ValueType tmp((*it)[query->operand1]);
                (*it)[query->operand1] = (*it)[query->operand2];
                (*it)[query->operand2] = tmp;*/
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

