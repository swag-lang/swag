#pragma once
struct Job;

struct JobThread
{
    JobThread(int rank);
    ~JobThread();

    void loop();
    void notifyJob();

    mutex              mutexNotify;
    condition_variable condVar;

    thread* thread = nullptr;

    bool requestEnd = false;
};
