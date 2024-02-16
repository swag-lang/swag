#pragma once
#ifdef SWAG_STATS
#include "ByteCodeOp.h"

struct Stats
{
	atomic<int> numFiles                 = 0;
	atomic<int> numLines                 = 0;
	atomic<int> numModules               = 0;
	atomic<int> numGenModules            = 0;
	atomic<int> testFunctions            = 0;
	atomic<int> runFunctions             = 0;
	atomic<int> numInstructions          = 0;
	atomic<int> totalOptimBC             = 0;
	atomic<int> totalConcreteTypes       = 0;
	atomic<int> totalConcreteStructTypes = 0;
	atomic<int> numInitPtr               = 0;
	atomic<int> numInitFuncPtr           = 0;
	atomic<int> numNodes                 = 0;
	atomic<int> releaseNodes             = 0;
	atomic<int> numTokens                = 0;

	atomic<uint64_t> readFilesTime            = 0;
	atomic<uint64_t> cfgTime                  = 0;
	atomic<uint64_t> tokenizerTime            = 0;
	atomic<uint64_t> syntaxTime               = 0;
	atomic<uint64_t> semanticTime             = 0;
	atomic<uint64_t> runTime                  = 0;
	atomic<uint64_t> runTestTime              = 0;
	atomic<uint64_t> prepOutputStage1TimeJob  = 0;
	atomic<uint64_t> prepOutputStage2TimeJob  = 0;
	atomic<uint64_t> prepOutputTimeJobGenFunc = 0;
	atomic<uint64_t> prepOutputTimeJobSaveObj = 0;
	atomic<uint64_t> genOutputTimeJob         = 0;
	atomic<uint64_t> optimBCTime              = 0;
	atomic<uint64_t> bootstrapTime            = 0;
	atomic<uint64_t> runtimeTime              = 0;
	atomic<uint64_t> outputDbgTime            = 0;

	atomic<size_t> allocatedMemory    = 0;
	atomic<size_t> maxAllocatedMemory = 0;
	atomic<size_t> memNodes           = 0;
	atomic<size_t> memNodesExt        = 0;
	atomic<size_t> memNodesLiteral    = 0;
	atomic<size_t> memScopes          = 0;
	atomic<size_t> memSeg             = 0;
	atomic<size_t> memConcat          = 0;
	atomic<size_t> memTypes           = 0;
	atomic<size_t> memParams          = 0;
	atomic<size_t> memInstructions    = 0;
	atomic<size_t> memBcStack         = 0;
	atomic<size_t> memSymName         = 0;
	atomic<size_t> memSymOver         = 0;
	atomic<size_t> memSymTable        = 0;
	atomic<size_t> memUtf8            = 0;
	atomic<size_t> memUtf8CStr        = 0;
	atomic<size_t> memNew             = 0;
	atomic<size_t> memStd             = 0;
	atomic<size_t> memFileBuffer      = 0;

	atomic<size_t> sizeBackendDbg = 0;

	atomic<int> countOpFreq[static_cast<int>(ByteCodeOp::End) + 1][static_cast<int>(ByteCodeOp::End) + 1] = {{0}};

#ifdef SWAG_DEV_MODE
	atomic<int> countTypesByKind[50] = {0};
#endif

	void print() const;
	void printFreq();
};

extern Stats g_Stats;
#endif // SWAG_STATS
