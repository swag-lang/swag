#pragma once
#include "Job.h"
#include "BackendFunctionBodyJobBase.h"

struct BackendLLVMFunctionBodyJob : public BackendFunctionBodyJobBase
{
    virtual ~BackendLLVMFunctionBodyJob() = default;
    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendLLVMFunctionBodyJob>(this);
    }
};
