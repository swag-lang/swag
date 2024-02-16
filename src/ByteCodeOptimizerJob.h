#pragma once
#include "ByteCodeOptContext.h"
#include "Job.h"
struct ByteCode;

struct ByteCodeOptimizerJob final : Job
{
	JobResult execute() override;

	
	void release() override { Allocator::free<ByteCodeOptimizerJob>(this); }
	

	ByteCodeOptContext optContext;
	int                startIndex;
	int                endIndex;
};
