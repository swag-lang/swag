#pragma once
#include "Os.h"

struct Timer
{
    explicit Timer(atomic<uint64_t>* dest, bool force = false) :
        destValue{dest}
    {
        start(force);
    }

    ~Timer()
    {
        stop();
    }

    void start(bool force)
    {
#ifdef SWAG_STATS
        timeBefore = OS::timerNow();
#else
        if (force)
            timeBefore = OS::timerNow();
#endif
    }

    void stop() const
    {
        if (timeBefore)
            *destValue += OS::timerNow() - timeBefore;
    }

    atomic<uint64_t>* destValue;
    uint64_t          timeBefore = 0;
};
