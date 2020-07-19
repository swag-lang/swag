#include "pch.h"
#include "ModuleCompileJob.h"
#include "ModuleTestJob.h"
#include "Backend.h"
#include "Stats.h"
#include "Workspace.h"
#include "ThreadManager.h"

thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    if (!module->backend->mustCompile)
    {
        if (buildParameters.compileType == BackendCompileType::Test)
            g_Log.messageHeaderCentered("Skipping build test", module->name.c_str(), LogColor::Gray);
        else
            g_Log.messageHeaderCentered("Skipping build", module->name.c_str(), LogColor::Gray);
    }
    else
    {
        const char* header = (buildParameters.compileType == BackendCompileType::Test) ? "Building test" : "Building";
        g_Log.messageHeaderCentered(header, module->name.c_str());

        // Generate output file
        module->backend->compile(buildParameters);
    }

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
    if (g_CommandLine.runBackendTests)
    {
        if (buildParameters.compileType == BackendCompileType::Test)
        {
            auto job             = g_Pool_moduleTestJob.alloc();
            job->module          = module;
            job->buildParameters = buildParameters;
            g_ThreadMgr.addJob(job);
        }
    }

    return JobResult::ReleaseJob;
}
