#include "pch.h"
#include "Jobs/ModuleExportJob.h"
#include "Backend/Backend.h"

JobResult ModuleExportJob::execute()
{
    return backend->generateExportFile(this);
}
