#pragma once
struct Job;
struct JobThread;
struct LoadingThread;

struct ThreadManager
{
    ~ThreadManager();

    void init();
    void addJob(Job* job);
    Job* getJob(JobThread* thread);
	bool doneWithJobs();
    void jobHasEnded();
    void waitEndJobs();

    Job* getJob();

    LoadingThread*     loadingThread = nullptr;
    vector<Job*>       queueJobs;
    vector<JobThread*> availableThreads;
    vector<JobThread*> workerThreads;
    mutex              mutexAdd;
    condition_variable condVar;
    mutex              mutexDone;
    condition_variable condVarDone;
    atomic<int>        pendingJobs;
};
