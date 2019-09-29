#pragma once
#ifdef SWAG_HAS_ASSERT
#include "SpinLock.h"

struct RaceCondition
{
    struct Instance
    {
        std::atomic<int> count;
        SpinLock         mutex;
        thread::id       lastThreadID;
        bool             defined = false;

        Instance() = default;
    };

    RaceCondition()                     = delete;
    RaceCondition(const RaceCondition&) = delete;
    RaceCondition(Instance* _instance);
    ~RaceCondition();

    Instance* myInstance;
};

#endif