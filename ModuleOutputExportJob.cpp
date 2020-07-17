#include "pch.h"
#include "ModuleOutputExportJob.h"
#include "Backend.h"

thread_local Pool<ModuleOutputExportJob> g_Pool_moduleOutputExportJob;

JobResult ModuleOutputExportJob::execute()
{
	backend->generateExportFile();
    return JobResult::ReleaseJob;
}
