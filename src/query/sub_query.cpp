#include "sub_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"

LEMONDB_TASK_PTR_IMPL(SubQuery, SubTask);
constexpr const char *SubQuery::qname;

QueryResult::Ptr SubQuery::execute() {
    start();
	using namespace std;
    if (this->operands.size() < 2)
       return make_unique<ErrorMsgResult> (
             qname, this->targetTable.c_str(),
             "Invalid number of operands (? operands)."_f % operands.size()
       );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        for (const auto &operand : this->operands) {
            if (operand == "KEY") {
                return make_unique<ErrorMsgResult>(
                        qname, this->targetTable.c_str(),
                        "Cannot subtract KEY!"
                );
            } else {
                fieldsId.emplace_back(table.getFieldIndex(operand));
            }
        }
        addIterationTask<SubTask>(db, table);
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

std::string SubQuery::toString() {
    return "QUERY = SUB" + this->targetTable + "\"";
}

QueryResult::Ptr SubQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
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


void SubTask::execute() {
    auto query = getQuery();
    try {
        auto numFields = query->fieldsId.size() - 1;
        Table::ValueType result;
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                result = (*it)[query->fieldsId[0]];
                for (int i = 1; i < numFields; ++i) {
                    result -= (*it)[query->fieldsId[i]];
                }
                (*it)[query->fieldsId[numFields]] = result;
                counter++;
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