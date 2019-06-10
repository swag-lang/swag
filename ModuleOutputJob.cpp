#include "pch.h"
#include "ModuleOutputJob.h"
#include "BackendC.h"

Pool<ModuleOutputJob> g_Pool_moduleOutputJob;

JobResult ModuleOutputJob::execute()
{
    BackendC backend(module);
    backend.generate();
    return JobResult::ReleaseJob;
}
