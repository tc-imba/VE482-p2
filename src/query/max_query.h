#ifndef LEMONDB_MAX_QUERY_H
#define LEMONDB_MAX_QUERY_H

#include "query.h"
#include "task.h"

class MaxQuery : public ComplexQuery {
    static constexpr const char *qname = "MAX";
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
/*	MaxTask *getTask(int index) {
		return (MaxTask*)tasks[index].get();
	}*/
};

class MaxTask : public Task {
	std::vector<int> Max;
protected:
	LEMONDB_QUERY_PTR(MaxQuery);
public:
    using Task::Task;
    void execute() override;
	std::vector<int> getFieldMax() { return Max; }
};

#endif //LEMONDB_MAX_QUERY_H
