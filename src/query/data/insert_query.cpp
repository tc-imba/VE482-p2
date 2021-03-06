#include <algorithm>
#include <iostream>
#include "insert_query.h"
#include "../../db/db.h"
#include "../../db/db_table.h"
#include "../../formatter.h"

constexpr const char *InsertQuery::qname;

QueryResult::Ptr InsertQuery::execute() {
    start();
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        addUniqueTask<InsertTask>(db, &table);
        return make_unique<RecordCountResult>(counter);
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

std::string InsertQuery::toString() {
    return "QUERY = INSERT " + this->targetTable + "\"";
}

QueryResult::Ptr InsertQuery::combine(int taskComplete) {
    //fprintf(stderr, "Complete %d\n", getId());
    return std::make_unique<SuccessMsgResult>(qname);
}

void InsertTask::execute() {
    auto query = getQuery();
    try {
        auto &operands = query->getOperands();
        auto &key = operands.front();
        std::vector<Table::ValueType> data;
        std::for_each(++operands.begin(), operands.end(), [&data](const std::string &item) {
            data.emplace_back(strtol(item.c_str(), NULL, 10));
        });
        table->insertByIndex(key, data);
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

