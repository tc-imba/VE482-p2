#include "query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include <fstream>
#include <iostream>
#include <cassert>

void TaskQuery::start() {
    //fprintf(stderr, "Start %d: %s %s\n", id, toString().c_str(), targetTable.empty() ? "( no target )" : "");
}

void TaskQuery::complete() {
    /**
     * @TODO add the complete query to the result vector here
     * should add a unique id for each query - ok
     * should add a function to print results in correct order
     */
    auto result = combine(taskComplete.fetch_add(1, std::memory_order_relaxed));
    if (result != nullptr) {
        complete(std::move(result));
    }
}

void TaskQuery::complete(QueryResult::Ptr &&result) {
    if (result->success()) {
        auto &db = Database::getInstance();
        //fprintf(stderr, "End %d: %s\n", id, toString().c_str());
        if (!targetTable.empty()) {
            auto &table = db[targetTable];
            table.completeQuery();
        }
        db.addResult(this, std::move(result));
        db.completeQuery();
    } else {
        //fprintf(stderr, "%s\n", result->toString().c_str());
    }
}

static const std::unordered_map<std::string, int> opmap{
        {">",  '>'},
        {"<",  '<'},
        {"=",  '='},
        {">=", 'g'},
        {"<=", 'l'},
};

bool ComplexQuery::evalCondition(const std::vector<QueryCondition> &conditions,
                                 const Table::Object &object) {
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

std::pair<std::string, bool>
ComplexQuery::initConditionFast(const Table &table) {
    std::pair<std::string, bool> result = {"", true};
    for (auto &cond : condition) {
        if (cond.field == "KEY") {
            if (cond.op != "=") {
                throw IllFormedQueryCondition(
                        "Can only compare equivalence on KEY"
                );
            } else if (result.first.empty()) {
                result.first = cond.value;
            } else if (result.first != cond.value) {
                result.second = false;
                return result;
            }
        } else {
            cond.fieldId = table.getFieldIndex(cond.field);
            cond.valueParsed = std::strtol(cond.value.c_str(), NULL, 10);
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
                cond.comp = std::greater<>();
                break;
            case '<' :
                cond.comp = std::less<>();
                break;
            case '=' :
                cond.comp = std::equal_to<>();
                break;
            case 'g' :
                cond.comp = std::greater_equal<>();
                break;
            case 'l' :
                cond.comp = std::less_equal<>();
                break;
            default:
                assert(0); // should never be here
            }
        }
    }
    return result;
}

bool ComplexQuery::evalConditionFast(const Table::Object &object) {
    bool ret = true;
    for (const auto &cond : condition) {
        if (cond.field != "KEY") {
            ret = ret && cond.comp(object[cond.fieldId], cond.valueParsed);
        } else {
            ret = ret && (object.key() == cond.value);
        }
    }
    return ret;
}

bool ComplexQuery::testKeyCondition(Table &table, std::function<void(bool, Table::Object::Ptr &&)> function) {
    auto condResult = initConditionFast(table);
    if (!condResult.second) {
        function(false, nullptr);
        return true;
    }
    if (!condResult.first.empty()) {
        auto object = table[condResult.first];
        if (object != nullptr && evalConditionFast(*object)) {
            function(true, std::move(object));
        } else {
            function(false, nullptr);
        }
        return true;
    }
    return false;
}