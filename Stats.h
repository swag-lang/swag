#pragma once
struct Stats
{
    atomic<int>    numFiles             = 0;
    atomic<int>    numLines             = 0;
    atomic<int>    numModules           = 0;
    atomic<int>    numGenModules        = 0;
    atomic<int>    testFunctions        = 0;
    atomic<int>    runFunctions         = 0;
    atomic<int>    numInstructions      = 0;
    atomic<int>    totalOptimsBC        = 0;
    atomic<int>    maxOpenFiles         = 0;
    atomic<int>    numOpenFiles         = 0;
    atomic<int>    allocatorMemory      = 0;
    atomic<int>    totalConcreteTypes   = 0;
    int            numWorkers           = 0;
    atomic<double> readFiles            = 0;
    atomic<double> totalTime            = 0;
    atomic<double> syntaxTime           = 0;
    atomic<double> semanticCompilerTime = 0;
    atomic<double> semanticModuleTime   = 0;
    atomic<double> runTime              = 0;
    atomic<double> runTestTime          = 0;
    atomic<double> outputTime           = 0;
    atomic<double> prepOutputTimeJob    = 0;
    atomic<double> genOutputTime        = 0;
    atomic<double> optimBCTime          = 0;
    atomic<int>    memNodes             = 0;
    atomic<int>    memScopes            = 0;
    atomic<int>    memSeg               = 0;
    atomic<int>    memConcat            = 0;
    atomic<int>    memTypes             = 0;
    atomic<int>    memInstructions      = 0;
};

extern Stats g_Stats;