#include <iostream>

#include "db_table.h"
#include "query_parser.h"
#include "query_builders.h"
#include "query.h"
#include "db.h"

#include <vector>
#include <map>

using namespace std;

std::string extractQueryString() {
    std::string buf;
    do {
        int ch = cin.get();
        if (ch == ';') return buf;
        if (ch == EOF) throw std::ios_base::failure("End of input");
        buf.push_back(ch);
    } while(1);
}


int main() {
    QueryParser p;
    //p.registerQueryBuilder(std::make_unique<FakeQueryBuilder>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(UpdateTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    while (cin) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString();
            Query::Ptr query = p.parseQuery(queryStr);
            QueryResult::Ptr result = query->execute();
            cout << endl;
            if (result->success()) {
                cout << result->toString() << endl;
            } else {
                std::flush(cout);
                cerr << "QUERY FAILED:\n\t" << result->toString() << endl;
            }
        } catch (const std::ios_base::failure& e) {
            // End of input
            break;
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
        }
    }
    return 0;
}