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
    module->backend = new BackendC(module);
    if (!module->backend->generate())
        return JobResult::ReleaseJob;

    // Compile the official normal version
    auto compileJob               = g_Pool_moduleCompileJob.alloc();
    compileJob->module            = module;
    compileJob->backendParameters = module->backendParameters;
    g_ThreadMgr.addJob(compileJob);

    // Compile a specific version, to test it
    if (g_CommandLine.unittest && g_CommandLine.runBackendTests && !module->byteCodeTestFunc.empty())
    {
        compileJob                            = g_Pool_moduleCompileJob.alloc();
        compileJob->module                    = module;
        compileJob->backendParameters         = module->backendParameters;
        compileJob->backendParameters.type    = BackendType::Exe;
        compileJob->backendParameters.postFix = ".test";
        compileJob->backendParameters.defines.clear();
        compileJob->backendParameters.defines.push_back("SWAG_IS_UNITTEST");
        compileJob->backendParameters.runTests = true;
        g_ThreadMgr.addJob(compileJob);
    }

    return JobResult::ReleaseJob;
}
