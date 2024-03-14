#include "pch.h"
#include "Semantic/ModuleSemanticJob.h"
#include "Semantic/SemanticJob.h"
#include "Wmf/Module.h"

JobResult ModuleSemanticJob::execute()
{
    if (!module)
        return JobResult::ReleaseJob;

    for (const auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;
        SWAG_ASSERT(file->module == module);
        const auto job = SemanticJob::newJob(dependentJob, file, file->astRoot, false);
        jobsToAdd.push_back(job);
    }

    return JobResult::ReleaseJob;
}
