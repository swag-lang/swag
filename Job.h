#pragma once
#include "Pool.h"
#include "DependentJobs.h"
#include "VectorNative.h"
struct JobThread;
struct AstNode;
struct SymbolName;
struct SourceFile;
struct Module;
struct Diagnostic;
struct TypeInfo;

enum class JobKind
{
    MISC,
    BACKEND_FCT_BODY,
};

static const uint32_t AFFINITY_BACKEND_FCTBODY = 0x00000001;
static const uint32_t AFFINITY_EXECBC   = 0x00000002;
static const uint32_t AFFINITY_ALL      = 0xFFFFFFFF;

enum class ContextResult
{
    Done,
    Pending,
    NewChilds,
};

struct JobContext
{
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);

    void reset()
    {
        baseJob    = nullptr;
        node       = nullptr;
        sourceFile = nullptr;
        result     = ContextResult::Done;
        expansionNode.clear();
    }

    VectorNative<AstNode*> expansionNode;

    Job*        baseJob    = nullptr;
    AstNode*    node       = nullptr;
    SourceFile* sourceFile = nullptr;

    ContextResult result = ContextResult::Done;
};

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
    KeepJobAlivePending,
};

static const uint32_t JOB_IS_IN_QUEUE    = 0x00000001;
static const uint32_t JOB_IS_IN_THREAD   = 0x00000002;
static const uint32_t JOB_IS_PENDING     = 0x00000004;
static const uint32_t JOB_IS_PENDING_RUN = 0x00000008;

struct Job : public PoolElem
{
    virtual JobResult execute() = 0;

    void addDependentJob(Job* job);
    void waitForSymbolNoLock(SymbolName* symbol);
    void waitForAllStructInterfaces(TypeInfo* typeInfo);
    void waitForAllStructMethods(TypeInfo* typeInfo);
    void setPending(SymbolName* symbolToWait);

    shared_mutex           executeMutex;
    DependentJobs          dependentJobs;
    VectorNative<AstNode*> dependentNodes;
    VectorNative<AstNode*> nodes;
    VectorNative<Job*>     jobsToAdd;

    AstNode*    originalNode        = nullptr;
    SymbolName* waitingSymbolSolved = nullptr;
    SourceFile* sourceFile          = nullptr;
    Module*     module              = nullptr;
    Job*        dependentJob        = nullptr;
    JobContext* baseContext         = nullptr;

    uint32_t flags           = 0;
    int32_t  waitingJobIndex = -1;
    uint32_t waitOnJobs      = 0;
    uint32_t affinity        = AFFINITY_ALL;
    JobKind  jobKind         = JobKind::MISC;

    void reset() override
    {
        dependentJobs.clear();
        dependentNodes.clear();
        nodes.clear();
        jobsToAdd.clear();
        originalNode        = nullptr;
        waitingSymbolSolved = nullptr;
        sourceFile          = nullptr;
        module              = nullptr;
        dependentJob        = nullptr;
        baseContext         = nullptr;
        flags               = 0;
        waitingJobIndex     = -1;
        waitOnJobs          = 0;
    }

    void release() override
    {
    }
};
