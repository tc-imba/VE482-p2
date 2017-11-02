#include "db.h"
#include "data_query.h"
#include <vector>

constexpr const char *InsertQuery::qname;
constexpr const char *UpdateQuery::qname;
constexpr const char *DeleteQuery::qname;
constexpr const char *SelectQuery::qname;
constexpr const char *DuplicateQuery::qname;

QueryResult::Ptr UpdateQuery::execute() {
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
        auto index = table.getFieldIndex(operands[0]);
        int newValue = std::stoi(operands[1]);
        for (auto &object : table) {
            if (evalCondition(condition, object)) {
                object[index] = newValue;
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
        vector<Table::ValueType> data;
        for (auto it = ++this->operands.begin(); it != this->operands.end(); ++it) {
            data.push_back(strtol(it->c_str(), NULL, 10));
        }
        table.insertByIndex(this->operands.front(), data);

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
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        if (this->operands.empty() || this->operands.size() > 1 + table.field().size())
            return make_unique<ErrorMsgResult>(
                    qname, this->targetTable.c_str(),
                    "Invalid number of operands (? operands)."_f % operands.size()
            );
        /**
         * @TODO complete here
         */

    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    }
}

std::string SelectQuery::toString() {
    return "QUERY = SELECT " + this->targetTable + "\"";
}

QueryResult::Ptr DuplicateQuery::execute() {
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

std::string DuplicateQuery::toString() {
    return "QUERY = DUPLICATE " + this->targetTable + "\"";
}