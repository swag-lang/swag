#pragma once
#include "CommandLine.h"

struct Timer
{
    Timer(atomic<double>& dest)
        : destValue{dest}
    {
    }

    void start(bool force = false)
    {
        if (g_CommandLine.stats || force)
            timeBefore = chrono::high_resolution_clock::now();
    }

    void stop(bool force = false)
    {
        if (g_CommandLine.stats || force)
        {
            auto timeAfter = chrono::high_resolution_clock::now();
            elapsed        = timeAfter - timeBefore;
            destValue      = destValue + elapsed.count();
        }
    }

    chrono::duration<double>                  elapsed;
    chrono::high_resolution_clock::time_point timeBefore;
    atomic<double>&                           destValue;
};