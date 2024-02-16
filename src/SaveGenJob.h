#pragma once
#include "Job.h"

struct Module;

struct SaveGenJob final : Job
{
	SaveGenJob();

	void      release() override;
	JobResult execute() override;

	static bool flush(Module* module);

	Utf8 content;
};
