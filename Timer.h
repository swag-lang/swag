#pragma once
#include "CommandLine.h"
#include "Os.h"

struct Timer
{
    Timer(atomic<uint64_t>& dest)
        : destValue{dest}
    {
    }

    Timer()
        : destValue{internal}
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
                started        = false;
                auto timeAfter = OS::timerNow();
                elapsed        = timeAfter - timeBefore;
                destValue += elapsed;
            }
        }
    }

    bool              started = false;
    atomic<uint64_t>  internal;
    uint64_t          elapsed;
    uint64_t          timeBefore;
    atomic<uint64_t>& destValue;
};