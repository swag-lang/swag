#include "pch.h"
#include "ModuleExportJob.h"
#include "Backend/Backend.h"

JobResult ModuleExportJob::execute()
{
    return backend->generateExportFile(this);
}
