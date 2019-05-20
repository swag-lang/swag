#pragma once

struct Job;
struct JobThread;

struct ThreadManager
{
    ~ThreadManager();

    void init();
    void addJob(Job* job);
    Job* getJob(JobThread* thread);
    void jobHasEnded();
    void waitEndJobs();

    Job* getJob();

    struct LoadingThread* m_loadingThread = nullptr;

    vector<Job*>       m_jobQueue;
    vector<JobThread*> m_availableThreads;
    vector<JobThread*> m_workers;
    mutex              m_mutexAdd;
    condition_variable m_Cv;
    mutex              m_mutexDone;
    condition_variable m_CvDone;
    atomic<int>        m_pendingJobs;
};
