#include "query.h"
#include "../db.h"
#include "../db_table.h"
#include <fstream>
#include <iostream>
#include <cassert>

bool ConditionedQuery::evalCondition(const std::vector<QueryCondition> &conditions,
                                     const Table::Object &object) {
    static const std::unordered_map<std::string, int> opmap {
            {">", '>'}, {"<", '<'}, {"=", '='},
            {">=", 'g'}, {"<=", 'l'},
    };
    bool ret = true;
    for (const auto& cond : conditions) {
        if (cond.field == "KEY") {
            if (cond.op != "=")
                throw IllFormedQueryCondition(
                        "Can only compare equivalence on KEY"
                );
            ret = ret && (object.key() == cond.value);
        } else {
            int rhs = std::stoi(cond.value);
            int lhs = object[cond.field];
            int op = 0;
            try {
                op = opmap.at(cond.op);
            } catch (const std::out_of_range &e) {
                throw IllFormedQueryCondition(
                        R"("?" is not a valid condition operator.)"_f % cond.op
                );
            }
            switch (op) {
                case '>' : ret = ret && (lhs > rhs); break;
                case '<' : ret = ret && (lhs < rhs); break;
                case '=' : ret = ret && (lhs == rhs); break;
                case 'g' : ret = ret && (lhs >= rhs); break;
                case 'l' : ret = ret && (lhs <= rhs); break;
                default:
                    assert(0); // should never be here
            }
        }
    }
    return ret;
}

