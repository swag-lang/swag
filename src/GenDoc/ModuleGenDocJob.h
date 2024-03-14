#pragma once
#include "GenDoc.h"
#include "Threading/Job.h"
#include "Backend/Runtime.h"

struct ModuleGenDocJob final : Job
{
    JobResult execute() override;

    BuildCfgDocKind docKind;
    GenDoc          genDoc;
};
