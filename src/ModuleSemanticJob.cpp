#include "pch.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "SemanticJob.h"

JobResult ModuleSemanticJob::execute()
{
    if (!module)
        return JobResult::ReleaseJob;

    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;

        auto job          = g_Allocator.alloc<SemanticJob>();
        job->sourceFile   = file;
        job->module       = module;
        job->dependentJob = dependentJob;
        job->nodes.push_back(file->astRoot);
        jobsToAdd.push_back(job);
    }

    return JobResult::ReleaseJob;
}
