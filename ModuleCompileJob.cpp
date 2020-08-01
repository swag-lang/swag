#include "pch.h"
#include "ModuleCompileJob.h"
#include "Backend.h"
#include "Workspace.h"

thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    auto timeBefore = chrono::high_resolution_clock::now();

    // Compile
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

    return JobResult::ReleaseJob;
}
