#include "pch.h"
#include "ModuleExportJob.h"
#include "Backend.h"

thread_local Pool<ModuleExportJob> g_Pool_moduleOutputExportJob;

JobResult ModuleExportJob::execute()
{
	backend->generateExportFile();
    return JobResult::ReleaseJob;
}
