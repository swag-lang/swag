#pragma once
#include "Job.h"
#include "BackendParameters.h"
#include "Backend.h"
struct Module;

struct ModulePrepOutputStage1Job : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};

struct ModulePrepOutputStage2Job : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};
