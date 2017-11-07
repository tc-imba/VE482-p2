//
// Created by 59889 on 2017/11/7.
//

#ifndef LEMONDB_DUPLICATE_QUERY_H
#define LEMONDB_DUPLICATE_QUERY_H

#ifndef LEMONDB_DUPLICATE_QUERY_H
#define LEMONDB_DUPLICATE_QUERY_H

#include "query.h"
#include "task.h"

class DuplicateQuery : public ComplexQuery {
    static constexpr const char *qname = "DUPLICATE";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
};

class DuplicateTask : public Task {
private:
    DuplicateQuery *getQuery() {
        return (DuplicateQuery *) query.get();
    }
public:
    DuplicateTask(const std::shared_ptr<ComplexQuery> &query,
               Table &table, Table::Iterator begin, Table::Iterator end) :
            Task(query, table, begin, end) {};
    void execute() override;
};

#endif //LEMONDB_DUPLICATE_QUERY_H
