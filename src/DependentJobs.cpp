#include "pch.h"
#include "DependentJobs.h"
#include "ThreadManager.h"

void DependentJobs::add(Job* job)
{
    // Add it once !
#ifdef SWAG_DEV_MODE
    for (auto j : list)
        SWAG_ASSERT(j != job);
#endif

    list.push_back(job);
}

void DependentJobs::clear()
{
    list.clear();
}

void DependentJobs::setRunning()
{
    for (auto job : list)
        g_ThreadMgr.addJob(job);
    list.clear();
}
