#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"

void Job::doneJob()
{
    scoped_lock lk(executeMutex);
    for (auto p : dependentJobs.list)
        g_ThreadMgr.addJob(p);
	dependentJobs.clear();
}

void Job::addDependentJob(Job* job)
{
	scoped_lock lk(executeMutex);
	dependentJobs.add(job);
}