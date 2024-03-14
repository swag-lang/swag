#pragma once
#include "Os/Os.h"

struct Timer
{
    explicit Timer(std::atomic<uint64_t>* dest, bool force = false) :
        destValue{dest}
    {
        start(force);
    }

    ~Timer()
    {
        stop();
    }

    void start([[maybe_unused]] bool force)
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

    std::atomic<uint64_t>* destValue;
    uint64_t               timeBefore = 0;
};
