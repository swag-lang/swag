#pragma once
#include "SpinLock.h"
#include "Assert.h"
#include "Job.h"

struct DependentJobs
{
    vector<Job*> list;

    void add(Job* job)
    {
        SWAG_ASSERT(!(job->flags & JOB_IS_DEPENDENT));
        job->flags |= JOB_IS_DEPENDENT;
        job->flags &= ~JOB_IS_IN_THREAD;
        list.push_back(job);
    }

    void clear()
    {
        list.clear();
    }
};
