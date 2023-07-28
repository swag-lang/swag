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

const uint32_t JOB_IS_IN_QUEUE          = 0x00000001;
const uint32_t JOB_IS_IN_THREAD         = 0x00000002;
const uint32_t JOB_IS_PENDING           = 0x00000004;
const uint32_t JOB_IS_PENDING_RUN       = 0x00000008;
const uint32_t JOB_COMPILER_PASS        = 0x00000010;
const uint32_t JOB_IS_IO                = 0x00000020;
const uint32_t JOB_IS_OPT               = 0x00000040;
const uint32_t JOB_PENDING_PLACE_HOLDER = 0x00000080;
const uint32_t JOB_ACCEPT_PENDING_COUNT = 0x00000100;
const uint32_t JOB_IS_DEBUGGER          = 0x00000200;

enum class ContextResult
{
    Done,
    Pending,
    NewChilds,
    NewChilds1,
};

#define YIELD()                                 \
    if (context->result != ContextResult::Done) \
        return true;

struct JobContext : public ErrorContext
{
    void reset()
    {
        baseJob           = nullptr;
        validIfParameters = nullptr;
        result            = ContextResult::Done;
        ErrorContext::reset();
    }

    Job*     baseJob           = nullptr;
    AstNode* validIfParameters = nullptr;

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
    GenExportedType,
    GenExportedType1,
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
    WaitStructSymbol,
};

struct Job
{
    virtual JobResult execute() = 0;
    virtual void      release(){};

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
    void setPendingInfos(JobWaitKind waitKind, SymbolName* symbolToWait = nullptr, AstNode* node = nullptr, TypeInfo* typeInfo = nullptr);
    void setPending(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo);

    SharedMutex            executeMutex;
    SharedMutex            mutexDependent;
    DependentJobs          dependentJobs;
    VectorNative<AstNode*> nodes;
    VectorNative<Job*>     jobsToAdd;

    JobThread*  jobThread    = nullptr;
    AstNode*    originalNode = nullptr;
    SourceFile* sourceFile   = nullptr;
    Module*     module       = nullptr;
    Job*        dependentJob = nullptr;
    Job*        wakeUpBy     = nullptr;
    JobContext* baseContext  = nullptr;
    JobGroup*   jobGroup     = nullptr;

    VectorNative<Job*> waitingJobs;
    SymbolName*        waitingSymbolSolved = nullptr;
    AstNode*           waitingNode         = nullptr;
    AstNode*           waitingHintNode     = nullptr;
    TypeInfo*          waitingType         = nullptr;
    JobWaitKind        waitingKind         = JobWaitKind::None;

    int32_t  waitingJobIndex = -1;
    uint32_t waitOnJobs      = 0;
    uint32_t flags           = 0;
    uint32_t affinity        = UINT32_MAX;
};
