#include "pch.h"
#include "ModuleCompileJob.h"
#include "ModuleTestJob.h"
#include "BackendC.h"
#include "Stats.h"
#include "Workspace.h"
#include "Os.h"
#include "ThreadManager.h"

thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    // Generate output file
    module->backend->compile(buildParameters);
    g_Stats.numGenModules++;

    // Notify we are done
    if (mutexDone)
    {
        unique_lock lk(*mutexDone);
        condVar->notify_all();
    }

    // The module is built, so notify (we notify before the test)
    module->setHasBeenBuilt(ModuleBuildResult::Full);

    // Test job
	// Do not set job->dependentJob, because nobody is dependent on that execution
	// Test can be run "on the void"
    if (g_CommandLine.runBackendTests)
    {
        if (buildParameters.flags & BUILDPARAM_FOR_TEST)
        {
            auto job             = g_Pool_moduleTestJob.alloc();
            job->module          = module;
            job->buildParameters = buildParameters;
            g_ThreadMgr.addJob(job);
        }
    }

    return JobResult::ReleaseJob;
}
