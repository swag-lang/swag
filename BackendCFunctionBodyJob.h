#pragma once
#include "Job.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    JobResult execute() override;
    BackendC* backend      = nullptr;
    ByteCode* byteCodeFunc = nullptr;
};

extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
