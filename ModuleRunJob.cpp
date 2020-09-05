#include "pch.h"
#include "ModuleRunJob.h"
#include "Workspace.h"
#include "Os.h"
#include "DiagnosticInfos.h"
#include "Module.h"
#include "Timer.h"
#include "Profile.h"

thread_local Pool<ModuleRunJob> g_Pool_moduleRunJob;

JobResult ModuleRunJob::execute()
{
    SWAG_PROFILE(PRF_RUN, format("run %s", module->name.c_str()));

    fs::path path = g_Workspace.targetPath.string() + buildParameters.outputFileName;
    path += OS::getOutputFileExtension(BackendOutputType::Binary);
    if (!fs::exists(path))
        return JobResult::ReleaseJob;

    // Timing...
    Timer timer(g_Stats.runTestTime);
    timer.start();

    if(buildParameters.compileType == BackendCompileType::Test)
        g_Log.messageHeaderCentered("Testing backend", module->name.c_str());
    else
        g_Log.messageHeaderCentered("Running backend", module->name.c_str());
    if (g_CommandLine.verbose && g_CommandLine.verboseBackendCommand)
        g_Log.verbosePass(LogPassType::Info, "Test", path.string());

    uint32_t numErrors = 0;
    OS::doProcess(path.string(), path.parent_path().parent_path().string(), true, numErrors, LogColor::Default);
    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;

    timer.stop();
    return JobResult::ReleaseJob;
}
