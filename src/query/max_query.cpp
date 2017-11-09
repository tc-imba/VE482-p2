#include "max_query.h"
#include "../db/db.h"
#include "../db/db_table.h"
#include "../formatter.h"

constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
	
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
		auto &table = db[this->targetTable];
		addIterationTask<MaxTask>(db, table);
		return make_unique<SuccessMsgResult>(qname);
	}
	catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No such table."s
        );
    } catch (const IllFormedQueryCondition &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                e.what()
        );
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unknown error '?'"_f % e.what()
        );
    } catch (const exception &e) {
        return std::make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Unkonwn error '?'."_f % e.what()
        );
    }
}

std::string MaxQuery::toString() {
    return "QUERY = MAX " + this->targetTable + "\"";
}

QueryResult::Ptr MaxQuery::combine() {
    using namespace std;
    if (taskComplete < tasks.size()) {
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Not completed yet."s
        );
    }
	size_t counter = 0;
	/*std::vector<int> totalFieldMax(std::move(this->getTask(0)->getFieldMax()));
	for (int i = 1; i < tasks.size(); ++i) {
		MaxTask *thisTask = this->getTask(i);
		for (int j = 0; j < operands.size(); ++j) {
			if (totalFieldMax[j] < thisTask->getFieldMax()[j]) totalFieldMax[j]= thisTask->getFieldMax()[j];
		}
	}*/
	return make_unique<RecordCountResult>(counter);
}

void MaxTask::execute() {
	auto query = getQuery();
    try {
		int numFields = this->getQuery()->getOperands().size();
		//Max.insert(Max.end(), numFields, INFINITY);
		if (query->getCondition().empty())
		{
			for (auto it = begin; it != end; ++it) {
				for (int i = 0; i < numFields; ++i) {
					int number = (*it)[(this->getQuery()->getOperands()[i])];
					if( number > Max[i]) Max[i]=number;
				}	
			}
		}
		else {
			for (auto it = begin; it != end; ++it) {
				if (query->evalCondition(query->getCondition(), *it)) {
					for (int i = 0; i < numFields; ++i) {
						int number = (*it)[(this->getQuery()->getOperands()[i])];
						if (number > Max[i]) Max[i] = number;
					}
				}
			}
		}
        Task::execute();
    } catch (const IllFormedQueryCondition &e) {
        return;
        // @TODO manage query exceptions later
        /*return std::make_unique<ErrorMsgResult>(
                query->qname, table.name(),
                e.what()
        );*/
    }
}

