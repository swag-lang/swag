#include "pch.h"
#include "ModuleRunJob.h"
#include "Workspace.h"
#include "Os.h"
#include "DiagnosticInfos.h"

thread_local Pool<ModuleRunJob> g_Pool_moduleRunJob;

JobResult ModuleRunJob::execute()
{
    SWAG_ASSERT(module->hasBeenBuilt & BUILDRES_COMPILER);
    fs::path path = g_Workspace.targetPath.string() + buildParameters.outputFileName + buildParameters.postFix;
    path += OS::getOutputFileExtension(BackendOutputType::Binary);
    if (!fs::exists(path))
        return JobResult::ReleaseJob;

#ifdef SWAG_HAS_ASSERT
    PushDiagnosticInfos di;
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.last().message = "ModuleTestJob - " + path.string();
    }
#endif

    if(buildParameters.compileType == BackendCompileType::Test)
        g_Log.messageHeaderCentered("Testing backend", module->name.c_str());
    else
        g_Log.messageHeaderCentered("Running backend", module->name.c_str());
    if (g_CommandLine.verbose && g_CommandLine.verboseBackendCommand)
        g_Log.verbose("   running " + path.string());

    uint32_t numErrors = 0;
    OS::doProcess(path.string(), path.parent_path().parent_path().string(), true, numErrors, LogColor::Default);
    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;

    return JobResult::ReleaseJob;
}
