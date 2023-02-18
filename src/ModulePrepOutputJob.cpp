#include "pch.h"
#include "ModulePrepOutputJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"
#include "Module.h"
#include "Stats.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"

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
            Report::error(module, Err(Err0269));
            return JobResult::ReleaseJob;
        }
    }

    return module->backend->prepareOutput(1, buildParameters, this);
}

JobResult ModulePrepOutputStage2Job::execute()
{
#ifdef SWAG_STATS
    Timer timer{&g_Stats.prepOutputStage2TimeJob};
#endif

    // Be sure we have a #main in case of an executable
    if (module->backend->mustCompile &&
        module->buildCfg.backendKind == BuildCfgBackendKind::Executable &&
        buildParameters.compileType != BackendCompileType::Test)
    {
        if (!module->mainIsDefined)
        {
            Report::error(module, Err(Err0269));
            return JobResult::ReleaseJob;
        }
    }

    return module->backend->prepareOutput(2, buildParameters, this);
}
