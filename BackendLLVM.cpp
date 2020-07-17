#include "pch.h"
#include "BackendLLVM.h"
#include "Job.h"

bool BackendLLVM::check()
{
    return true;
}

JobResult BackendLLVM::preCompile(Job* ownerJob, int preCompileIndex)
{
    return JobResult::ReleaseJob;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    return true;
}
