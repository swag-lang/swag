#pragma once
#include "Job.h"
#include "GenDoc.h"
#include "Runtime.h"

struct ModuleGenDocJob : public Job
{
    JobResult execute() override;

    BuildCfgDocKind docKind;
    GenDoc          genDoc;
};
