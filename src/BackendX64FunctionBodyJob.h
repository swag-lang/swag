#pragma once
#include "Job.h"
#include "BackendFunctionBodyJobBase.h"

struct BackendX64FunctionBodyJob : public BackendFunctionBodyJobBase
{
    virtual ~BackendX64FunctionBodyJob() = default;

    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendX64FunctionBodyJob>(this);
    }
};
