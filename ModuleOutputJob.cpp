#include "pch.h"
#include "ModuleOutputJob.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Module.h"
#include "ThreadManager.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    module->backend = new BackendC(module);
    if (!module->backend->generate())
        return JobResult::ReleaseJob;

	// Compile the generated files
    auto compileJob               = g_Pool_moduleCompileJob.alloc();
    compileJob->module            = module;
    compileJob->backendParameters = module->backendParameters;
    g_ThreadMgr.addJob(compileJob);

    return JobResult::ReleaseJob;
}
