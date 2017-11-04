//
// Created by admin on 2017/11/2.
//

#ifndef LEMONDB_DELETE_QUERY_H
#define LEMONDB_DELETE_QUERY_H

#include "query.h"

class DeleteQuery : public ComplexQuery {
    static constexpr const char *qname = "DELETE";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
};

class DeleteTask : public Task {
private:
    DeleteQuery *getQuery() {
        return (DeleteQuery *) query.get();
    }

public:
    void execute() override;
};

#endif //LEMONDB_DELETE_QUERY_H
