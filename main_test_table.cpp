#include <iostream>

#include "db_table.h"

#include <vector>
#include <map>

using namespace std;

int main() {
    Table tab("Test", vector<Table::FieldID>{"A", "B", "C", "D"});
    std::cout << tab << std::endl;
    std::cout << (tab.begin() == tab.end()) << endl;
    tab.insert("alpha",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    tab.insert("beta",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    tab.insert("gamma",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"C", 2000},{"B", 20},
    });
    tab.insert("delta",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    try {
        tab.insert("delta", map<Table::FieldID, Table::ValueType> {
                {"A", 10},
                {"D", 2000},
                {"B", 20},
        });
    } catch (const ConflictingKey& e) {
        std::cout << e.what() << endl;
    }
    const auto tab2 = tab;
    std::cout << tab << std::endl;
    std::cout << (tab.begin() == tab.end()) << endl;
    auto it = tab.begin();
    cout << it->key() << " " <<  it->get("A") << endl;
    ++it;
    cout << it->key() << " " <<  it->get("A") << endl;
    for (auto Object : tab) {
        cout << Object.key() << " " << Object["D"] << endl;
        Object["A"] += Object["B"] + Object["D"];
    }
    std::cout << tab << std::endl;
    it = tab.begin();
    while (!tab.empty()) {
        it = tab.erase(it);
        std::cout << tab << std::endl;
    }

    tab.insert("alpha",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    tab.insert("beta",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    tab.insert("gamma",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"C", 2000},{"B", 20},
    });
    tab.insert("delta",  map<Table::FieldID, Table::ValueType> {
            {"A", 10}, {"D", 2000},{"B", 20},
    });
    tab.insertByIndex("zeta",  vector<Table::ValueType> {1, 2, 3,});
    tab.insertByIndex("keta",  vector<Table::ValueType> {1, 2, 3, 5});

    std::cout << tab << std::endl;

    std::cout << tab2 << std::endl;

    for (auto Object : tab2) {
        cout << Object.key() << " " << Object[0]
             << " " << Object["B"] + Object[3] << endl;
    }
    return 0;
}