#pragma once
#include "ByteCodeInstruction.h"
#include "ByteCodeOp.h"
#include "Mutex.h"
#include "Profiler.h"
#include "RaceCondition.h"
#include "Register.h"

struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCode;
struct SourceLocation;

struct ByteCodePrintOptions
{
	ByteCodeInstruction* curIp = nullptr;
};

struct ByteCode
{
	struct Location
	{
		SourceFile*     file     = nullptr;
		SourceLocation* location = nullptr;
	};

	struct PrintInstructionLine
	{
		Utf8 rank;
		Utf8 name;
		Utf8 instRef;
		Utf8 flags;
		Utf8 pretty;
#ifdef SWAG_DEV_MODE
		Utf8 devMode;
#endif
	};

	void release();

	static bool isMemCpy(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_IS_MEMCPY; }
	static bool isPushParam(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_IS_PUSH_PARAM; }
	static bool isCall(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_IS_CALL; }
	static bool isJump(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_IS_JUMP; }
	static bool isJumpDyn(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_IS_JUMPDYN; }
	static bool isJumpOrDyn(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_IS_JUMP | OPFLAG_IS_JUMPDYN); }
	static bool isRet(const ByteCodeInstruction* inst) { return inst->op == ByteCodeOp::Ret || inst->op == ByteCodeOp::CopyRBtoRRRet; }

	static bool hasRefToRegA(const ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !inst->hasFlag(BCI_IMM_A) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A); }
	static bool hasRefToRegB(const ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !inst->hasFlag(BCI_IMM_B) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B); }
	static bool hasRefToRegC(const ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !inst->hasFlag(BCI_IMM_C) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C); }
	static bool hasRefToRegD(const ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !inst->hasFlag(BCI_IMM_D) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D); }
	static bool hasRefToReg(const ByteCodeInstruction* inst, uint32_t reg) { return hasRefToRegA(inst, reg) || hasRefToRegB(inst, reg) || hasRefToRegC(inst, reg) || hasRefToRegD(inst, reg); }

	static bool hasWriteRefToRegA(const ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !inst->hasFlag(BCI_IMM_A) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_A; }
	static bool hasWriteRefToRegB(const ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !inst->hasFlag(BCI_IMM_B) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_B; }
	static bool hasWriteRefToRegC(const ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !inst->hasFlag(BCI_IMM_C) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_C; }
	static bool hasWriteRefToRegD(const ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !inst->hasFlag(BCI_IMM_D) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_D; }
	static bool hasWriteRefToReg(const ByteCodeInstruction* inst, uint32_t reg) { return hasWriteRefToRegA(inst, reg) || hasWriteRefToRegB(inst, reg) || hasWriteRefToRegC(inst, reg) || hasWriteRefToRegD(inst, reg); }

	static bool hasReadRefToRegA(const ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !inst->hasFlag(BCI_IMM_A) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_A; }
	static bool hasReadRefToRegB(const ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !inst->hasFlag(BCI_IMM_B) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_B; }
	static bool hasReadRefToRegC(const ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !inst->hasFlag(BCI_IMM_C) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_C; }
	static bool hasReadRefToRegD(const ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !inst->hasFlag(BCI_IMM_D) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_D; }
	static bool hasReadRefToReg(const ByteCodeInstruction* inst, uint32_t reg) { return hasReadRefToRegA(inst, reg) || hasReadRefToRegB(inst, reg) || hasReadRefToRegC(inst, reg) || hasReadRefToRegD(inst, reg); }

	static bool hasWriteRegInA(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_A) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_A; }
	static bool hasWriteRegInB(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_B) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_B; }
	static bool hasWriteRegInC(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_C) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_C; }
	static bool hasWriteRegInD(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_D) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_WRITE_D; }

	static bool hasReadRegInA(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_A) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_A; }
	static bool hasReadRegInB(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_B) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_B; }
	static bool hasReadRegInC(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_C) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_C; }
	static bool hasReadRegInD(const ByteCodeInstruction* inst) { return !inst->hasFlag(BCI_IMM_D) && g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & OPFLAG_READ_D; }

	static bool hasSomethingInA(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A | OPFLAG_READ_VAL32_A | OPFLAG_READ_VAL64_A); }
	static bool hasSomethingInB(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B | OPFLAG_READ_VAL32_B | OPFLAG_READ_VAL64_B); }
	static bool hasSomethingInC(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C | OPFLAG_READ_VAL32_C | OPFLAG_READ_VAL64_C); }
	static bool hasSomethingInD(const ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[static_cast<int>(inst->op)].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D | OPFLAG_READ_VAL32_D | OPFLAG_READ_VAL64_D); }

	static void*    doByteCodeLambda(void* ptr);
	static void*    undoByteCodeLambda(void* ptr);
	static bool     isByteCodeLambda(void* ptr);
	static Location getLocation(const ByteCode* bc, const ByteCodeInstruction* ip, bool getInline = false);

	static uint32_t   getSetZeroAtPointerSize(const ByteCodeInstruction* inst, uint32_t& offset);
	static uint32_t   getSetZeroStackSize(const ByteCodeInstruction* inst, uint32_t& offset);
	void              printSourceCode(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip, uint32_t* lastLine = nullptr, SourceFile** lastFile = nullptr) const;
	static Utf8       getPrettyInstruction(ByteCodeInstruction* ip);
	static Utf8       getInstructionReg(const char* name, const Register& reg, bool regW, bool regR, bool regImm);
	void              getPrintInstruction(const ByteCodePrintOptions& options, ByteCodeInstruction* ip, PrintInstructionLine& line) const;
	static void       printInstruction(const ByteCodePrintOptions& options, const ByteCodeInstruction* ip, const PrintInstructionLine& line);
	void              printInstruction(const ByteCodePrintOptions& options, ByteCodeInstruction* ip) const;
	static void       alignPrintInstructions(const ByteCodePrintOptions& options, Vector<PrintInstructionLine>& lines, bool defaultLen = false);
	void              print(const ByteCodePrintOptions& options, uint32_t start, uint32_t count) const;
	void              print(const ByteCodePrintOptions& options);
	void              printName();
	Utf8              getPrintRefName();
	Utf8              getPrintName();
	Utf8              getPrintFileName() const;
	Utf8              getCallNameFromDecl();
	Utf8              getCallName();
	TypeInfoFuncAttr* getCallType() const;
	bool              canEmit() const;
	void              markLabels() const;
	bool              isDoingNothing() const;
	void              makeRoomForInstructions(uint32_t room = 1);

	static bool areSame(ByteCodeInstruction* start0, const ByteCodeInstruction* end0, const ByteCodeInstruction* start1, const ByteCodeInstruction* end1, bool specialJump, bool specialCall);
	uint32_t    computeCrc(ByteCodeInstruction* ip, uint32_t oldCrc, bool specialJump, bool specialCall) const;

	VectorNative<uint32_t>            availableRegistersRC;
	VectorNative<pair<void*, size_t>> autoFree;

	Mutex                  mutexCallName;
	Utf8                   name;
	Utf8                   callName;
	VectorNative<AstNode*> localVars;
	VectorNative<AstNode*> dependentCalls;
	SetUtf8                hasForeignFunctionCallsModules;

