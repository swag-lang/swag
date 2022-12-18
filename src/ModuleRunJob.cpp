#include "pch.h"
#include "ModuleRunJob.h"
#include "Workspace.h"
#include "Module.h"
#include "Timer.h"
#include "Backend.h"

JobResult ModuleRunJob::execute()
{
    fs::path path = g_Workspace->targetPath.string() + buildParameters.outputFileName.c_str();
    path += Backend::getOutputFileExtension(g_CommandLine->target, BuildCfgBackendKind::Executable);
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

        Utf8 cmdLine = path.string();
        cmdLine += " ";
        cmdLine += g_CommandLine->userArguments;

        OS::doProcess(module, cmdLine, path.parent_path().parent_path().string(), numErrors);
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
