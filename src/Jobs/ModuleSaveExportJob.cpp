#include "pch.h"
#include "Backend/Backend.h"
#include "Jobs/ModuleSaveExportJob.h"
#include "Wmf/Module.h"

ModuleSaveExportJob::ModuleSaveExportJob()
{
    addFlag(JOB_IS_IO);
}

JobResult ModuleSaveExportJob::execute()
{
    module->backend->saveExportFile();
    return JobResult::ReleaseJob;
}
