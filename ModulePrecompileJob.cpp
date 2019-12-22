#include "pch.h"
#include "ModulePreCompileJob.h"
#include "BackendC.h"
#include "Workspace.h"

thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;

JobResult ModulePreCompileJob::execute()
{
	return module->backend->preCompile(this);
}
