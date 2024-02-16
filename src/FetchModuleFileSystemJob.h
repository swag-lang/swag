#pragma once
#include "Job.h"

struct FetchModuleFileSystemJob final : Job
{
	FetchModuleFileSystemJob();

	JobResult execute() override;
	bool      collectSourceFiles = true;
};
