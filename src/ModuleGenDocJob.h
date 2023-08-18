#pragma once
#include "Job.h"
#include "GenDoc.h"
#include "Runtime.h"

struct ModuleGenDocJob : public Job
{
    JobResult execute() override;

    DocKind docKind;
    GenDoc  genDoc;
};
