#include "pch.h"
#include "Backend/Backend.h"
#include "Core/Timer.h"
#include "Jobs/ModuleRunJob.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

JobResult ModuleRunJob::execute()
{
    Path path = g_Workspace->targetPath;
    path.append(buildParameters.module->name);
    path += Backend::getOutputFileExtension(g_CommandLine.target, BuildCfgOutputKind::Executable);
    std::error_code err;
    if (!std::filesystem::exists(path, err))
        return JobResult::ReleaseJob;

#ifdef SWAG_STATS
    Timer timer(&g_Stats.runTestTime);
#endif

    Utf8 cmdLine = path;
    cmdLine += " ";
    cmdLine += g_CommandLine.userArguments;

    if (buildParameters.compileType == Test)
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, cmdLine, path.parent_path().parent_path(), numErrors);
        g_Workspace->numErrors += numErrors;
        module->numErrors += numErrors;
    }
    else
    {
        uint32_t numErrors = 0;
        OS::doProcess(module, cmdLine, path.parent_path().parent_path(), numErrors);
    }

    return JobResult::ReleaseJob;
}
