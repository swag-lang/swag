#include "pch.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "SemanticJob.h"

thread_local Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;

JobResult ModuleSemanticJob::execute()
{
    if(!module)
        return JobResult::ReleaseJob;

    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;

        auto job          = g_Pool_semanticJob.alloc();
        job->sourceFile   = file;
        job->module       = module;
        job->dependentJob = dependentJob;
        job->nodes.push_back(file->astRoot);
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
