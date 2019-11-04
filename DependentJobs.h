#pragma once
struct Job;

struct DependentJobs
{
    void add(Job* job);
    void clear();

    vector<Job*> list;
};
