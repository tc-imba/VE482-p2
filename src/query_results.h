#ifndef SRC_QUERY_RESULTS_H
#define SRC_QUERY_RESULTS_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sstream>
#include "formatter.h"

class QueryResult {
public:
    typedef std::unique_ptr<QueryResult> Ptr;

    virtual bool success()  = 0;

    virtual std::string toString() = 0;

    virtual ~QueryResult() = default;
};

class FailedQueryResult : public QueryResult {
    const std::string data;
public:
    bool success() override { return false; }
};

class SuceededQueryResult : public QueryResult {
public:
    bool success() override { return true; }
};

class NullQueryResult : public SuceededQueryResult {
public:
    std::string toString() override {
        return std::string();
    }
};

class ErrorMsgResult : public FailedQueryResult {
    std::string msg;
public:
    ErrorMsgResult(const char *qname,
                   const std::string &msg) {
        this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
    }

    ErrorMsgResult(const char *qname,
                   const char *table,
                   const std::string &msg) {
        this->msg =
                R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
    }

    std::string toString() override {
        return msg;
    }
};

class SuccessMsgResult : public SuceededQueryResult {
    std::string msg;
public:

    SuccessMsgResult(const int number) {
        this->msg = R"(Answer = "?".)"_f % number;
    }

    SuccessMsgResult(std::vector<int> results) {
        std::stringstream ss;
        ss << "Answer = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ")";
        this->msg = ss.str();
    }

    SuccessMsgResult(const char *qname) {
        this->msg = R"(Query "?" success.)"_f % qname;
    }

    SuccessMsgResult(const char *qname, const std::string &msg) {
        this->msg = R"(Query "?" success : ?)"_f % qname % msg;
    }

    SuccessMsgResult(const char *qname,
                     const char *table,
                     const std::string &msg) {
        this->msg = R"(Query "?" success in Table "?" : ?)"_f
                    % qname % table % msg;
    }

    std::string toString() override {
        return msg;
    }
};

class RecordCountResult : public SuceededQueryResult {
    const int affectedRows;
public:
    explicit RecordCountResult(int count) : affectedRows(count) {}

    std::string toString() override {
        return "Affected ? rows."_f % affectedRows;
    }
};

class AnswerResult : public SuceededQueryResult {
    std::string str;
public:
    explicit AnswerResult(std::vector<int> &&answer) {
        str = "ANSWER = ( ?)"_f % answer;
    }

    explicit AnswerResult(int answer) {
        str = "ANSWER = ?"_f % answer;
    }

    std::string toString() override {
        return str;
    }
};

class SelectResult : public SuceededQueryResult {
    std::vector<std::pair<std::string, std::vector<int> > > results;
public:
    explicit SelectResult(std::vector<std::pair<std::string, std::vector<int> > > &&results) : results(results) {}

    std::string toString() override {
        std::string str;
        for (const auto &pair :results) {
            str += "( ? ?)\n"_f % pair.first % pair.second;
        }
        return str;
    }
};

#endif //SRC_QUERY_RESULTS_H
