#pragma once
#include "GenDoc.h"
#include "Job.h"
#include "Runtime.h"

struct ModuleGenDocJob final : Job
{
	JobResult execute() override;

	BuildCfgDocKind docKind;
	GenDoc          genDoc;
};
