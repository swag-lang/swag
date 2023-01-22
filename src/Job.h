#pragma once
#include "DependentJobs.h"
#include "VectorNative.h"
#include "Utf8.h"
#include "Mutex.h"
#include "ErrorContext.h"
struct JobThread;
struct AstNode;
struct SymbolName;
struct SymbolOverload;
struct SourceFile;
struct Module;
struct Diagnostic;
struct TypeInfo;
struct AstFuncDecl;
struct JobGroup;
enum class DiagnosticLevel;

static const uint32_t JOB_IS_IN_QUEUE          = 0x00000001;
static const uint32_t JOB_IS_IN_THREAD         = 0x00000002;
static const uint32_t JOB_IS_PENDING           = 0x00000004;
static const uint32_t JOB_IS_PENDING_RUN       = 0x00000008;
static const uint32_t JOB_COMPILER_PASS        = 0x00000010;
static const uint32_t JOB_IS_IO                = 0x00000020;
static const uint32_t JOB_IS_OPT               = 0x00000040;
static const uint32_t JOB_PENDING_PLACE_HOLDER = 0x00000080;
static const uint32_t JOB_ACCEPT_PENDING_COUNT = 0x00000100;
static const uint32_t JOB_IS_DEBUGGER          = 0x00000200;

enum class ContextResult
{
    Done,
    Pending,
    NewChilds,
    NewChilds1,
};

struct JobContext
{
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
    bool checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue);

    void reset()
    {
        baseJob            = nullptr;
        node               = nullptr;
        sourceFile         = nullptr;
        selectIfParameters = nullptr;
        result             = ContextResult::Done;
        hasError           = false;
        silentError        = 0;
        errorContextStack.clear();
    }

    vector<ErrorContext> errorContextStack;

    Job*        baseJob            = nullptr;
    AstNode*    node               = nullptr;
    SourceFile* sourceFile         = nullptr;
    AstNode*    selectIfParameters = nullptr;
    bool        hasError           = false;
    uint32_t    silentError        = false;

    ContextResult result = ContextResult::Done;
};

enum class JobResult
{
    Continue,
    ReleaseJob,
    KeepJobAlive,
};

enum class JobWaitKind
{
    None,
    SemByteCodeGenerated,
    SemByteCodeResolved,
    SemFullResolve,
    SemPartialResolve,
    MakeConcrete,
    MakeConcrete1,
    PendingLambdaTyping,
    LoadFile,
    AskBcWaitResolve,
    MakeInline,
    EmitInit,
    EmitDrop,
    EmitPostMove,
    EmitPostCopy,
    WaitSymbol,
    WaitInterfacesFor,
    WaitInterfaces,
    WaitInterfacesReg,
    WaitMethods,
    WaitSpecialMethods,
    DepDone,
    WaitDepDoneExec,
};

struct Job
{
    virtual JobResult execute() = 0;

    virtual void release()
    {
    }

    void addDependentJob(Job* job);
    void waitSymbolNoLock(SymbolName* symbol);
    void waitAllStructInterfacesReg(TypeInfo* typeInfo);
    void waitAllStructInterfaces(TypeInfo* typeInfo);
    void waitAllStructSpecialMethods(TypeInfo* typeInfo);
    void waitAllStructMethods(TypeInfo* typeInfo);
    void waitStructGeneratedAlloc(TypeInfo* typeInfo);
    void waitStructGenerated(TypeInfo* typeInfo);
    void waitOverloadCompleted(SymbolOverload* overload);
    void waitFuncDeclFullResolve(AstFuncDecl* funcDecl);
    void waitTypeCompleted(TypeInfo* typeInfo);
    void setPending(SymbolName* symbolToWait, JobWaitKind waitKind, AstNode* node, TypeInfo* typeInfo);

    SharedMutex            executeMutex;
    SharedMutex            mutexDependent;
    DependentJobs          dependentJobs;
    VectorNative<AstNode*> nodes;
    VectorNative<Job*>     jobsToAdd;
    VectorNative<Job*>     waitingJobs;

    JobThread*  jobThread           = nullptr;
    AstNode*    originalNode        = nullptr;
    SourceFile* sourceFile          = nullptr;
    Module*     module              = nullptr;
    Job*        dependentJob        = nullptr;
    Job*        wakeUpBy            = nullptr;
    JobContext* baseContext         = nullptr;
    SymbolName* waitingSymbolSolved = nullptr;
    AstNode*    waitingIdNode       = nullptr;
    AstNode*    waitingHintNode     = nullptr;
    TypeInfo*   waitingIdType       = nullptr;
    JobGroup*   jobGroup            = nullptr;

    JobWaitKind waitingKind     = JobWaitKind::None;
    int32_t     waitingJobIndex = -1;
    uint32_t    waitOnJobs      = 0;
    uint32_t    flags           = 0;
    uint32_t    affinity        = UINT32_MAX;
};
