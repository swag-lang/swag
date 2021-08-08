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
        if (g_CommandLine.stats || force)
        {
            started    = true;
            timeBefore = OS::timerNow();
        }
    }

    void stop()
    {
        if (started)
        {
            started   = false;
            timeAfter = OS::timerNow();
            elapsed   = timeAfter - timeBefore;
            *destValue += elapsed;
        }
    }

    atomic<uint64_t>* destValue;
    uint64_t          elapsed    = 0;
    uint64_t          timeBefore = 0;
    uint64_t          timeAfter  = 0;

    bool started = false;
};