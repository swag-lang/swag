#include "pch.h"
#include "Module.h"
#include "BackendX64SaveObjJob.h"
#include "ModulePrepOutputJob.h"
#include "BackendX64.h"

thread_local Pool<BackendX64SaveObjJob> g_Pool_backendX64SaveObjJob;

JobResult BackendX64SaveObjJob::execute()
{
    ((BackendX64*) module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
