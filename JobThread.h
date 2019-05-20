#pragma once
#include "PoolFactory.h"

struct JobThread
{
    JobThread();
    ~JobThread();

    void loop();
    void notifyJob();
    void waitJob();

    thread*            thread     = nullptr;
    bool               requestEnd = false;
    mutex              mutexNotify;
    condition_variable condVar;
};
