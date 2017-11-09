#ifndef LEMONDB_MIN_QUERY_H
#define LEMONDB_MIN_QUERY_H

#include "query.h"
#include "task.h"

class MinQuery : public ComplexQuery {
    static constexpr const char *qname = "MIN";
public:
    LEMONDB_QUERY_WRITER(false);
    using ComplexQuery::ComplexQuery;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
/*	MinTask *getTask(int index) {
		return (MinTask*)tasks[index].get();
	}*/
};

class MinTask : public Task {
	std::vector<int> Min;
protected:
    LEMONDB_QUERY_PTR(MinQuery);
public:
    using Task::Task;
    void execute() override;
	std::vector<int> getFieldMin() { return Min; }
};

#endif //LEMONDB_MIN_QUERY_H
