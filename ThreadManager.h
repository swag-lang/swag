#pragma once
struct Job;
struct JobThread;
struct LoadingThread;
struct SavingThread;

struct ThreadManager
{
    ~ThreadManager();

    void init();
	void addJobs(const vector<Job*>& jobs);
    void addJob(Job* job);
	void addJobNoLock(Job* job);
    Job* getJob(JobThread* thread);
    bool doneWithJobs();
	void executeOneJob(Job* job);
    void jobHasEnded(Job* job);
    void waitEndJobs();
    void addPendingJob(Job* job);

    Job* getJob();

    LoadingThread*     loadingThread = nullptr;
    SavingThread*      savingThread  = nullptr;
    vector<Job*>       queueJobs;
    vector<JobThread*> availableThreads;
    vector<JobThread*> workerThreads;
    mutex              mutexAdd;
    condition_variable condVar;
    mutex              mutexDone;
    condition_variable condVarDone;
    vector<Job*>       pendingJobs;
    atomic<int>        processingJobs = 0;
};

extern ThreadManager g_ThreadMgr;