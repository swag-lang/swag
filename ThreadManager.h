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
    bool doneWithJobs();
    void executeOneJob(Job* job, int& exceptionCode);
    void jobHasEnded(Job* job, JobResult result);
    void waitEndJobs();
    void participate(mutex& lock, uint32_t affinity, function<bool(Job*)> canGetJob = nullptr);
    void participate(function<bool(Job*)> canGetJob = nullptr);

    Job* getJob(uint32_t affinity = AFFINITY_ALL, function<bool(Job*)> canGetJob = nullptr);

    vector<Job*>       queueJobs;
    vector<JobThread*> availableThreads;
    vector<JobThread*> workerThreads;
    mutex              mutexAdd;
    condition_variable condVar;
    mutex              mutexDone;
    condition_variable condVarDone;
    vector<Job*>       waitingJobs;
    atomic<int>        jobsInThreads = 0;
};

extern ThreadManager g_ThreadMgr;