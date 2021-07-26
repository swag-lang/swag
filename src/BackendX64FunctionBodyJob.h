#pragma once
#include "Job.h"
#include "BackendFunctionBodyJobBase.h"

struct BackendX64FunctionBodyJob : public BackendFunctionBodyJobBase
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<BackendX64FunctionBodyJob>(this);
    }
};

