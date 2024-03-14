#pragma once

struct Job;

struct DependentJobs
{
    void add(Job* job);
    void clear();
    void setRunning();

    VectorNative<Job*> list;
};
