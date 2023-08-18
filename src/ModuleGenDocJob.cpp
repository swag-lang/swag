#include "pch.h"
#include "ModuleGenDocJob.h"

JobResult ModuleGenDocJob::execute()
{
    genDoc.generate(module);
    return JobResult::ReleaseJob;
}
