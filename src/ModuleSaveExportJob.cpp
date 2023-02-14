#include "pch.h"
#include "Module.h"
#include "ModuleSaveExportJob.h"
#include "Backend.h"

JobResult ModuleSaveExportJob::execute()
{
    module->backend->saveExportFile();
    return JobResult::ReleaseJob;
}
