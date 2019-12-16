#include "pch.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"

thread_local Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;

JobResult ModuleSemanticJob::execute()
{
    if (buildFileMode)
    {
        if (module->buildFile)
        {
            if (g_CommandLine.verboseBuildPass)
                g_Log.verbose(format("   module '%s', first semantic pass on '%s'", module->name.c_str(), module->buildFile->path.c_str()));

            auto job             = g_Pool_semanticJob.alloc();
            job->sourceFile      = module->buildFile;
            job->module          = module;
            job->dependentJob = dependentJob;
            job->nodes.push_back(module->buildFile->astRoot);
            g_ThreadMgr.addJob(job);
        }
    }
    else
    {
        for (auto file : module->files)
        {
            if (file->buildPass < BuildPass::Semantic)
                continue;
            if (file == module->buildFile)
                continue;

            auto job             = g_Pool_semanticJob.alloc();
            job->sourceFile      = file;
            job->module          = module;
            job->dependentJob = dependentJob;
            job->nodes.push_back(file->astRoot);
            g_ThreadMgr.addJob(job);
        }
    }

    return JobResult::ReleaseJob;
}
