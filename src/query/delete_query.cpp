//
// Created by admin on 2017/11/2.
//

#include "delete_query.h"
#include "../db.h"
#include "../db_table.h"

constexpr const char *DeleteQuery::qname;

QueryResult::Ptr DeleteQuery::execute() {
    using namespace std;
    if (!this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        if (condition.empty()) {
            counter = table.clear();
            return make_unique<RecordCountResult>(counter);
        }
        for (auto it = table.begin(); it != table.end(); ++it) {
            if (evalCondition(condition, *it)) {
                table.erase(it);
                counter++;
            } else {
                table.move(it);
            }
        }
        table.swapData();
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

std::string DeleteQuery::toString() {
    return "QUERY = DELETE " + this->targetTable + "\"";
}
