#pragma once
#include "Job.h"
#include "Concat.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    JobResult execute() override;
    BackendC* backend      = nullptr;
    ByteCode* byteCodeFunc = nullptr;
    Concat    concat;
};

extern thread_local PoolFree<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
