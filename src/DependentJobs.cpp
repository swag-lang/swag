#include "pch.h"
#include "DependentJobs.h"
#include "ThreadManager.h"
#include "CommandLine.h"

void DependentJobs::add(Job* job)
{
    // Add it once !
    if (g_CommandLine.devMode)
    {
        for (auto j : list)
        {
            SWAG_ASSERT(j != job);
        }
    }

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
