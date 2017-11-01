//
// Created by 38569 on 2017/11/1.
//

#ifndef LEMONDB_TASK_RESULTS_H
#define LEMONDB_TASK_RESULTS_H

class TaskResult {
public:
    typedef std::unique_ptr<TaskResult> Ptr;

    virtual bool success()  = 0;

    virtual std::string toString() = 0;

    virtual ~TaskResult() = default;
};

#endif //LEMONDB_TASK_RESULTS_H
