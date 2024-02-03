#include "pch.h"
#include "ModuleGenOutputJob.h"
#include "Backend.h"
#include "Module.h"
#include "Statistics.h"
#include "Timer.h"

JobResult ModuleGenOutputJob::execute()
{
#ifdef SWAG_STATS
    Timer timer(&g_Stats.genOutputTimeJob);
#endif

    module->backend->generateOutput(buildParameters);

#ifdef SWAG_STATS
    g_Stats.numGenModules++;
#endif

    // Notify we are done
    if (mutexDone)
    {
        ScopedLock lk(*mutexDone);
        condVar->notify_all();
    }

    // The module is built, so notify (we notify before the test)
    if (buildParameters.compileType != BackendCompileType::Test)
        module->setHasBeenBuilt(BUILDRES_COMPILER);

    return JobResult::ReleaseJob;
}
