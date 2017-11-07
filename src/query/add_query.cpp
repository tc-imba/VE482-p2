#include "add_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"

constexpr const char *AddQuery::qname;

QueryResult::Ptr AddQuery::execute() {
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
        addIterationTask<AddTask>(db, table);
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

std::string AddQuery::toString() {
    return "QUERY = ADD" + this->targetTable + "\"";
}

QueryResult::Ptr AddQuery::combine() {
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


void AddTask::execute() {
    try {
        int numFields = this->getQuery()->getOperands().size();
        int sum;
        for (auto it = begin; it != end; ++it) {
            if (query->evalCondition(query->getCondition(), *it)) {
                sum=0;
                for (int i = 0; i < numFields - 1; ++i) {
                    sum = sum + (*it)[this->getQuery()->getOperands()[i]];
                }
                (*it)[this->getQuery()->getOperands()[numFields-1]] = sum;
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