#include "db.h"
#include "data_query.h"
#include <vector>

constexpr const char *InsertQuery::qname;
constexpr const char *UpdateQuery::qname;
constexpr const char *DeleteQuery::qname;
constexpr const char *SelectQuery::qname;

QueryResult::Ptr UpdateQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    string field = operands[0];
    int counter = 0;
    try {
        int newValue = std::stoi(operands[1]);
        auto &table = db[this->targetTable];
        for (auto object : table) {
            if (evalCondition(condition, object)) {
                object[field] = newValue;
                counter++;
            }
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const TableFieldNotFound &e) {
        // Field not found
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                e.what()
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


QueryResult::Ptr DeleteQuery::execute() {
    using namespace std;
    if (this->operands.size() != 0)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    int counter = 0;
    try {
        auto &table = db[this->targetTable];
        for (auto it = table.begin(); it != table.end();) {
            if (evalCondition(condition, *it)) {
                it = table.erase(it);
                counter++;
            } else ++it;
        }
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

QueryResult::Ptr InsertQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        if (this->operands.size() != 1 + table.field().size())
            return make_unique<ErrorMsgResult>(
                    qname, this->targetTable.c_str(),
                    "Invalid number of operands (? operands)."_f % operands.size()
            );
        table.insertByIndex(this->operands.front(),
                            vector<Table::ValueType>(++this->operands.begin(), this->operands.end()));

        return make_unique<NullQueryResult>();
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const ConflictingKey &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Key conflict."s
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

QueryResult::Ptr SelectQuery::execute() {

}

std::string SelectQuery::toString() {
    return "QUERY = SELECT " + this->targetTable + "\"";
}
