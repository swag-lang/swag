#pragma once
#include "Job.h"
struct ByteCode;

struct ByteCodeOptimizerJob : public Job
{
    JobResult   execute() override;
    static bool optimize(Module* module, int startIndex, int endIndex, bool isAsync);
    int         startIndex, endIndex;
};

extern thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;
