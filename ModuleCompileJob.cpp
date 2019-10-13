#include "pch.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Stats.h"
#include "Module.h"

Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    module->backend->compile(buildParameters);
    g_Stats.numGenModules++;

    if (mutexDone)
    {
        std::unique_lock<std::mutex> lk(*mutexDone);
        condVar->notify_all();
    }

    return JobResult::ReleaseJob;
}
