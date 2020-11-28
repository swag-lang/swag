#pragma once
#include "CommandLine.h"

struct Timer
{
    Timer(atomic<double>& dest)
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
            timeBefore = chrono::high_resolution_clock::now();
        }
    }

    void stop(bool force = false)
    {
        if (g_CommandLine.stats || g_CommandLine.verbose || force)
        {
            if (started)
            {
                started        = false;
                auto timeAfter = chrono::high_resolution_clock::now();
                elapsed        = timeAfter - timeBefore;
                destValue      = destValue + elapsed.count();
            }
        }
    }

    bool                                      started = false;
    atomic<double>                            internal;
    chrono::duration<double>                  elapsed;
    chrono::high_resolution_clock::time_point timeBefore;
    atomic<double>&                           destValue;
};