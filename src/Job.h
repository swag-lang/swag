#pragma once
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
    bool report(const char* hint, const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue);
    bool internalError(const char* msg, AstNode* specNode = nullptr);

    void reset()
    {
        baseJob            = nullptr;
        node               = nullptr;
        sourceFile         = nullptr;
        selectIfParameters = nullptr;
        result             = ContextResult::Done;
        hasError           = false;
        expansionNode.clear();
    }

    enum class ExpansionType
    {
        Generic,
        Inline,
        SelectIf,
        CheckIf,
        Node,
    };

    VectorNative<pair<AstNode*, ExpansionType>> expansionNode;

    Job*        baseJob            = nullptr;
    AstNode*    node               = nullptr;
    SourceFile* sourceFile         = nullptr;
    AstNode*    selectIfParameters = nullptr;
    bool        hasError           = false;

    ContextResult result = ContextResult::Done;
};

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

struct Job
{
    virtual JobResult execute() = 0;

    virtual void release()
    {
    }

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
    SourceFile* sourceFile          = nullptr;
    Module*     module              = nullptr;
    Job*        dependentJob        = nullptr;
    Job*        wakeUpBy            = nullptr;
    JobContext* baseContext         = nullptr;
    const char* waitingId           = nullptr;
    AstNode*    waitingIdNode       = nullptr;
    TypeInfo*   waitingIdType       = nullptr;

    int32_t  waitingJobIndex = -1;
    uint32_t waitOnJobs      = 0;

    uint8_t flags = 0;
};
