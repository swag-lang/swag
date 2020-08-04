#include "pch.h"
#include "ModuleCompileJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"

thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    Timer timer(g_Stats.compileTime);

    module->backend->compile(buildParameters);
    g_Stats.numGenModules++;

    // Notify we are done
    if (mutexDone)
    {
        unique_lock lk(*mutexDone);
        condVar->notify_all();
    }

    // The module is built, so notify (we notify before the test)
    if (buildParameters.compileType == BackendCompileType::Normal)
        module->setHasBeenBuilt(BUILDRES_COMPILER);

    return JobResult::ReleaseJob;
}
