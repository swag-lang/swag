#pragma once
#include "Job.h"
#include "BackendFunctionBodyJobBase.h"

struct BackendSCBEFunctionBodyJob : public BackendFunctionBodyJobBase
{
    virtual ~BackendSCBEFunctionBodyJob() = default;

    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendSCBEFunctionBodyJob>(this);
    }
};
