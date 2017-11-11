#ifndef LEMONDB_MAX_QUERY_H
#define LEMONDB_MAX_QUERY_H

#include "query.h"
#include "task.h"

class MaxTask;

class MaxQuery : public ComplexQuery {
    static constexpr const char *qname = "MAX";
	std::vector<Table::FieldIndex> fieldsId;
protected:
	LEMONDB_TASK_PTR_DEF(MaxTask);
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	friend class MaxTask;
};

class MaxTask : public Task {
	std::vector<Table::ValueType> fieldsMax;
protected:
	LEMONDB_QUERY_PTR(MaxQuery);
public:
    using Task::Task;
    void execute() override;
	friend class MaxQuery;
//	std::vector<int> getFieldMax() { return Max; }
};

#endif //LEMONDB_MAX_QUERY_H
