#include "pch.h"
#include "ModuleOutputExportJob.h"
#include "BackendC.h"

Pool<ModuleOutputExportJob> g_Pool_moduleOutputExportJob;

JobResult ModuleOutputExportJob::execute()
{
	backend->generateExportFile();
    return JobResult::ReleaseJob;
}
