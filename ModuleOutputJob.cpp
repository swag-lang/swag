#include "pch.h"
#include "ModuleOutputJob.h"
#include "BackendC.h"
#include "Module.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    module->backend = new BackendC(module);
    module->backend->generate();
    return JobResult::ReleaseJob;
}
