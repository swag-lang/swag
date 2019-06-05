#pragma once
struct Stats
{
    atomic<int> numFiles   = 0;
    atomic<int> numLines   = 0;
    atomic<int> numModules = 0;
    int         numWorkers = 0;
};

extern Stats g_Stats;