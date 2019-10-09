#pragma once
#include "SpinLock.h"
#include "Assert.h"
struct Job;

struct DependentJobs
{
    vector<Job*> list;

    void add(Job* job)
    {
#ifdef SWAG_HAS_ASSERT
        for (auto here : list)
            SWAG_ASSERT(here != job);
#endif
        list.push_back(job);
    }

    void clear()
    {
        list.clear();
    }
};
