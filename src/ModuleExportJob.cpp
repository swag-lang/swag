#include "pch.h"
#include "ModuleExportJob.h"
#include "Backend.h"

JobResult ModuleExportJob::execute()
{
	return backend->generateExportFile(this);
}
