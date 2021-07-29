#pragma once
#include "CommandLine.h"
#include "Os.h"

struct Timer
{
    Timer(atomic<uint64_t>* dest)
        : destValue{dest}
    {
    }

    Timer()
        : destValue{&internal}
    {
    }

    ~Timer()
    {
        stop();
    }

    void start(bool force = false)
    {
        if (g_CommandLine.stats || g_CommandLine.verbose || force)
        {
            started    = true;
            timeBefore = OS::timerNow();
        }
    }

    void stop(bool force = false)
    {
        if (g_CommandLine.stats || g_CommandLine.verbose || force)
        {
            if (started)
            {
                started   = false;
                timeAfter = OS::timerNow();
                elapsed   = timeAfter - timeBefore;
                *destValue += elapsed;
            }
        }
    }

    atomic<uint64_t>  internal;
    atomic<uint64_t>* destValue;
    uint64_t          elapsed    = 0;
    uint64_t          timeBefore = 0;
    uint64_t          timeAfter  = 0;

    bool started = false;
};