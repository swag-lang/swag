#include "pch.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"

Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;

JobResult ModuleSemanticJob::execute()
{
    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;

        auto job        = g_Pool_semanticJob.alloc();
        job->sourceFile = file;
        job->nodes.push_back(file->astRoot);
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
