#pragma once
#include "Job.h"
struct SyntaxJob;

struct SyntaxContext : JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : Job
{
    virtual ~SyntaxJob() = default;

    void release() override
    {
        Allocator::free<SyntaxJob>(this);
    }

    JobResult execute() override;

    SyntaxContext context;
};
