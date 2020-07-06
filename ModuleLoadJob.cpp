#include "pch.h"
#include "ModuleLoadJob.h"
#include "ModuleManager.h"
#include "Module.h"
#include "Os.h"

thread_local Pool<ModuleLoadJob> g_Pool_moduleLoadJob;

JobResult ModuleLoadJob::execute()
{
    if (!g_ModuleMgr.loadModule(module->name, false, true))
    {
        module->error(format("failed to load module '%s' => %s", module->name.c_str(), OS::getLastErrorAsString().c_str()));
        return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
