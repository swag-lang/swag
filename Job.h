#pragma once
#include "Pool.h"
struct JobThread;

enum class JobResult
{
    ReleaseJob,
    KeepJobAlive,
};

struct Job : public PoolElement
{
    virtual JobResult execute() = 0;

    void reset() override
    {
        pendingIndex = -1;
    }

    JobThread* thread       = nullptr;
    int        pendingIndex = -1;
};
