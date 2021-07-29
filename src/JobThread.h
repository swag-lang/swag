#pragma once
struct Job;

struct JobThread
{
    JobThread(int rank);
    ~JobThread();

    void loop();
    void notifyJob();
    void waitForANewJob();

    mutex              mutexNotify;
    condition_variable condVar;

    thread* thread = nullptr;

    bool requestEnd = false;
};
