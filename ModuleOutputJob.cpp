#include "pch.h"
#include "ModuleOutputJob.h"
#include "BackendC.h"
#include "Stats.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    BackendC backend(module);
    if (!backend.generate())
        return JobResult::ReleaseJob;
    g_Stats.numGenModules++;
    return JobResult::ReleaseJob;
}
