#include "pch.h"
#include "DependentJobs.h"
#include "Job.h"
#include "SpinLock.h"
#include "Assert.h"

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
