#pragma once
#include "Job.h"
struct SyntaxJob;

struct SyntaxContext : JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : Job
{
    virtual   ~SyntaxJob() = default;
    void      release() override;
    JobResult execute() override;

    SyntaxContext context;
};
