#pragma once
#include "Pool.h"
#include "DependentJobs.h"
struct JobThread;
struct AstNode;
struct SymbolName;
struct SourceFile;
struct Diagnostic;
struct TypeInfo;

enum class ContextResult
{
    Done,
    Pending,
    NewChilds,
};

struct JobContext
{
    AstNode*         node;
    ContextResult    result     = ContextResult::Done;
    SourceFile*      sourceFile = nullptr;
    vector<AstNode*> expansionNode;

    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);

    void reset()
    {
        expansionNode.clear();
    }
};

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

static const uint32_t JOB_IS_DEPENDENT = 0x00000001;
static const uint32_t JOB_IS_IN_THREAD = 0x00000002;

struct Job
{
    virtual JobResult execute() = 0;

    void addDependentJob(Job* job);
    void doneJob();
    void waitForSymbolNoLock(SymbolName* symbol);
    void waitForAllStructInterfaces(TypeInfo* typeInfo);
    void setPending();

    shared_mutex     executeMutex;
    DependentJobs    dependentJobs;
    vector<AstNode*> dependentNodes;
    vector<AstNode*> nodes;
    JobThread*       thread              = nullptr;
    SymbolName*      waitingSymbolSolved = nullptr;
    SourceFile*      sourceFile          = nullptr;
    JobContext*      baseContext         = nullptr;
    uint32_t         flags               = 0;
    int32_t          pendingIndex        = -1;
};
