#pragma once
#include "Job.h"

struct SourceFile;

struct LoadSourceFileJob final : Job
{
	LoadSourceFileJob();

	void      release() override;
	JobResult execute() override;
};
