#pragma once
struct Stats
{
    atomic<int>              numFiles              = 0;
    atomic<int>              numLines              = 0;
    atomic<int>              numModules            = 0;
    atomic<int>              numGenModules         = 0;
    atomic<int>              testFunctions         = 0;
    atomic<int>              numInstructions       = 0;
    atomic<int>              maxOpenFiles          = 0;
    atomic<int>              numOpenFiles          = 0;
    atomic<int>              allocatorMemory       = 0;
    atomic<int>              wastedAllocatorMemory = 0;
    atomic<int>              totalConcreteTypes    = 0;
    int                      numWorkers            = 0;
    chrono::duration<double> totalTime;
    atomic<double>           precompileTime = 0;
    atomic<double>           compileTime    = 0;
};

extern Stats g_Stats;