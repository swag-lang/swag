#include "pch.h"
#include "ModulePreCompileJob.h"
#include "ModuleTestJob.h"
#include "BackendC.h"
#include "Stats.h"
#include "Workspace.h"
#include "Os.h"
#include "ThreadManager.h"

thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;

JobResult ModulePreCompileJob::execute()
{
    if (!module->backend->preCompile())
        return JobResult::ReleaseJob;
    return JobResult::ReleaseJob;
}
