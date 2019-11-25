#include "pch.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"

Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;

JobResult ModuleSemanticJob::execute()
{
    SourceFile* buildFile = module->buildFiles.empty() ? nullptr : module->buildFiles[0];
    if (buildFileMode)
    {
        // Be sure we have maximum one "build.swg" file
        if (module->buildFiles.size() > 1)
        {
            Diagnostic                diag(format("'build.swg' file defined multiple times in module '%s'", module->name.c_str()));
            vector<const Diagnostic*> notes;
            for (auto one : module->buildFiles)
                notes.push_back(new Diagnostic{format("one file is '%s'", one->path.string().c_str()), DiagnosticLevel::Note});
            module->buildFiles[0]->report(diag, notes);
            return JobResult::ReleaseJob;
        }

        if (buildFile)
        {
            if (g_CommandLine.verboseBuildPass)
                g_Log.verbose(format("   module '%s', first semantic pass on '%s'", module->name.c_str(), buildFile->path.string().c_str()));

            auto job        = g_Pool_semanticJob.alloc();
            job->sourceFile = buildFile;
            job->nodes.push_back(buildFile->astRoot);
            g_ThreadMgr.addJob(job);
        }
    }
    else
    {
        for (auto file : module->files)
        {
            if (file->buildPass < BuildPass::Semantic)
                continue;
            if (file == buildFile)
				continue;

            auto job        = g_Pool_semanticJob.alloc();
            job->sourceFile = file;
            job->nodes.push_back(file->astRoot);
            g_ThreadMgr.addJob(job);
        }
    }

    return JobResult::ReleaseJob;
}
