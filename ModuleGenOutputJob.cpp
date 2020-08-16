#include "pch.h"
#include "ModuleGenOutputJob.h"
#include "Backend.h"
#include "Module.h"
#include "Timer.h"

thread_local Pool<ModuleGenOutputJob> g_Pool_moduleGenOutputJob;

JobResult ModuleGenOutputJob::execute()
{
    Timer timer(g_Stats.genOutputTime);

    module->backend->generateOutput(buildParameters);
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
