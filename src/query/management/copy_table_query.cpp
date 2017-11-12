//
// Created by liu on 17-11-12.
//

#include "copy_table_query.h"

constexpr const char *CopyTableQuery::qname;

std::string CopyTableQuery::toString() {
    return "QUERY = Copy TABLE, "
                   "Table = \"" + targetTable + "\", "
                   "To Table = \"" + newTableName + "\"";
}

QueryResult::Ptr CopyTableQuery::execute() {
    start();
    using namespace std;
    auto &db = Database::getInstance();
    try {
        auto &table = db.ensureTable(newTableName);
        if (table.isInited()) {
            return make_unique<ErrorMsgResult>(qname, newTableName.c_str(),
                    "Already exists."s
            );
        }
        table.copy(db[targetTable]);
        complete(make_unique<SuccessMsgResult>(qname, targetTable.c_str()));
        return make_unique<SuccessMsgResult>(qname, targetTable.c_str());
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, targetTable.c_str(),
                "No such table."s
        );
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}
