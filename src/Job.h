#pragma once
#include "DependentJobs.h"
#include "ErrorContext.h"
#include "Mutex.h"

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

constexpr uint32_t JOB_IS_IN_QUEUE          = 0x00000001;
constexpr uint32_t JOB_IS_IN_THREAD         = 0x00000002;
constexpr uint32_t JOB_IS_PENDING           = 0x00000004;
constexpr uint32_t JOB_IS_PENDING_RUN       = 0x00000008;
constexpr uint32_t JOB_COMPILER_PASS        = 0x00000010;
constexpr uint32_t JOB_IS_IO                = 0x00000020;
constexpr uint32_t JOB_IS_OPT               = 0x00000040;
constexpr uint32_t JOB_PENDING_PLACE_HOLDER = 0x00000080;
constexpr uint32_t JOB_ACCEPT_PENDING_COUNT = 0x00000100;
constexpr uint32_t JOB_IS_DEBUGGER          = 0x00000200;

enum class ContextResult
{
	Done,
	Pending,
	NewChilds,
	NewChilds1,
};

struct JobContext : ErrorContext
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
	virtual void      release();
	virtual           ~Job() = default;

	void addDependentJob(Job* job);
	void setPendingInfos(JobWaitKind waitKind, SymbolName* symbolToWait = nullptr, AstNode* node = nullptr, TypeInfo* typeInfo = nullptr);
	void setPending(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo);

	// clang-format off
	bool hasFlag(uint32_t fl) const { return flags & fl; }
	void addFlag(uint32_t fl) { flags |= fl; }
	void setFlags(uint32_t fl) { flags = fl; }
	void removeFlag(uint32_t fl) { flags &= ~fl; }
	// clang-format on

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

#define YIELD()                                     \
    do                                              \
    {                                               \
        if (context->result != ContextResult::Done) \
            return true;                            \
    } while (0)
