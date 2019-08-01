#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Module.h"
#include "ThreadManager.h"
#include "CommandLine.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    assert(!module->backend);
    module->backend = new BackendC(module);
    if (!module->backend->generate())
        return JobResult::ReleaseJob;
    if (module->buildPass < BuildPass::Full)
        return JobResult::ReleaseJob;

    // Compile a specific version, to test it
    if (g_CommandLine.unittest)
    {
        auto compileJob                       = g_Pool_moduleCompileJob.alloc();
        compileJob->module                    = module;
        compileJob->backendParameters         = module->backendParameters;
        compileJob->backendParameters.type    = BackendType::Exe;
        compileJob->backendParameters.postFix = ".test";
        compileJob->backendParameters.defines.clear();
        compileJob->backendParameters.defines.push_back("SWAG_IS_UNITTEST");
        g_ThreadMgr.addJob(compileJob);
    }

    // Compile the official normal version
    else
    {
        auto compileJob               = g_Pool_moduleCompileJob.alloc();
        compileJob->module            = module;
        compileJob->backendParameters = module->backendParameters;
        g_ThreadMgr.addJob(compileJob);
    }

    return JobResult::ReleaseJob;
}
