#pragma once
struct Job;

struct JobThread
{
    JobThread();
    ~JobThread();

	bool executeJob(Job* job, bool& exception);
    void loop();
    void notifyJob();
    void waitJob();

    thread*            thread     = nullptr;
    bool               requestEnd = false;
    mutex              mutexNotify;
    condition_variable condVar;
};
