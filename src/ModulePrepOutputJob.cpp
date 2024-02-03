#include "pch.h"
#include "ModulePrepOutputJob.h"
#include "Backend.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Report.h"
#include "Statistics.h"
#include "Timer.h"
#include "Workspace.h"

JobResult ModulePrepOutputStage1Job::execute()
{
#ifdef SWAG_STATS
    Timer timer{&g_Stats.prepOutputStage1TimeJob};
#endif

    // Be sure we have a #main in case of an executable
    if (module->backend->mustCompile &&
        module->buildCfg.backendKind == BuildCfgBackendKind::Executable &&
        buildParameters.compileType != BackendCompileType::Test)
    {
        if (!module->mainIsDefined)
        {
            Report::error(module, Err(Err0523));
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
