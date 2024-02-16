#pragma once
#include "Job.h"

struct ModuleSaveExportJob final : Job
{
	ModuleSaveExportJob();

	JobResult execute() override;
};
