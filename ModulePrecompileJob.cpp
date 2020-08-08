#include "pch.h"
#include "ModulePreCompileJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"
#include "Module.h"

thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;

JobResult ModulePreCompileJob::execute()
{
    Timer timer(g_Stats.precompileTimeJob);
    return module->backend->preCompile(buildParameters, this);
}
