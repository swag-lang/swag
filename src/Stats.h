#pragma once
struct Stats
{
    int         numWorkers               = 0;
    atomic<int> numFiles                 = 0;
    atomic<int> numLines                 = 0;
    atomic<int> numModules               = 0;
    atomic<int> numGenModules            = 0;
    atomic<int> testFunctions            = 0;
    atomic<int> runFunctions             = 0;
    atomic<int> numInstructions          = 0;
    atomic<int> totalOptimsBC            = 0;
    atomic<int> totalConcreteTypes       = 0;
    atomic<int> totalConcreteStructTypes = 0;
    atomic<int> skippedModules           = 0;
    atomic<int> numInitPtr               = 0;
    atomic<int> numInitFuncPtr           = 0;

    atomic<uint64_t> readFilesTime             = 0;
    atomic<uint64_t> totalTime                 = 0;
    atomic<uint64_t> cfgTime                   = 0;
    atomic<uint64_t> syntaxTime                = 0;
    atomic<uint64_t> semanticModuleTime        = 0;
    atomic<uint64_t> runTime                   = 0;
    atomic<uint64_t> runTestTime               = 0;
    atomic<uint64_t> prepOutputTimeJob         = 0;
    atomic<uint64_t> prepOutputTimeJob_GenFunc = 0;
    atomic<uint64_t> prepOutputTimeJob_SaveObj = 0;
    atomic<uint64_t> genOutputTimeJob          = 0;
    atomic<uint64_t> optimBCTime               = 0;
    atomic<uint64_t> bootstrapTime             = 0;
    atomic<uint64_t> runtimeTime               = 0;
    atomic<uint64_t> outputDbgTime             = 0;

    atomic<size_t> allocatorMemory = 0;
    atomic<size_t> wastedMemory    = 0;
    atomic<size_t> memNodes        = 0;
    atomic<size_t> memNodesExt     = 0;
    atomic<size_t> memScopes       = 0;
    atomic<size_t> memSeg          = 0;
    atomic<size_t> memConcat       = 0;
    atomic<size_t> memTypes        = 0;
    atomic<size_t> memInstructions = 0;
    atomic<size_t> memBcStack      = 0;
    atomic<size_t> memSymName      = 0;
    atomic<size_t> memSymOver      = 0;
    atomic<size_t> memSymTable     = 0;
    atomic<size_t> memUtf8         = 0;

    atomic<size_t> sizeBackendDbg = 0;

    void print();
};

extern Stats g_Stats;