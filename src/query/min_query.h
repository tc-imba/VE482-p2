#ifndef LEMONDB_MIN_QUERY_H
#define LEMONDB_MIN_QUERY_H

#include "query.h"
#include "task.h"

class MinTask;

class MinQuery : public ComplexQuery {
    static constexpr const char *qname = "MIN";
	std::vector<Table::FieldIndex> fieldsId;
protected:
    LEMONDB_TASK_PTR_DEF(MinTask);
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
//	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	friend class MinTask;
/*	MinTask *getTask(int index) {
		return (MinTask*)tasks[index].get();
	}*/
};

class MinTask : public Task {
	std::vector<Table::ValueType> fieldsMin;
protected:
    LEMONDB_QUERY_PTR(MinQuery);
public:
    using Task::Task;
    void execute() override;
	friend class MinQuery;
//	std::vector<Table::ValueType> getFieldMin() { return fieldsMin; }
};

#endif //LEMONDB_MIN_QUERY_H
