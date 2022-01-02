#pragma once
#include "DependentJobs.h"
#include "VectorNative.h"
#include "Utf8.h"
#include "Mutex.h"
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

static const uint8_t JOB_IS_IN_QUEUE       = 0x01;
static const uint8_t JOB_IS_IN_THREAD      = 0x02;
static const uint8_t JOB_IS_PENDING        = 0x04;
static const uint8_t JOB_IS_PENDING_RUN    = 0x08;
static const uint8_t JOB_COMPILER_PASS     = 0x10;
static const uint8_t JOB_IS_IO             = 0x20;
static const uint8_t JOB_IS_OPT            = 0x40;
static const uint8_t JOB_NO_PENDING_REPORT = 0x80;

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
        errorContextStack.clear();
    }

    enum class ErrorContextType
    {
        Generic,
        Inline,
        SelectIf,
        CheckIf,
        Node,
        Export,
        Message,
    };

    struct ErrorContext
    {
        AstNode*         node = nullptr;
        ErrorContextType type = ErrorContextType::Node;
        Utf8             msg  = "";
    };

    vector<ErrorContext> errorContextStack;

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

enum class JobWaitKind
{
    None,
    SemByteCodeGenerated,
    SemByteCodeResolved,
    SemFullResolve,
    SemPartialResolve,
    MakeConcrete,
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

    JobThread*  jobThread           = nullptr;
    AstNode*    originalNode        = nullptr;
    SourceFile* sourceFile          = nullptr;
    Module*     module              = nullptr;
    Job*        dependentJob        = nullptr;
    Job*        wakeUpBy            = nullptr;
    JobContext* baseContext         = nullptr;
    SymbolName* waitingSymbolSolved = nullptr;
    AstNode*    waitingIdNode       = nullptr;
    TypeInfo*   waitingIdType       = nullptr;
    Job*        waitingJob          = nullptr;
    JobGroup*   jobGroup            = nullptr;

    JobWaitKind waitingKind     = JobWaitKind::None;
    int32_t     waitingJobIndex = -1;
    uint32_t    waitOnJobs      = 0;

    uint8_t flags = 0;
};
