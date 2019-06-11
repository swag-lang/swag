#pragma once
struct Stats
{
    atomic<int>              numFiles      = 0;
    atomic<int>              numLines      = 0;
    atomic<int>              numModules    = 0;
    atomic<int>              numGenModules = 0;
    int                      numWorkers    = 0;
    chrono::duration<double> totalTime;
    chrono::duration<double> outputTime;
};

extern Stats g_Stats;