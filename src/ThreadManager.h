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
    Job* getJob();
    Job* getJobNoLock();
    Job* getJob(JobThread* thread);
    Job* getJob(VectorNative<Job*>& queue);
    bool doneWithJobs();
    void clearOptionalJobs();
    void executeOneJob(Job* job);
    void jobHasEnded(Job* job, JobResult result);
    void waitEndJobs();
    bool tryExecuteJob();

    VectorNative<Job*>       queueJobsIO;
    VectorNative<Job*>       queueJobs;
    VectorNative<Job*>       queueJobsOpt;
    VectorNative<JobThread*> availableThreads;
    VectorNative<JobThread*> workerThreads;
    VectorNative<Job*>       waitingJobs;
    mutex                    mutexAdd;
    condition_variable       condVar;
    mutex                    mutexDone;
    condition_variable       condVarDone;
    atomic<int>              jobsInThreads    = 0;
    atomic<int>              jobsOptInThreads = 0;
    int                      currentJobsIO    = 0;
};

extern ThreadManager g_ThreadMgr;