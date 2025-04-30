#include "pch.h"
#include "Threading/Job.h"

void Job::release()
{
}

void Job::addDependentJob(Job* job)
{
    ScopedLock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::addDependentJobOnce(Job* job)
{
    ScopedLock lk(executeMutex);
    dependentJobs.addOnce(job);
}

void Job::setPendingInfos(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo)
{
    waitingSymbolSolved = symbolToWait;
    waitingKind         = waitKind;
    waitingNode         = node;
    waitingHintNode     = nullptr;
    waitingType         = typeInfo;
}

void Job::setPending(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(baseContext);
    if (baseContext->result == ContextResult::Pending)
        return;
    setPendingInfos(waitKind, symbolToWait, node, typeInfo);
    baseContext->result = ContextResult::Pending;
}

void Job::addPendingInline(const JobPendingInline& pending)
{
    pendingInlines.push_back(pending);
}

void Job::removePendingInline(const AstIdentifier* node)
{
    for (uint32_t i = 0; i < pendingInlines.size(); i++)
    {
        if (pendingInlines[i].identifier == node)
        {
            pendingInlines.erase(i);
            break;
        }
    }
}
