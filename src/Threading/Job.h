#pragma once
#include "Jobs/DependentJobs.h"
#include "Report/ErrorContext.h"
#include "Threading/Mutex.h"

struct AstFuncDecl;
struct AstNode;
struct Diagnostic;
struct JobGroup;
struct JobThread;
struct Module;
struct SourceFile;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;

using JobFlags                              = Flags<uint32_t>;
constexpr JobFlags JOB_IS_IN_QUEUE          = 0x00000001;
constexpr JobFlags JOB_IS_IN_THREAD         = 0x00000002;
constexpr JobFlags JOB_IS_PENDING           = 0x00000004;
constexpr JobFlags JOB_IS_PENDING_RUN       = 0x00000008;
constexpr JobFlags JOB_COMPILER_PASS        = 0x00000010;
constexpr JobFlags JOB_IS_IO                = 0x00000020;
constexpr JobFlags JOB_IS_OPT               = 0x00000040;
constexpr JobFlags JOB_PENDING_PLACE_HOLDER = 0x00000080;
constexpr JobFlags JOB_ACCEPT_PENDING_COUNT = 0x00000100;
constexpr JobFlags JOB_IS_DEBUGGER          = 0x00000200;

enum class ContextResult
{
    Done,
    Pending,
    NewChildren,
    NewChildren1,
};

struct JobContext : ErrorContext
{
    void reset()
    {
        baseJob         = nullptr;
        whereParameters = nullptr;
        result          = ContextResult::Done;
        ErrorContext::reset();
    }

    Job*     baseJob         = nullptr;
    AstNode* whereParameters = nullptr;

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
    SemByteCodeGenerated1,
    SemByteCodeGenerated2,
    SemByteCodeGenerated3,
    SemByteCodeGenerated4,
    SemByteCodeResolved,
    SemFullResolve,
    SemPartialResolve,
    SemStructStartSolve,
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
    WaitPreResolve,
};

struct Job
{
    virtual JobResult execute() = 0;
    virtual void      release();
    virtual ~         Job() = default;

    void addDependentJob(Job* job);
    void addDependentJobOnce(Job* job);
    void setPendingInfos(JobWaitKind waitKind, SymbolName* symbolToWait = nullptr, AstNode* node = nullptr, TypeInfo* typeInfo = nullptr);
    void setPending(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo);

    bool hasFlag(JobFlags fl) const { return flags.has(fl); }
    void addFlag(JobFlags fl) { flags.add(fl); }
    void removeFlag(JobFlags fl) { flags.remove(fl); }

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

    uint32_t waitingJobIndex = UINT32_MAX;
    uint32_t waitOnJobs      = 0;
    JobFlags flags           = 0;
    uint32_t affinity        = UINT32_MAX;
};

#define YIELD()                                     \
    do                                              \
    {                                               \
        if (context->result != ContextResult::Done) \
            return true;                            \
    } while (0)

#define YIELD_VOID()                                \
    do                                              \
    {                                               \
        if (context->result != ContextResult::Done) \
            return;                                 \
    } while (0)