#ifdef SWAG_STATS
	MapUtf8<FFIStat> ffiProfile;
	uint64_t         profileStart     = 0;
	uint64_t         profileCumTime   = 0;
	uint64_t         profileFFI       = 0;
	uint32_t         profileCallCount = 0;
	double           profilePerCall   = 0;
	Set<ByteCode*>   profileChildren;
#endif

	ByteCodeInstruction* out          = nullptr;
	SourceFile*          sourceFile   = nullptr;
	TypeInfoFuncAttr*    typeInfoFunc = nullptr;
	AstNode*             node         = nullptr;

	uint32_t stackSize             = 0;
	uint32_t dynStackSize          = 0;
	uint32_t numInstructions       = 0;
	uint32_t maxInstructions       = 0;
	uint32_t maxCallResults        = 0;
	uint32_t maxCallParams         = 0;
	uint32_t maxSpVaargs           = 0;
	uint32_t maxReservedRegisterRC = 0;
	uint32_t numJumps              = 0;
	uint32_t staticRegs            = 0;
	uint32_t registerGetContext    = UINT32_MAX;
	uint32_t registerStoreRR       = UINT32_MAX;

	bool isCompilerGenerated     = false;
	bool isAddedToList           = false;
	bool isDirtyRegistersRC      = false;
	bool isUsed                  = false;
	bool forceEmit               = false;
	bool hasForeignFunctionCalls = false;
	bool sanDone                 = false;
	bool isInSeg                 = false;

	atomic<bool> isEmpty;

	SWAG_RACE_CONDITION_INSTANCE(raceCond);
};
