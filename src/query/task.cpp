//
// Created by liu on 17-11-6.
//

#include "task.h"
#include "query.h"

TaskQuery *Task::getQuery() {
    return dynamic_cast<TaskQuery *>(query);
}

void Task::execute() {
    getQuery()->complete();
}