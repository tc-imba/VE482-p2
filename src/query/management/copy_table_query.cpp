//
// Created by liu on 17-11-12.
//

#include "copy_table_query.h"

constexpr const char *CopyTableQuery::qname;
constexpr const char *CopyTableDestQuery::qname;

std::string CopyTableQuery::toString() {
    return "QUERY = Copy TABLE, "
                   "Table = \"" + targetTable + "\", "
                   "To Table = \"" + destTableName + "\"";
}

QueryResult::Ptr CopyTableQuery::execute() {
    start();
    using namespace std;
    if (!destQuery->ready) {
        return make_unique<ErrorMsgResult>(
                qname, targetTable.c_str(),
                "Not ready"s
        );
    }

    auto &db = Database::getInstance();
    try {
        auto &table = db.ensureTable(destTableName);
        /*if (table.isInited()) {
            return make_unique<ErrorMsgResult>(qname, destTableName.c_str(),
                    "Already exists."s
            );
        }*/
        table.copy(db[targetTable]);
        complete(make_unique<SuccessMsgResult>(qname, targetTable.c_str()));
        destQuery->complete(make_unique<SuccessMsgResult>(qname, targetTable.c_str()));
        return make_unique<NullQueryResult>();
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, targetTable.c_str(),
                "No such table."s
        );
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

Query::Ptr CopyTableQuery::createDestQuery() {
    auto ptr = std::make_unique<CopyTableDestQuery>(destTableName, this);
    destQuery = ptr.get();
    return ptr;
}

std::string CopyTableDestQuery::toString() {
    return "QUERY = Copy to TABLE, "
                   "Table = \"" + targetTable + "\"";
}

QueryResult::Ptr CopyTableDestQuery::execute() {
    start();
    using namespace std;
    srcQuery->execute();
    ready = true;
    return make_unique<NullQueryResult>();
}