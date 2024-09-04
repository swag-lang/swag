#include "pch.h"
#include "Jobs/ModulePrepOutputJob.h"
#include "Backend/Backend.h"
#include "Main/Statistics.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#endif

JobResult ModulePrepOutputStage1Job::execute()
{
#ifdef SWAG_STATS
    Timer timer{&g_Stats.prepOutputStage1TimeJob};
#endif

    // Be sure we have a #main in case of an executable
    if (module->backend->mustCompile &&
        module->buildCfg.backendKind == BuildCfgBackendKind::Executable &&
        buildParameters.compileType != Test)
    {
        if (!module->mainIsDefined)
        {
            Report::error(module, toErr(Err0747));
            return JobResult::ReleaseJob;
        }
    }

    return module->backend->prepareOutput(buildParameters, 1, this);
}

JobResult ModulePrepOutputStage2Job::execute()
{
#ifdef SWAG_STATS
    Timer timer{&g_Stats.prepOutputStage2TimeJob};
#endif

    return module->backend->prepareOutput(buildParameters, 2, this);
}
