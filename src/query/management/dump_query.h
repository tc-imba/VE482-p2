//
// Created by 59889 on 2017/11/12.
//

#ifndef LEMONDB_DUMP_QUERY_H
#define LEMONDB_DUMP_QUERY_H

#include "../query.h"
#include "../task.h"

class DumpTableQuery : public TaskQuery {
    static constexpr const char *qname = "DUMP";
    const std::string fileName;
public:
    LEMONDB_QUERY_WRITER(false)
    DumpTableQuery(std::string table, std::string filename)
    : TaskQuery(std::move(table)), fileName(std::move(filename)) {}
    QueryResult::Ptr execute() override;
    std::string toString() override;
    QueryResult::Ptr combine() override;
    friend class DumpTableTask;
};

class DumpTableTask : public Task {
protected:
    LEMONDB_QUERY_PTR(DumpTableQuery);
public:
    using Task::Task;
    void execute() override;
};

#endif //LEMONDB_DUMP_QUERY_H
