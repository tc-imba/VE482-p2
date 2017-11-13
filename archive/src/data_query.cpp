#include "db.h"
#include "data_query.h"


constexpr const char* InsertQuery::qname;
constexpr const char* UpdateQuery::qname;
constexpr const char* DeleteQuery::qname;
constexpr const char* SelectQuery::qname;

QueryResult::Ptr UpdateQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database& db = Database::getInstance();
    string field = operands[0];
    int counter = 0;
    try {
        int newValue = std::stoi(operands[1]);
        auto& table = db[this->targetTable];
        for (auto object : table) {
            if (evalCondition(condition, object)) {
                object[field] = newValue;
                counter++;
            }
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound& e) {
        return make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const TableFieldNotFound& e) {
        // Field not found
        return make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                e.what()
        );
    }  catch (const IllFormedQueryCondition& e) {
        return std::make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                e.what()
        );
    } catch (const invalid_argument& e) {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                "Unknown error '?'"_f % e.what()
        );
    } catch (const exception& e) {
        return std::make_unique<ErrorMsgResult> (
                qname, this->targetTable.c_str(),
                "Unkonwn error '?'."_f % e.what()
        );
    }
}

std::string UpdateQuery::toString() {
    return "QUERY = UPDATE " + this->targetTable + "\"";
}


