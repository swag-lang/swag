#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "Global.h"

void Job::doneJob()
{
    scoped_lock lk(executeMutex);
    for (auto p : dependentJobs.list)
        g_ThreadMgr.addJob(p);
    dependentJobs.clear();
}

void Job::addDependentJob(Job* job)
{
    scoped_lock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::setPending()
{
    SWAG_ASSERT(baseContext);
    baseContext->result = ContextResult::Pending;
}