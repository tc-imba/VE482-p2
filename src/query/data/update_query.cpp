#include "update_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

constexpr const char *UpdateQuery::qname;

QueryResult::Ptr UpdateQuery::execute() {
    start();
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        if (operands[0] == "KEY") {
            keyValue = operands[1];
        } else {
            fieldId = table.getFieldIndex(operands[0]);
            fieldValue = strtol(operands[1].c_str(), NULL, 10);
        }
        auto result = initConditionFast(table);
        if (!result.second) {
            complete(std::make_unique<RecordCountResult>(0));
            return make_unique<NullQueryResult>();
        }
        addIterationTask<UpdateTask>(db, table);
        return make_unique<RecordCountResult>(counter);
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

std::string UpdateQuery::toString() {
    return "QUERY = UPDATE " + this->targetTable + "\"";
}

QueryResult::Ptr UpdateQuery::combine(int taskComplete) {
    using namespace std;
    if (taskComplete < tasksSize) {
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


void UpdateTask::execute() {
    auto query = getQuery();
    try {
        for (auto it = begin; it != end; ++it) {
            if (query->evalConditionFast(*it)) {
                if (query->keyValue.empty()) {
                    (*it)[query->fieldId] = query->fieldValue;
                } else {
                    it->setKey(query->keyValue);
                }
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











