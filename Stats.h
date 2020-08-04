#pragma once
struct Stats
{
    atomic<int>    numFiles              = 0;
    atomic<int>    numLines              = 0;
    atomic<int>    numModules            = 0;
    atomic<int>    numGenModules         = 0;
    atomic<int>    testFunctions         = 0;
    atomic<int>    runFunctions          = 0;
    atomic<int>    numInstructions       = 0;
    atomic<int>    maxOpenFiles          = 0;
    atomic<int>    numOpenFiles          = 0;
    atomic<int>    allocatorMemory       = 0;
    atomic<int>    wastedAllocatorMemory = 0;
    atomic<int>    totalConcreteTypes    = 0;
    int            numWorkers            = 0;
    atomic<double> totalTime             = 0;
    atomic<double> syntaxTime            = 0;
    atomic<double> semanticTime          = 0;
    atomic<double> runTime               = 0;
    atomic<double> outputTime            = 0;
    atomic<double> precompileTimeJob     = 0;
    atomic<double> compileTime           = 0;
};

extern Stats g_Stats;