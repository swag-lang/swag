#pragma once
#include "Job.h"

struct JobThread;
struct IoThread;
enum class JobResult;

struct JobQueue
{
    VectorNative<Job*>         jobs;
    Vector<VectorNative<Job*>> affinity;
    uint32_t                   affinityCount = 0;
};

struct ThreadManager
{
    void init();
    void addJob(Job* job);
    void addJobNoLock(Job* job);
    Job* getJob();
    Job* getJob(JobQueue& queue);
    void eatJob(Job* job);
    Job* getJob(JobThread* thread);
    bool doneWithJobs() const;
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
    SharedMutex              mutexAdd;
    condition_variable       condVar;
    mutex                    mutexDone;
    condition_variable       condVarDone;
    atomic<int>              jobsInThreads    = 0;
    atomic<int>              jobsOptInThreads = 0;
    atomic<int>              addJobCount      = 0;
    atomic<int>              currentJobsIO    = 0;
    uint32_t                 numWorkers       = 0;
    bool                     debuggerMode     = false;
};

extern ThreadManager g_ThreadMgr;
