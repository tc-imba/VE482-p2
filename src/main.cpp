#include <iostream>
#include <fstream>

#include "db/db_table.h"
#include "query_parser.h"
#include "query_builders.h"
#include "query/query.h"
#include "db/db.h"

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
    } while (1);
}


int main(int argc, char *argv[]) {
#ifdef DEBUG
    std::ifstream fin;
    if (argc > 1) {
        fin.open(argv[1]);
        if (fin.is_open()) {
            cin.rdbuf(fin.rdbuf());
        }
    }
#endif
    Database &db = Database::getInstance();
    QueryParser p;
    //p.registerQueryBuilder(std::make_unique<FakeQueryBuilder>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());
    while (cin && !db.isEnd()) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString();
            Query::Ptr query = p.parseQuery(queryStr);
            db.addQuery(std::move(query));
            //QueryResult::Ptr result = query->execute();
            /*cout << endl;
            if (result->success()) {
                cout << result->toString() << endl;
            } else {
                std::flush(cout);
                cerr << "QUERY FAILED:\n\t" << result->toString() << endl;
            }*/
        } catch (const std::ios_base::failure &e) {
            // End of input
            db.endQuery();
            break;
        } catch (const std::exception &e) {
            cerr << e.what() << endl;
            db.endQuery();
            break;
        } catch (...) {
            db.endQuery();
            break;
        }
    }
    db.joinThreads();
    /*while (db.isBusy()) {
        std::this_thread::yield();
    }*/
    return 0;
}