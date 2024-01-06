#pragma once
#include "Mutex.h"
#include "Job.h"

struct JobGroup
{
    Mutex              mutex;
    atomic<int>        runningJobs;
    VectorNative<Job*> jobs;

    Job* pickJob();
    void complete(Job* parentJob);

    void addJob(Job* job)
    {
        ScopedLock lk(mutex);
        jobs.push_back(job);
    }

    void doneJob()
    {
        runningJobs--;
    }

    void moveFrom(JobGroup& grp)
    {
        ScopedLock lk(mutex);
        while (runningJobs.load() > 0)
            this_thread::yield();
        jobs = std::move(grp.jobs);
    }

    void waitAndClear()
    {
        ScopedLock lk(mutex);
        while (runningJobs.load() > 0)
            this_thread::yield();
        for (auto j : jobs)
            j->release();
        jobs.clear();
    }
};