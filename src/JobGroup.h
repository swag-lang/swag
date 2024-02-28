#pragma once
#include "Job.h"
#include "Mutex.h"

struct JobGroup
{
    Job* pickJob();
    void complete(Job* parentJob);
    void addJob(Job* job);
    void doneJob();
    void moveFrom(JobGroup& grp);
    void waitAndClear();

    Mutex                 mutex;
    std::atomic<uint32_t> runningJobs = 0;
    VectorNative<Job*>    jobs;
};
