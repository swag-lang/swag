#pragma once

struct Timer
{
    Timer(atomic<double>& dest, bool force = false)
        : destValue{dest}
        , wasForced{force}
    {
        if (g_CommandLine.stats)
            timeBefore = chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if (g_CommandLine.stats || wasForced)
        {
            auto                     timeAfter = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed   = timeAfter - timeBefore;
            destValue                          = destValue + elapsed.count();
        }
    }

    chrono::high_resolution_clock::time_point timeBefore;
    atomic<double>&                           destValue;
    bool                                      wasForced = false;
};