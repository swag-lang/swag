#pragma once
#include "Job.h"

struct JobThread;
struct IoThread;
enum class JobResult;

struct ThreadManager
{
    void init();
    void addJob(Job* job);
    void addJobNoLock(Job* job);
    Job* getJob(JobThread* thread);
    Job* getJob(uint32_t affinity, VectorNative<Job*>& queue);
    bool doneWithJobs();
    void executeOneJob(Job* job);
    void jobHasEnded(Job* job, JobResult result);
    void waitEndJobs();
    void participate(mutex& lock, uint32_t affinity);
    bool participate(uint32_t affinity);

    Job* getJob(uint32_t affinity = AFFINITY_ALL);

    VectorNative<Job*>       queueJobsIO;
    VectorNative<Job*>       queueJobs;
    VectorNative<JobThread*> availableThreads;
    VectorNative<JobThread*> workerThreads;
    VectorNative<Job*>       waitingJobs;
    mutex                    mutexAdd;
    condition_variable       condVar;
    mutex                    mutexDone;
    condition_variable       condVarDone;
    atomic<int>              jobsInThreads = 0;
    int                      currentJobsIO = 0;
};

extern ThreadManager g_ThreadMgr;