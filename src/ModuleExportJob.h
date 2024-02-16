#pragma once
#include "Job.h"

struct Backend;

struct ModuleExportJob final : Job
{
	JobResult execute() override;

	Backend* backend = nullptr;
};
