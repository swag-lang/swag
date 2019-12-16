#include "pch.h"
#include "ModuleTestJob.h"
#include "Workspace.h"
#include "Os.h"

thread_local Pool<ModuleTestJob> g_Pool_moduleTestJob;

JobResult ModuleTestJob::execute()
{
    SWAG_ASSERT(module->hasBeenBuilt & BUILDRES_COMPILER);
    fs::path path = buildParameters.destFile + buildParameters.postFix;
    path += OS::getOutputFileExtension(buildParameters.type);
    SWAG_ASSERT(fs::exists(path));

    g_Log.messageHeaderCentered("Testing backend", module->name.c_str(), LogColor::DarkMagenta);
    if (g_CommandLine.verbose && g_CommandLine.verboseBackendCommand)
        g_Log.verbose("   running " + path.string());

    uint32_t numErrors = 0;
    OS::doProcess(path.string(), path.parent_path().parent_path().string(), true, numErrors, LogColor::Default);
    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;

    return JobResult::ReleaseJob;
}
