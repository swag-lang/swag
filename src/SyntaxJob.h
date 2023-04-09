#pragma once
#include "Job.h"
struct SyntaxJob;

struct SyntaxContext : public JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : public Job
{
    virtual ~SyntaxJob() = default;

    void release() override
    {
        Allocator::free<SyntaxJob>(this);
    }

    JobResult execute() override;

    SyntaxContext context;
};
