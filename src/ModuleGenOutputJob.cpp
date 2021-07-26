#include "pch.h"
#include "ModuleGenOutputJob.h"
#include "Backend.h"
#include "Module.h"
#include "Timer.h"
#include "Stats.h"

JobResult ModuleGenOutputJob::execute()
{
    Timer timer(&g_Stats.genOutputTimeJob);
    timer.start();

    module->backend->generateOutput(buildParameters);
    g_Stats.numGenModules++;

    // Notify we are done
    if (mutexDone)
    {
        unique_lock lk(*mutexDone);
        condVar->notify_all();
    }

    // The module is built, so notify (we notify before the test)
    if (buildParameters.compileType != BackendCompileType::Test)
        module->setHasBeenBuilt(BUILDRES_COMPILER);

    timer.stop();
    return JobResult::ReleaseJob;
}
