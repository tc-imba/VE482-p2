#ifndef LEMONDB_INSERT_QUERY_H
#define LEMONDB_INSERT_QUERY_H

#include "query.h"
#include "task.h"
#include <sstream>

class InsertQuery : public ComplexQuery {
    static constexpr const char *qname = "INSERT";
public:
    LEMONDB_QUERY_WRITER(true);
    using ComplexQuery::ComplexQuery;
	//Table::KeyType key = operands[0];
	//std::vector<Table::ValueType> data;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	//Table::KeyType getkey() { return key; }
	//std::vector<Table::ValueType> getdata() { return data; }
	friend class InsertTask;
};

class InsertTask : public Task {
protected:
    LEMONDB_QUERY_PTR(InsertQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_INSERT_QUERY_H
