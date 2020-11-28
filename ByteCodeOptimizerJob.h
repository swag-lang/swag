#pragma once
#include "Job.h"
struct ByteCode;
struct ByteCodeOptContext;

struct ByteCodeOptimizerJob : public Job
{
    ByteCodeOptimizerJob();
    JobResult execute() override;
    bool      optimize(bool isAsync);

    vector<function<void(ByteCodeOptContext*)>> passes;
    int                                         startIndex, endIndex;
};

extern thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;
