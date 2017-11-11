#include "query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include <fstream>
#include <iostream>
#include <cassert>

void TaskQuery::complete() {
    /**
     * @TODO add the complete query to the result vector here
     * should add a unique id for each query - ok
     * should add a function to print results in correct order
     */
    ++taskComplete;
    auto result = combine();
    if (result != nullptr) {
        complete(std::move(result));
    }
}

void TaskQuery::complete(QueryResult::Ptr &&result) {
    if (result->success()) {
        auto &db = Database::getInstance();
        if (!targetTable.empty()) {
            auto &table = db[targetTable];
            table.refreshQuery();
        }
        db.addResult(this, std::move(result));
        db.completeQuery();
    } else {
        fprintf(stderr, "%s\n", result->toString().c_str());
    }
}

bool ComplexQuery::evalCondition(const std::vector<QueryCondition> &conditions,
                                 const Table::Object &object) {
    static const std::unordered_map<std::string, int> opmap{
            {">",  '>'},
            {"<",  '<'},
            {"=",  '='},
            {">=", 'g'},
            {"<=", 'l'},
    };
    bool ret = true;
    for (const auto &cond : conditions) {
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
            case '>' :
                ret = ret && (lhs > rhs);
                break;
            case '<' :
                ret = ret && (lhs < rhs);
                break;
            case '=' :
                ret = ret && (lhs == rhs);
                break;
            case 'g' :
                ret = ret && (lhs >= rhs);
                break;
            case 'l' :
                ret = ret && (lhs <= rhs);
                break;
            default:
                assert(0); // should never be here
            }
        }
    }
    return ret;
}

