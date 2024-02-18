#include "pch.h"
#include "JobGroup.h"

Job* JobGroup::pickJob()
{
    if (!mutex.try_lock())
        return nullptr;
    if (jobs.empty())
    {
        mutex.unlock();
        return nullptr;
    }

    const auto job = jobs.back();
    jobs.pop_back();
    job->jobGroup = this;
    ++runningJobs;
    mutex.unlock();
    return job;
}

void JobGroup::waitAndClear()
{
    ScopedLock lk(mutex);
    while (runningJobs.load() > 0)
        this_thread::yield();
    for (const auto j : jobs)
        j->release();
    jobs.clear();
}

void JobGroup::complete(Job* parentJob)
{
    ScopedLock lk(mutex);
    while (runningJobs.load() > 0)
        this_thread::yield();
    for (auto j : jobs)
    {
        j->dependentJob = parentJob;
        j->jobGroup     = nullptr;
        j->removeFlag(JOB_IS_OPT);
        parentJob->jobsToAdd.push_back(j);
    }

    jobs.clear();
}

void JobGroup::addJob(Job* job)
{
    ScopedLock lk(mutex);
    jobs.push_back(job);
}

void JobGroup::doneJob()
{
    --runningJobs;
}

void JobGroup::moveFrom(JobGroup& grp)
{
    ScopedLock lk(mutex);
    while (runningJobs.load() > 0)
        this_thread::yield();
    jobs = std::move(grp.jobs);
}
