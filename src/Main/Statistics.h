#pragma once
#ifdef SWAG_STATS
#include "Backend/ByteCode/ByteCodeOp.h"

struct Stats
{
    std::atomic<uint32_t> numFiles                 = 0;
    std::atomic<uint32_t> numLines                 = 0;
    std::atomic<uint32_t> numModules               = 0;
    std::atomic<uint32_t> numGenModules            = 0;
    std::atomic<uint32_t> testFunctions            = 0;
    std::atomic<uint32_t> runFunctions             = 0;
    std::atomic<uint32_t> numInstructions          = 0;
    std::atomic<uint32_t> totalOptimBC             = 0;
    std::atomic<uint32_t> totalConcreteTypes       = 0;
    std::atomic<uint32_t> totalConcreteStructTypes = 0;
    std::atomic<uint32_t> numInitPtr               = 0;
    std::atomic<uint32_t> numInitFuncPtr           = 0;
    std::atomic<uint32_t> numNodes                 = 0;
    std::atomic<uint32_t> releaseNodes             = 0;
    std::atomic<uint32_t> numTokens                = 0;
    std::atomic<uint32_t> numCustom0               = 0;
    std::atomic<uint32_t> numCustom1               = 0;

    std::atomic<uint64_t> readFilesTime            = 0;
    std::atomic<uint64_t> cfgTime                  = 0;
    std::atomic<uint64_t> tokenizerTime            = 0;
    std::atomic<uint64_t> syntaxTime               = 0;
    std::atomic<uint64_t> semanticTime             = 0;
    std::atomic<uint64_t> runTime                  = 0;
    std::atomic<uint64_t> runTestTime              = 0;
    std::atomic<uint64_t> prepOutputStage1TimeJob  = 0;
    std::atomic<uint64_t> prepOutputStage2TimeJob  = 0;
    std::atomic<uint64_t> prepOutputTimeJobGenFunc = 0;
    std::atomic<uint64_t> prepOutputTimeJobSaveObj = 0;
    std::atomic<uint64_t> genOutputTimeJob         = 0;
    std::atomic<uint64_t> optimBCTime              = 0;
    std::atomic<uint64_t> bootstrapTime            = 0;
    std::atomic<uint64_t> runtimeTime              = 0;
    std::atomic<uint64_t> outputDbgTime            = 0;

    std::atomic<size_t> allocatedMemory    = 0;
    std::atomic<size_t> maxAllocatedMemory = 0;
    std::atomic<size_t> memNodes           = 0;
    std::atomic<size_t> memNodesExt        = 0;
    std::atomic<size_t> memNodesLiteral    = 0;
    std::atomic<size_t> memIdentifiersExt  = 0;
    std::atomic<size_t> memScopes          = 0;
    std::atomic<size_t> memSeg             = 0;
    std::atomic<size_t> memConcat          = 0;
    std::atomic<size_t> memTypes           = 0;
    std::atomic<size_t> memParams          = 0;
    std::atomic<size_t> memInstructions    = 0;
    std::atomic<size_t> memBcStack         = 0;
    std::atomic<size_t> memSymName         = 0;
    std::atomic<size_t> memSymOver         = 0;
    std::atomic<size_t> memSymTable        = 0;
    std::atomic<size_t> memUtf8            = 0;
    std::atomic<size_t> memUtf8CStr        = 0;
    std::atomic<size_t> memNew             = 0;
    std::atomic<size_t> memStd             = 0;
    std::atomic<size_t> memFileBuffer      = 0;

    std::atomic<size_t> sizeBackendDbg = 0;

    std::atomic<uint32_t> countOpFreq[static_cast<int>(ByteCodeOp::End) + 1][static_cast<int>(ByteCodeOp::End) + 1] = {{0}};

#ifdef SWAG_DEV_MODE
    std::atomic<uint32_t> countTypesByKind[50] = {0};
#endif

    void print() const;
    void printFreq();
};

extern Stats g_Stats;
#endif // SWAG_STATS
