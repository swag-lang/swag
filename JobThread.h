#pragma once
#include "PoolFactory.h"

struct JobThread
{
    JobThread();
    ~JobThread();

    void loop();
    void notifyJob();
    void waitJob();

    thread*            m_thread     = nullptr;
    bool               m_requestEnd = false;
    mutex              m_mutexNotify;
    condition_variable m_Cv;
};
