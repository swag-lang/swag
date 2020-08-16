#include "pch.h"
#include "ModulePrepOutputJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"
#include "Module.h"

thread_local Pool<ModulePrepOutputJob> g_Pool_modulePrepOutputJob;

JobResult ModulePrepOutputJob::execute()
{
    return module->backend->prepareOutput(buildParameters, this);
}
