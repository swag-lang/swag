#include "pch.h"
#include "Module.h"
#include "ModuleSaveExportJob.h"
#include "OS.h"
#include "Module.h"
#include "Backend.h"

thread_local Pool<ModuleSaveExportJob> g_Pool_moduleSaveExportJob;

JobResult ModuleSaveExportJob::execute()
{
    module->backend->saveExportFile();
    return JobResult::ReleaseJob;
}
