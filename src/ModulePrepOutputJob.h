#pragma once
#include "Job.h"
#include "BackendParameters.h"
#include "Backend.h"
struct Module;

struct ModulePrepOutputJob : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};

