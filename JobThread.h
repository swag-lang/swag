#pragma once
struct Job;

struct JobThread
{
    JobThread();
    ~JobThread();

    void loop();
    void notifyJob();
    void waitForANewJob();

    thread*            thread     = nullptr;
    bool               requestEnd = false;
    mutex              mutexNotify;
    condition_variable condVar;
};
