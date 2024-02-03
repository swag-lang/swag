#include "pch.h"
#include "ModuleSaveExportJob.h"
#include "Backend.h"
#include "Module.h"

JobResult ModuleSaveExportJob::execute()
{
    module->backend->saveExportFile();
    return JobResult::ReleaseJob;
}
