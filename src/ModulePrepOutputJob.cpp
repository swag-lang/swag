#include "pch.h"
#include "ModulePrepOutputJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"
#include "Module.h"
#include "Stats.h"
#include "ErrorIds.h"
#include "Diagnostic.h"

JobResult ModulePrepOutputJob::execute()
{
    Timer timer{&g_Stats.prepOutputTimeJob};

    // Be sure we have a #main in case of an executable
    if (module->backend->mustCompile &&
        module->buildCfg.backendKind == BuildCfgBackendKind::Executable &&
        buildParameters.compileType != BackendCompileType::Test)
    {
        if (!module->mainIsDefined)
        {
            module->error(Err(Err0269));
            return JobResult::ReleaseJob;
        }
    }

    return module->backend->prepareOutput(buildParameters, this);
}
