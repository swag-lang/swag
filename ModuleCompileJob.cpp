#include "pch.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Stats.h"
#include "Module.h"
#include "Workspace.h"
#include "Global.h"
#include "Os.h"

Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    // Generate output file
    module->backend->compile(buildParameters);
    g_Stats.numGenModules++;

    // Notify we are done
    if (mutexDone)
    {
        std::unique_lock<std::mutex> lk(*mutexDone);
        condVar->notify_all();
    }

    // Test
    if (buildParameters.flags & BUILDPARAM_FOR_TEST)
    {
        fs::path path = buildParameters.destFile + ".test.exe";
        if (fs::exists(path))
        {
            g_Log.messageHeaderCentered("Testing backend", module->name.c_str());
            uint32_t numErrors = 0;
            OS::doProcess(path.string(), path.parent_path().string(), true, numErrors);
            g_Workspace.numErrors += numErrors;
            module->numErrors += numErrors;
        }
    }

    return JobResult::ReleaseJob;
}
