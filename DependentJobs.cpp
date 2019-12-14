#include "pch.h"
#include "DependentJobs.h"
#include "Job.h"
#include "Assert.h"
#include "ThreadManager.h"

void DependentJobs::add(Job* job)
{
    SWAG_ASSERT(!(job->flags & JOB_IS_DEPENDENT));
    job->flags |= JOB_IS_DEPENDENT;
    job->flags &= ~JOB_IS_IN_THREAD;
    list.push_back(job);
}

void DependentJobs::clear()
{
    list.clear();
}

void DependentJobs::setRunning()
{
    for (auto job : list)
        g_ThreadMgr.addJob(job);
    list.clear();
}
