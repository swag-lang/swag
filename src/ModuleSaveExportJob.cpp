#include "pch.h"
#include "ModuleSaveExportJob.h"
#include "Backend.h"
#include "Module.h"

ModuleSaveExportJob::ModuleSaveExportJob()
{
    addFlag(JOB_IS_IO);
}

JobResult ModuleSaveExportJob::execute()
{
    module->backend->saveExportFile();
    return JobResult::ReleaseJob;
}
