#include "pch.h"
#include "ModuleRunJob.h"
#include "Workspace.h"
#include "Module.h"
#include "Timer.h"
#include "Backend.h"

JobResult ModuleRunJob::execute()
{
    fs::path path = g_Workspace->targetPath.string() + buildParameters.outputFileName.c_str();
    path += Backend::getOutputFileExtension(BuildCfgBackendKind::Executable);
    if (!fs::exists(path))
        return JobResult::ReleaseJob;

    // Timing...
    Timer timer(&g_Stats.runTestTime);

    if (buildParameters.compileType == BackendCompileType::Test)
        g_Log.messageHeaderCentered("Testing backend", module->name.c_str());
    else
        g_Log.messageHeaderCentered("Running backend", module->name.c_str());

    if (buildParameters.compileType == BackendCompileType::Test)
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, path.string(), path.parent_path().parent_path().string(), true, numErrors, LogColor::Default);
        g_Workspace->numErrors += numErrors;
        module->numErrors += numErrors;
    }
    else
    {
        auto cmdLine = path.string();
        cmdLine += " ";
        cmdLine += g_CommandLine->userArguments;
        OS::doRunProcess(cmdLine, path.parent_path().parent_path().string());
    }

    return JobResult::ReleaseJob;
}
