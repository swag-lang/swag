#include "pch.h"
#include "ModuleExportJob.h"
#include "Backend.h"

thread_local Pool<ModuleExportJob> g_Pool_moduleExportJob;

JobResult ModuleExportJob::execute()
{
	return backend->generateExportFile(this);
}
