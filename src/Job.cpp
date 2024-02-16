#include "pch.h"
#include "Job.h"

void Job::release()
{
}

void Job::addDependentJob(Job* job)
{
	ScopedLock lk(executeMutex);
	dependentJobs.add(job);
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
