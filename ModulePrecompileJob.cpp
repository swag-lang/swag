#include "pch.h"
#include "ModulePreCompileJob.h"
#include "BackendC.h"
#include "Workspace.h"

Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;

JobResult ModulePreCompileJob::execute()
{
	return module->backend->preCompile(this);
}
