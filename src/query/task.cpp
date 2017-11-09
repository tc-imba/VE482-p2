//
// Created by liu on 17-11-6.
//

#include "task.h"
#include "query.h"

void Task::execute() {
    getQuery()->complete();
}