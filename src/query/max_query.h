#ifndef LEMONDB_MAX_QUERY_H
#define LEMONDB_MAX_QUERY_H

#include "query.h"
#include "task.h"

class MaxQuery : public ComplexQuery {
    static constexpr const char *qname = "MAX";
public:
    using ComplexQuery::ComplexQuery;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	MaxTask *getTask(int index) {
		return (MaxTask*)tasks[index].get();
	}
};

class MaxTask : public Task {
private:
    MaxQuery *getQuery() {
        return (MaxQuery *) query.get();
    }
	std::vector<int> Max;
public:
    MaxTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
	std::vector<int> getFieldMax() { return Max; }
};

#endif //LEMONDB_MAX_QUERY_H
