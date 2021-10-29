#ifndef SRC_QUERY_RESULTS_H
#define SRC_QUERY_RESULTS_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "formatter.h"

class QueryResult {
public:
    typedef std::unique_ptr<QueryResult> Ptr;
    virtual bool        success()  = 0;
    virtual std::string toString() = 0;
    virtual ~QueryResult() = default;
};

class FailedQueryResult : public QueryResult {
    const std::string data;
public:
    bool success() override { return false; }
};

class SucceededQueryResult : public QueryResult {
public:
    bool success() override { return true; }
};

class NullQueryResult : public SucceededQueryResult {
public:
    std::string toString() override {
        return std::string();
    }
};

class ErrorMsgResult : public FailedQueryResult {
    std::string msg;
public:
    ErrorMsgResult(const char* qname,
                   const std::string& msg) {
        this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
    }

    ErrorMsgResult(const char* qname,
                   const char* table,
                   const std::string& msg) {
        this->msg =
                R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
    }

    std::string toString() override {
        return msg;
    }
};

class SuccessMsgResult : public SucceededQueryResult {
    std::string msg;
public:

    SuccessMsgResult(const char* qname) {
        this->msg = R"(Query "?" success.)"_f % qname;
    }

    SuccessMsgResult(const char* qname, const std::string& msg) {
        this->msg = R"(Query "?" success : ?)"_f % qname % msg;
    }

    SuccessMsgResult(const char* qname,
                     const char* table,
                     const std::string& msg) {
        this->msg = R"(Query "?" success in Table "?" : ?)"_f
                     % qname % table % msg;
    }

    std::string toString() override {
        return msg;
    }
};

class RecordCountResult : public SucceededQueryResult {
    const int affectedRows;
public:
    RecordCountResult(int count) : affectedRows(count) {}

    std::string toString() override {
        return "Affected ? rows."_f % affectedRows;
    }
};


#endif //SRC_QUERY_RESULTS_H
