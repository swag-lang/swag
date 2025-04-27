#include "pch.h"
#include "Jobs/DependentJobs.h"
#include "Threading/ThreadManager.h"

void DependentJobs::add(Job* job)
{
    // Add it once!
#ifdef SWAG_DEV_MODE
    for (const auto j : list)
        SWAG_ASSERT(j != job);
#endif

    list.push_back(job);
}

void DependentJobs::addOnce(Job* job)
{
    for (const auto j : list)
    {
        if (j == job)
            return;
    }

    list.push_back(job);
}

void DependentJobs::clear()
{
    list.clear();
}

void DependentJobs::setRunning()
{
    for (const auto job : list)
        g_ThreadMgr.addJob(job);
    list.clear();
}
