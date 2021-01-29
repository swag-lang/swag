#include "pch.h"
#include "ModuleRunJob.h"
#include "Workspace.h"
#include "Os.h"
#include "Module.h"
#include "Timer.h"
#include "Profile.h"
#include "Backend.h"

thread_local Pool<ModuleRunJob> g_Pool_moduleRunJob;

JobResult ModuleRunJob::execute()
{
    SWAG_PROFILE(PRF_RUN, format("run %s", module->name.c_str()));

    fs::path path = g_Workspace.targetPath.string() + buildParameters.outputFileName;
    path += Backend::getOutputFileExtension(BackendOutputType::Binary);
    if (!fs::exists(path))
        return JobResult::ReleaseJob;

    // Timing...
    Timer timer(&g_Stats.runTestTime);
    timer.start();

    if (buildParameters.compileType == BackendCompileType::Test)
        g_Log.messageHeaderCentered("Testing backend", module->name.c_str());
    else
        g_Log.messageHeaderCentered("Running backend", module->name.c_str());
    if (g_CommandLine.verboseBackendCommand)
        g_Log.verbosePass(LogPassType::Info, "Test", path.string());

    if (buildParameters.compileType == BackendCompileType::Test)
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, path.string(), path.parent_path().parent_path().string(), true, numErrors, LogColor::Default);
        g_Workspace.numErrors += numErrors;
        module->numErrors += numErrors;
    }
    else
    {
        auto cmdLine = path.string();
        cmdLine += " ";
        cmdLine += g_CommandLine.userArguments;
        OS::doRunProcess(cmdLine, path.parent_path().parent_path().string());
    }

    timer.stop();
    return JobResult::ReleaseJob;
}
