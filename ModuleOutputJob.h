#pragma once
#include "Job.h"
struct Module;

struct ModuleOutputJob : public Job
{
    JobResult execute() override;

    Module* module = nullptr;
};
