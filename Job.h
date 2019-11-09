#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "DependentJobs.h"
struct JobThread;
struct AstNode;

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

static const uint32_t JOB_IS_DEPENDENT = 0x00000001;
static const uint32_t JOB_IS_IN_THREAD = 0x00000002;

struct Job : public PoolElement
{
    virtual JobResult execute() = 0;

    void reset() override
    {
        flags        = 0;
        pendingIndex = -1;
        thread       = nullptr;
        dependentNodes.clear();
    }

    void addDependentJob(Job* job);
    void doneJob();

    virtual void setPending()
    {
    }

    SpinLock         executeMutex;
    uint32_t         flags;
    JobThread*       thread;
    int              pendingIndex;
    DependentJobs    dependentJobs;
    vector<AstNode*> dependentNodes;
};
