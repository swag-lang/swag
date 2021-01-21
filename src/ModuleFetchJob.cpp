#include "pch.h"
#include "Module.h"
#include "ModuleFetchJob.h"
#include "OS.h"

thread_local Pool<ModuleFetchJob> g_Pool_moduleFetchJob;

JobResult ModuleFetchJob::execute()
{
    return JobResult::ReleaseJob;
}