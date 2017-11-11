#include "query_builders.h"
#include "management_query.h"
#include "query/load_table_query.h"
#include "query/update_query.h"
#include "query/delete_query.h"
#include "query/select_query.h"
#include "query/count_query.h"
#include "query/insert_query.h"
#include "query/min_query.h"
#include "query/max_query.h"
#include "query/sum_query.h"
#include <iostream>
#include <iomanip>

// Prints out debugging information.
// Does no real work
Query::Ptr FakeQueryBuilder::tryExtractQuery
        (TokenizedQueryString &query) {
    std::cout << "Query string: \n" << query.rawQeuryString << "\n";
    std::cout << "Tokens:\n";
    int count = 0;
    for (const auto &tok : query.token) {
        std::cout << std::setw(10) << "\"" << tok << "\"";
        count = (count + 1) % 5;
        if (count == 4) std::cout << std::endl;
    }
    if (count != 4) std::cout << std::endl;
    return this->nextBuilder->tryExtractQuery(query);
}

Query::Ptr ManageTableQueryBuilder::tryExtractQuery
        (TokenizedQueryString &query) {
    if (query.token.size() == 2) {
        if (query.token.front() == "LOAD")
            return std::make_unique<LoadTableQuery>(query.token[1]);
        if (query.token.front() == "DROP")
            return std::make_unique<DropTableQuery>(query.token[1]);
        if (query.token.front() == "TRUNCATE")
            return std::make_unique<TruncateTableQuery>(query.token[1]);
    }
    if (query.token.size() == 3) {
        if (query.token.front() == "DUMP")
            return std::make_unique<DumpTableQuery>(
                    query.token[1], query.token[2]
            );
        if (query.token.front() == "COPYTABLE")
            return std::make_unique<CopytableTableQuery>(query.token[1], query.token[2]);
    }
    return this->nextBuilder->tryExtractQuery(query);
}

Query::Ptr DebugQueryBuilder::tryExtractQuery(TokenizedQueryString &query) {
    if (query.token.size() == 1) {
        if (query.token.front() == "LIST")
            return std::make_unique<ListTableQuery>();
        if (query.token.front() == "QUIT")
            return std::make_unique<QuitQuery>();
    }
    if (query.token.size() == 2) {
        if (query.token.front() == "SHOWTABLE")
            return std::make_unique<PrintTableQuery>(query.token[1]);
    }
    return BasicQueryBuilder::tryExtractQuery(query);
}

void ComplexQueryBuilder::parseToken(TokenizedQueryString &query) {
    // Treats forms like:
    //
    // $OPER$ ( arg1 arg2 ... )
    // FROM table
    // WHERE ( KEY = $STR$ ) ( $field$ $OP$ $int$ ) ...
    //
    // The "WHERE" clause can be ommitted
    // The args of OPER clause can be ommitted

    auto it = query.token.cbegin();
    auto end = query.token.cend();
    it += 1; // Take to args;
    if (it == query.token.end())
        throw IllFormedQuery("Missing FROM clause");
    if (*it != "FROM") {
        if (*it != "(")
            throw IllFormedQuery("Ill-formed operand.");
        ++it;
        while (*it != ")") {
            this->operandToken.push_back(*it);
            ++it;
            if (it == end)
                throw IllFormedQuery("Ill-formed operand");
        }
        if (++it == end || *it != "FROM")
            throw IllFormedQuery("Missing FROM clause");
    }
    if (++it == end)
        throw IllFormedQuery("Missing targed table");
    this->targetTable = *it;
    if (++it == end) // the "WHERE" clause is ommitted
        return;
    if (*it != "WHERE")
        // Hmmm, C++11 style Raw-string literal
        // Reference:
        // http://en.cppreference.com/w/cpp/language/string_literal
        throw IllFormedQuery(
                R"(Expecting "WHERE", found "?".)"_f % *it
        );
    while (++it != end) {
        if (*it != "(")
            throw IllFormedQuery("Ill-formed query condition");
        QueryCondition cond;
        if (++it == end)
            throw IllFormedQuery("Missing field in condition");
        cond.field = *it;
        if (++it == end)
            throw IllFormedQuery("Missing operator in condition");
        cond.op = *it;
        if (++it == end)
            throw IllFormedQuery("Missing  in condition");
        cond.value = *it;
        if (++it == end || *it != ")")
            throw IllFormedQuery("Ill-formed query condition");
        this->conditionToken.push_back(cond);
    }
}

Query::Ptr ComplexQueryBuilder::tryExtractQuery(TokenizedQueryString &query) {
    try {
        this->parseToken(query);
    } catch (const IllFormedQuery &e) {
        std::cout << e.what() << std::endl;
        return this->nextBuilder->tryExtractQuery(query);
    }
    std::string operation = query.token.front();
    if (operation == "INSERT")
        return std::make_unique<InsertQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "UPDATE")
        return std::make_unique<UpdateQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "SELECT")
        return std::make_unique<SelectQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "DELETE")
        return std::make_unique<DeleteQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "SUM")
        return std::make_unique<SumQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "MIN")
        return std::make_unique<MinQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    if (operation == "MAX")
        return std::make_unique<MaxQuery>(
                this->targetTable, this->operandToken, this->conditionToken);
    std::cout << "Complicated query found!" << std::endl;
    std::cout << "Operation = " << query.token.front() << std::endl;
    std::cout << "    Operands : ";
    for (const auto &oprand : this->operandToken)
        std::cout << oprand << " ";
    std::cout << std::endl;
    std::cout << "Target Table = " << this->targetTable << std::endl;
    if (this->conditionToken.empty())
        std::cout << "No WHERE clause specified." << std::endl;
    else
        std::cout << "Conditions = ";
    for (const auto &cond : this->conditionToken)
        std::cout << cond.field << cond.op << cond.value << " ";
    std::cout << std::endl;

    return this->nextBuilder->tryExtractQuery(query);
}

void ComplexQueryBuilder::clear() {
    this->conditionToken.clear();
    this->targetTable = "";
    this->operandToken.clear();
    this->nextBuilder->clear();
}

// Query::Ptr UpdateTableQueryBuilder::tryExtractQuery(TokenizedQueryString &query) {
//     if (query.token.front() != "UPDATE")
//         return this->nextBuilder->tryExtractQuery(query);
//     this->parseToken(query); // Expects throw on failure.
//     return std::make_unique<UpdateQuery> (
//             this->targetTable, this->operandToken, this->conditionToken
//     );
// }
