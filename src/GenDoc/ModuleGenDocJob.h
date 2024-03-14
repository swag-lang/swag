#pragma once
#include "Backend/Runtime.h"
#include "GenDoc/GenDoc.h"
#include "Threading/Job.h"

struct ModuleGenDocJob final : Job
{
    JobResult execute() override;

    BuildCfgDocKind docKind;
    GenDoc          genDoc;
};
