#include "max_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"
#include <climits>

LEMONDB_TASK_PTR_IMPL(MaxQuery, MaxTask);
constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
//    Table::SizeType counter = 0;
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

QueryResult::Ptr MaxQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
    auto it = tasks.begin();
    std::vector<Table::ValueType> fieldsMax(std::move(getTask(it)->fieldsMax));
    for (++it; it != tasks.end(); ++it) {
        auto numFields = fieldsId.size();
        for (int i = 0; i < numFields; ++i) {
            Table::ValueType tmp = this->getTask(it)->fieldsMax[i];
            if (tmp > fieldsMax[i]) {
                fieldsMax[i] = tmp;
            }
//            fieldsMin[i] = tmp < fieldsMin[i] ? tmp : fieldsMin[i];
        }
    }
    return make_unique<AnswerResult>(std::move(fieldsMax));
}

void MaxTask::execute() {
	auto query = getQuery();
    try {
        auto numFields = query->fieldsId.size();
        fieldsMax.insert(fieldsMax.end(), numFields, INT_MIN);
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                for (int i = 0; i < numFields; ++i) {
                    Table::ValueType tmp = (*it)[query->fieldsId[i]];
                    if (tmp > fieldsMax[i]) fieldsMax[i] = tmp;
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

