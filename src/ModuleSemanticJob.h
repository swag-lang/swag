#pragma once
#include "Job.h"
struct Module;

struct ModuleSemanticJob : public Job
{
    JobResult execute() override;
};

