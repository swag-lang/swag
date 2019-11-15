#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "Global.h"
#include "SymTable.h"

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

void Job::waitForSymbolNoLock(SymbolName* symbol)
{
    waitingSymbolSolved = symbol;
    setPending();
    symbol->dependentJobs.add(this);
}

void Job::setPending()
{
    SWAG_ASSERT(baseContext);
    baseContext->result = ContextResult::Pending;
	g_ThreadMgr.addPendingJob(this);
}