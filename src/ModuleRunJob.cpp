#include "pch.h"
#include "ModuleRunJob.h"
#include "Backend.h"
#include "Module.h"
#include "Timer.h"
#include "Workspace.h"

JobResult ModuleRunJob::execute()
{
    Path path = g_Workspace->targetPath;
    path.append(buildParameters.outputFileName.c_str());
    path += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgOutputKind::Executable).c_str();
    error_code err;
    if (!exists(path, err))
        return JobResult::ReleaseJob;

#ifdef SWAG_STATS
    Timer timer(&g_Stats.runTestTime);
#endif

    Utf8 cmdLine = path.string();
    cmdLine += " ";
    cmdLine += g_CommandLine.userArguments;

    if (buildParameters.compileType == Test)
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, cmdLine, path.parent_path().parent_path().string(), numErrors);
        g_Workspace->numErrors += numErrors;
        module->numErrors += numErrors;
    }
    else
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, cmdLine, path.parent_path().parent_path().string(), numErrors);
    }

    return JobResult::ReleaseJob;
}
