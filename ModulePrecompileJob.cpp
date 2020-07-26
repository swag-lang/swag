#include "pch.h"
#include "ModulePreCompileJob.h"
#include "Backend.h"
#include "Workspace.h"

thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;

JobResult ModulePreCompileJob::execute()
{
    auto timeBefore = chrono::high_resolution_clock::now();

    auto result = module->backend->preCompile(buildParameters, this);

    auto                     timeAfter = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed   = timeAfter - timeBefore;
    g_Stats.precompileTime = g_Stats.precompileTime + elapsed.count();

    return result;
}
