#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Module.h"
#include "ThreadManager.h"
#include "CommandLine.h"
#include "Diagnostic.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    SWAG_ASSERT(!module->backend);
    module->backend = new BackendC(module);
    if (!module->backend->generate())
        return JobResult::ReleaseJob;
    if (module->buildPass < BuildPass::Full)
        return JobResult::ReleaseJob;

    // Compile a specific version, to test it
    if (g_CommandLine.test)
    {
        auto compileJob                     = g_Pool_moduleCompileJob.alloc();
        compileJob->module                  = module;
        compileJob->buildParameters         = module->buildParameters;
        compileJob->buildParameters.type    = BackendType::Exe;
        compileJob->buildParameters.postFix = ".test";
        compileJob->buildParameters.defines.clear();
        compileJob->buildParameters.defines.push_back("SWAG_IS_UNITTEST");
        g_ThreadMgr.addJob(compileJob);
    }

    // Compile the official normal version
    else
    {
        auto compileJob             = g_Pool_moduleCompileJob.alloc();
        compileJob->module          = module;
        compileJob->buildParameters = module->buildParameters;
        g_ThreadMgr.addJob(compileJob);
    }

    return JobResult::ReleaseJob;
}
