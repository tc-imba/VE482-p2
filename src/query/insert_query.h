//
// Created by admin on 2017/11/2.
//

#ifndef LEMONDB_DELETE_QUERY_H
#define LEMONDB_DELETE_QUERY_H

#include "query.h"
#include "task.h"
#include <sstream>

class InsertQuery : public ComplexQuery {
    static constexpr const char *qname = "INSERT";
public:
    using ComplexQuery::ComplexQuery;
	//Table::KeyType key = operands[0];
	//std::vector<Table::ValueType> data;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	//Table::KeyType getkey() { return key; }
	//std::vector<Table::ValueType> getdata() { return data; }
};

class InsertTask : public Task {
private:
    InsertQuery *getQuery() {
        return (InsertQuery *) query.get();
    }
public:
    InsertTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};

#endif //LEMONDB_DELETE_QUERY_H
