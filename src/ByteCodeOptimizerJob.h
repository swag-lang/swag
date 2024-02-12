#pragma once
#include "ByteCodeOptContext.h"
#include "Job.h"
struct ByteCode;

struct ByteCodeOptimizerJob : Job
{
    virtual   ~ByteCodeOptimizerJob() = default;
    JobResult execute() override;

	// clang-format off
	void release() override { Allocator::free<ByteCodeOptimizerJob>(this); }
    // clang-format on

    ByteCodeOptContext optContext;
    int                startIndex;
    int                endIndex;
};
