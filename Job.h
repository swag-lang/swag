#pragma once
#include "Pool.h"
#include "RaceCondition.h"
struct JobThread;

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

struct Job : public PoolElement
{
    virtual JobResult execute() = 0;

    void reset() override
    {
        pendingIndex = -1;
        thread       = nullptr;
    }

    JobThread* thread       = nullptr;
    int        pendingIndex = -1;
#ifdef SWAG_HAS_ASSERT
    RaceCondition::Instance raceCondition;
#endif
};
