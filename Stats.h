#pragma once
struct Stats
{
    atomic<int>              numFiles        = 0;
    atomic<int>              numLines        = 0;
    atomic<int>              numModules      = 0;
    atomic<int>              numGenModules   = 0;
    atomic<int>              testFunctions   = 0;
    atomic<int>              numInstructions = 0;
    atomic<int>              maxOpenFiles    = 0;
    atomic<int>              numOpenFiles    = 0;
    atomic<int>              allocatorMemory = 0;
    int                      numWorkers      = 0;
    chrono::duration<double> totalTime;
};

extern Stats g_Stats;