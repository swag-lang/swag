#pragma once
#include "Backend.h"
#include "BackendParameters.h"
#include "Job.h"
struct Module;

struct ModulePrepOutputStage1Job : Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};

struct ModulePrepOutputStage2Job : Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};
