#include "pch.h"
#include "ModuleGenDocJob.h"

JobResult ModuleGenDocJob::execute()
{
    genDoc.generate(module, docKind);
    return JobResult::ReleaseJob;
}
