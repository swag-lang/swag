#pragma once
#include "Mutex.h"
struct Job;

struct JobThread
{
    JobThread(int rank);
    ~JobThread();

    void loop();
    void notifyJob();

    mutex              mutexNotify;
    condition_variable condVar;

    thread* thread     = nullptr;
    int     threadRank = -1;

    bool requestEnd = false;
};

extern thread_local int g_ThreadIndex;
