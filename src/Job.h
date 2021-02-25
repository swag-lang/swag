#pragma once
#include "Pool.h"
#include "DependentJobs.h"
#include "VectorNative.h"
#include "Utf8.h"
struct JobThread;
struct AstNode;
struct SymbolName;
struct SourceFile;
struct Module;
struct Diagnostic;
struct TypeInfo;

static const uint8_t AFFINITY_BACKEND_FCTBODY = 0x01;
static const uint8_t AFFINITY_EXECBC          = 0x02;
static const uint8_t AFFINITY_IO              = 0x04;
static const uint8_t AFFINITY_NONE            = 0x00;
static const uint8_t AFFINITY_ALL             = 0xFF;

static const uint8_t JOB_IS_IN_QUEUE    = 0x01;
static const uint8_t JOB_IS_IN_THREAD   = 0x02;
static const uint8_t JOB_IS_PENDING     = 0x04;
static const uint8_t JOB_IS_PENDING_RUN = 0x08;
static const uint8_t JOB_COMPILER_PASS  = 0x10;
static const uint8_t JOB_IS_IO          = 0x20;

enum class ContextResult
{
    Done,
    Pending,
    NewChilds,
};

struct JobContext
{
    void setErrorContext(const Diagnostic& diag, vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);

    void reset()
    {
        baseJob            = nullptr;
        node               = nullptr;
        sourceFile         = nullptr;
        selectIfParameters = nullptr;
        result             = ContextResult::Done;
        expansionNode.clear();
    }

    enum class ExpansionType
    {
        Generic,
        Inline,
        SelectIf,
    };

    VectorNative<pair<AstNode*, ExpansionType>> expansionNode;

    Job*        baseJob            = nullptr;
    AstNode*    node               = nullptr;
    SourceFile* sourceFile         = nullptr;
    AstNode*    selectIfParameters = nullptr;

    ContextResult result = ContextResult::Done;
};

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

struct Job : public PoolElem
{
    virtual JobResult execute() = 0;

    void addDependentJob(Job* job);
    void waitForSymbolNoLock(SymbolName* symbol);
    void waitForAllStructInterfaces(TypeInfo* typeInfo);
    void waitForAllStructMethods(TypeInfo* typeInfo);
    void waitStructGenerated(TypeInfo* typeInfo);
    void waitTypeCompleted(TypeInfo* typeInfo);
    void setPending(SymbolName* symbolToWait, const char* id, AstNode* node, TypeInfo* typeInfo);

    shared_mutex           executeMutex;
    shared_mutex           mutexDependent;
    DependentJobs          dependentJobs;
    VectorNative<AstNode*> dependentNodes;
    VectorNative<AstNode*> nodes;
    VectorNative<Job*>     jobsToAdd;

    AstNode*    originalNode        = nullptr;
    SymbolName* waitingSymbolSolved = nullptr;
    const char* waitingId           = nullptr;
    AstNode*    waitingIdNode       = nullptr;
    TypeInfo*   waitingIdType       = nullptr;
    SourceFile* sourceFile          = nullptr;
    Module*     module              = nullptr;
    Job*        dependentJob        = nullptr;
    Job*        wakeUpBy            = nullptr;
    JobContext* baseContext         = nullptr;

    int32_t  waitingJobIndex = -1;
    uint32_t waitOnJobs      = 0;

    uint8_t flags    = 0;
    uint8_t affinity = AFFINITY_ALL;

    void reset() override
    {
        dependentJobs.clear();
        dependentNodes.clear();
        nodes.clear();
        jobsToAdd.clear();
        originalNode        = nullptr;
        waitingSymbolSolved = nullptr;
        waitingId           = nullptr;
        waitingIdNode       = nullptr;
        waitingIdType       = nullptr;
        sourceFile          = nullptr;
        module              = nullptr;
        dependentJob        = nullptr;
        baseContext         = nullptr;
        wakeUpBy            = nullptr;
        flags               = 0;
        waitingJobIndex     = -1;
        waitOnJobs          = 0;
    }

    void release() override
    {
    }
};
