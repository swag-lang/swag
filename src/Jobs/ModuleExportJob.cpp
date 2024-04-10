#include "pch.h"
#include "Backend/Backend.h"
#include "Jobs/ModuleExportJob.h"

JobResult ModuleExportJob::execute()
{
    return backend->generateExportFile(this);
}
