#include "pch.h"
#include "Module.h"
#include "BackendX64GenObjJob.h"
#include "ModulePrepOutputJob.h"
#include "BackendX64.h"

thread_local Pool<BackendX64GenObjJob> g_Pool_backendX64GenObjJob;

JobResult BackendX64GenObjJob::execute()
{
    ((BackendX64*) module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
