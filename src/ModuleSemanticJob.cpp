#include "pch.h"
#include "ModuleSemanticJob.h"
#include "Module.h"
#include "SemanticJob.h"

JobResult ModuleSemanticJob::execute()
{
    if (!module)
        return JobResult::ReleaseJob;

    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;
        SWAG_ASSERT(file->module == module);
        auto job = SemanticJob::newJob(dependentJob, file, file->astRoot, false);
        jobsToAdd.push_back(job);
    }

    return JobResult::ReleaseJob;
}
