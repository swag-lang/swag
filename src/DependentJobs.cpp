#include "pch.h"
#include "DependentJobs.h"
#include "ThreadManager.h"
#include "CommandLine.h"

void DependentJobs::add(Job* job)
{
    SWAG_RACE_CONDITION_WRITE(raceCondition);

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
    SWAG_RACE_CONDITION_WRITE(raceCondition);
    list.clear();
}

void DependentJobs::setRunning()
{
    SWAG_RACE_CONDITION_WRITE(raceCondition);
    for (auto job : list)
        g_ThreadMgr.addJob(job);
    list.clear();
}
