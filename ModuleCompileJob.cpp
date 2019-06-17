#include "pch.h"
#include "ModuleCompileJob.h"
#include "BackendC.h"
#include "Stats.h"
#include "Module.h"

Pool<ModuleCompileJob> g_Pool_moduleCompileJob;

JobResult ModuleCompileJob::execute()
{
    module->backend->compile();
    g_Stats.numGenModules++;
    return JobResult::ReleaseJob;
}
