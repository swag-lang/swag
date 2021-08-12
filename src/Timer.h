#pragma once
#include "CommandLine.h"
#include "Os.h"

struct Timer
{
    Timer(atomic<uint64_t>* dest, bool force = false)
        : destValue{dest}
    {
        start(force);
    }

    ~Timer()
    {
        stop();
    }

    void start(bool force)
    {
        if ((g_CommandLine && g_CommandLine->stats) || force)
            timeBefore = OS::timerNow();
    }

    void stop()
    {
        if (timeBefore)
            *destValue += OS::timerNow() - timeBefore;
    }

    atomic<uint64_t>* destValue;
    uint64_t          timeBefore = 0;
};