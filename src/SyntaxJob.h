#pragma once
#include "Job.h"
struct SyntaxJob;

struct SyntaxContext : public JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : public Job
{
    void release() override
    {
        g_Allocator.free<SyntaxJob>(this);
    }

    JobResult execute() override;

    SyntaxContext context;
};
