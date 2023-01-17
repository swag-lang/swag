#pragma once
#include "Job.h"

struct JobThread;
struct IoThread;
enum class JobResult;

struct JobQueue
{
    VectorNative<Job*>         jobs;
    vector<VectorNative<Job*>> affinity;
    uint32_t                   affinityCount = 0;
};

struct ThreadManager
{
    void init();
    void addJob(Job* job);
    void addJobNoLock(Job* job);
    Job* getJob();
    Job* getJobNoLock();
    Job* getJobNoLock(JobQueue& queue);
    void eatJob(Job* job);
    Job* getJob(JobThread* thread);
    bool doneWithJobs();
    void clearOptionalJobs();
    void executeOneJob(Job* job);
    void jobHasEnded(Job* job, JobResult result);
    void waitEndJobsSync();
    void waitEndJobs();
    bool tryExecuteJob();

    JobQueue queueJobsIO;
    JobQueue queueJobs;
    JobQueue queueJobsOpt;

    VectorNative<JobThread*> availableThreads;
    VectorNative<JobThread*> workerThreads;
    VectorNative<Job*>       waitingJobs;
    Mutex                    mutexAdd;
    condition_variable       condVar;
    mutex                    mutexDone;
    condition_variable       condVarDone;
    atomic<int>              jobsInThreads    = 0;
    atomic<int>              jobsOptInThreads = 0;
    atomic<int>              addJobCount      = 0;
    int                      currentJobsIO    = 0;
};

extern ThreadManager g_ThreadMgr;