#pragma once
#include "VectorNative.h"
#include "ScopedLock.h"
#include "Job.h"

struct JobGroup
{
    mutex              mutex;
    atomic<int>        runningJobs;
    VectorNative<Job*> jobs;

    void addJob(Job* job)
    {
        ScopedLock lk(mutex);
        jobs.push_back(job);
    }

    Job* pickJob()
    {
        if (!mutex.try_lock())
            return nullptr;
        if (jobs.empty())
        {
            mutex.unlock();
            return nullptr;
        }

        auto job = jobs.back();
        jobs.pop_back();
        job->jobGroup = this;
        runningJobs++;
        mutex.unlock();
        return job;
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
        jobs = move(grp.jobs);
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

    void complete(Job* parentJob)
    {
        ScopedLock lk(mutex);
        while (runningJobs.load() > 0)
            this_thread::yield();
        for (auto j : jobs)
        {
            j->dependentJob = parentJob;
            j->jobGroup     = nullptr;
            j->flags &= ~JOB_IS_OPT;
            parentJob->jobsToAdd.push_back(j);
        }

        jobs.clear();
    }
};