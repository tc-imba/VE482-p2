#ifndef LEMONDB_MIN_QUERY_H
#define LEMONDB_MIN_QUERY_H

#include "query.h"
#include "task.h"

class MinQuery : public ComplexQuery {
    static constexpr const char *qname = "MIN";
public:
    using ComplexQuery::ComplexQuery;
	std::vector<std::string> &getOperands() { return operands; }
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
	MinTask *getTask(int index) {
		return (MinTask*)tasks[index].get();
	}
};

class MinTask : public Task {
private:
    MinQuery *getQuery() {
        return (MinQuery *) query.get();
    }
	std::vector<int> Min;
public:
    MinTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
	std::vector<int> getFieldMin() { return Min; }
};

#endif //LEMONDB_MIN_QUERY_H
