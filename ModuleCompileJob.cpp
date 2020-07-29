#include "pch.h"
#include "ModuleCompileJob.h"
#include "ModuleRunJob.h"
#include "Backend.h"
#include "Stats.h"
#include "Workspace.h"
#include "ThreadManager.h"

thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    auto timeBefore = chrono::high_resolution_clock::now();

    module->backend->compile(buildParameters);

    auto                     timeAfter = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed   = timeAfter - timeBefore;
    g_Stats.compileTime                = g_Stats.compileTime + elapsed.count();

    g_Stats.numGenModules++;

    // Notify we are done
    if (mutexDone)
    {
        unique_lock lk(*mutexDone);
        condVar->notify_all();
    }

    // The module is built, so notify (we notify before the test)
    module->setHasBeenBuilt(BUILDRES_COMPILER);

    // Test job
    // Do not set job->dependentJob, because nobody is dependent on that execution
    // Test can be run "on the void"
    if ((g_CommandLine.runBackendTests && buildParameters.compileType == BackendCompileType::Test) ||
        (g_CommandLine.run && buildParameters.outputType == BackendOutputType::Binary))
    {
        auto job             = g_Pool_moduleRunJob.alloc();
        job->module          = module;
        job->buildParameters = buildParameters;
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
