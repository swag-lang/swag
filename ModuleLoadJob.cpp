#include "pch.h"
#include "ModuleLoadJob.h"
#include "ByteCodeModuleManager.h"
#include "Module.h"
#include "Os.h"

thread_local Pool<ModuleLoadJob> g_Pool_moduleLoadJob;

JobResult ModuleLoadJob::execute()
{
    if (!g_ModuleMgr.loadModule(module->name))
    {
        module->error(format("fail to load module '%s' => %s", module->name.c_str(), OS::getLastErrorAsString().c_str()));
        return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
