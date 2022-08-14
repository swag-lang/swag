#pragma once
#include "Utf8.h"
#include "Register.h"
#include "Assert.h"
#include "VectorNative.h"
#include "ByteCodeOp.h"
#include "Mutex.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;
struct ByteCode;
struct SourceLocation;

static const uint32_t BCI_JUMP_DEST     = 0x00000001;
static const uint32_t BCI_DEBUG         = 0x00000002;
static const uint32_t BCI_CSTDONE       = 0x00000004;
static const uint32_t BCI_SAFETY        = 0x00000008;
static const uint32_t BCI_IMM_A         = 0x00000010;
static const uint32_t BCI_IMM_B         = 0x00000020;
static const uint32_t BCI_IMM_C         = 0x00000040;
static const uint32_t BCI_IMM_D         = 0x00000080;
static const uint32_t BCI_OPT_FLAG      = 0x00000100;
static const uint32_t BCI_POST_COPYMOVE = 0x00000200;
static const uint32_t BCI_UNPURE        = 0x00000400;
static const uint32_t BCI_TRYCATCH      = 0x00000800;
static const uint32_t BCI_SHIFT_SMALL   = 0x00001000;
static const uint32_t BCI_START_STMT_N  = 0x00002000;
static const uint32_t BCI_START_STMT_S  = 0x00004000;
static const uint32_t BCI_START_STMT    = BCI_START_STMT_N | BCI_START_STMT_S;
static const uint32_t BCI_NO_BACKEND    = 0x00008000;
static const uint32_t BCI_VARIADIC      = 0x00010000;

struct ByteCodeInstruction
{
    Register        a;
    Register        b;
    Register        c;
    Register        d;
    AstNode*        node;
    SourceLocation* location;
    uint32_t        flags;
    ByteCodeOp      op;
#ifdef SWAG_DEV_MODE
    const char* sourceFile;
    int         sourceLine;
    int         serial;
    int         treeNode;
    uint32_t    crc;
#endif
};

struct ByteCode
{
    static uint32_t getSetZeroAtPointerSize(ByteCodeInstruction* inst, uint32_t& offset)
    {
        switch (inst->op)
        {
        case ByteCodeOp::SetZeroAtPointer8:
            offset = inst->b.u32;
            return 1;
        case ByteCodeOp::SetZeroAtPointer16:
            offset = inst->b.u32;
            return 2;
        case ByteCodeOp::SetZeroAtPointer32:
            offset = inst->b.u32;
            return 4;
        case ByteCodeOp::SetZeroAtPointer64:
            offset = inst->b.u32;
            return 8;
        case ByteCodeOp::SetZeroAtPointerX:
            offset = inst->c.u32;
            return inst->b.u32;
        }

        return 0;
    }

    static uint32_t getSetZeroStackSize(ByteCodeInstruction* inst, uint32_t& offset)
    {
        switch (inst->op)
        {
        case ByteCodeOp::SetZeroStack8:
            offset = inst->a.u32;
            return 1;
        case ByteCodeOp::SetZeroStack16:
            offset = inst->a.u32;
            return 2;
        case ByteCodeOp::SetZeroStack32:
            offset = inst->a.u32;
            return 4;
        case ByteCodeOp::SetZeroStack64:
            offset = inst->a.u32;
            return 8;
        case ByteCodeOp::SetZeroStackX:
            offset = inst->a.u32;
            return inst->b.u32;
        }

        return 0;
    }

    void releaseOut();

    // clang-format off
    static uint32_t isCopyRBtoRA(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_IS_COPY_RBRA; }
    static bool     isMemCpy(ByteCodeInstruction* inst)     { return g_ByteCodeOpDesc[(int) inst->op].flags & OPFLAG_IS_MEMCPY; }
    static bool     isJump(ByteCodeInstruction* inst)       { return g_ByteCodeOpDesc[(int) inst->op].flags & OPFLAG_IS_JUMP; }
    static bool     isJumpDyn(ByteCodeInstruction* inst)    { return g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_IS_JUMPDYN; }
    static bool     isJumpOrDyn(ByteCodeInstruction* inst)  { return (g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_IS_JUMP| OPFLAG_IS_JUMPDYN)); }

    inline static bool hasRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A); }
    inline static bool hasRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B); }
    inline static bool hasRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C); }
    inline static bool hasRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D); }
    inline static bool hasRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasRefToRegA(inst, reg) || hasRefToRegB(inst, reg) || hasRefToRegC(inst, reg) || hasRefToRegD(inst, reg); }

    inline static bool hasWriteRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_A; }
    inline static bool hasWriteRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_B; }
    inline static bool hasWriteRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_C; }
    inline static bool hasWriteRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_D; }
    inline static bool hasWriteRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasWriteRefToRegA(inst, reg) || hasWriteRefToRegB(inst, reg) || hasWriteRefToRegC(inst, reg) || hasWriteRefToRegD(inst, reg); }

    inline static bool hasReadRefToRegA(ByteCodeInstruction* inst, uint32_t reg) { return inst->a.u32 == reg && !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_A; }
    inline static bool hasReadRefToRegB(ByteCodeInstruction* inst, uint32_t reg) { return inst->b.u32 == reg && !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_B; }
    inline static bool hasReadRefToRegC(ByteCodeInstruction* inst, uint32_t reg) { return inst->c.u32 == reg && !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_C; }
    inline static bool hasReadRefToRegD(ByteCodeInstruction* inst, uint32_t reg) { return inst->d.u32 == reg && !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_D; }
    inline static bool hasReadRefToReg(ByteCodeInstruction* inst, uint32_t reg)  { return hasReadRefToRegA(inst, reg) || hasReadRefToRegB(inst, reg) || hasReadRefToRegC(inst, reg) || hasReadRefToRegD(inst, reg); }

    inline static bool hasWriteRegInA(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_A; }
    inline static bool hasWriteRegInB(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_B; }
    inline static bool hasWriteRegInC(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_C; }
    inline static bool hasWriteRegInD(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_WRITE_D; }

    inline static bool hasReadRegInA(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_A) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_A; }
    inline static bool hasReadRegInB(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_B) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_B; }
    inline static bool hasReadRegInC(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_C) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_C; }
    inline static bool hasReadRegInD(ByteCodeInstruction* inst) { return !(inst->flags & BCI_IMM_D) && g_ByteCodeOpDesc[(int)inst->op].flags & OPFLAG_READ_D; }

    inline static bool hasSomethingInA(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_A | OPFLAG_WRITE_A | OPFLAG_READ_VAL32_A | OPFLAG_READ_VAL64_A); }
    inline static bool hasSomethingInB(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_B | OPFLAG_WRITE_B | OPFLAG_READ_VAL32_B | OPFLAG_READ_VAL64_B); }
    inline static bool hasSomethingInC(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_C | OPFLAG_WRITE_C | OPFLAG_READ_VAL32_C | OPFLAG_READ_VAL64_C); }
    inline static bool hasSomethingInD(ByteCodeInstruction* inst) { return g_ByteCodeOpDesc[(int)inst->op].flags & (OPFLAG_READ_D | OPFLAG_WRITE_D | OPFLAG_READ_VAL32_D | OPFLAG_READ_VAL64_D); }
    // clang-format on

    void addCallStack(ByteCodeRunContext* context);
    void enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet = 0, uint32_t returnRegOnRet = UINT32_MAX, uint32_t incSPPostCall = 0);
    void leaveByteCode(ByteCodeRunContext* context, bool popCallStack = true);
    void markLabels();
    bool isDoingNothing();

    static void* doForeignLambda(void* ptr);
    static bool  isForeignLambda(void* ptr);
    static void* undoForeignLambda(void* ptr);
    static void* doByteCodeLambda(void* ptr);
    static void* undoByteCodeLambda(void* ptr);
    static bool  isByteCodeLambda(void* ptr);

    void              printSourceCode(ByteCodeInstruction* ip, uint32_t* lastLine = nullptr, SourceFile** lastFile = nullptr);
    void              printPrettyInstruction(ByteCodeInstruction* ip);
    void              printInstructionReg(const char* name, const Register& reg, bool regW, bool regR, bool regImm);
    void              printInstruction(ByteCodeInstruction* ip, ByteCodeInstruction* curIp = nullptr);
    void              print(ByteCodeInstruction* curIp = nullptr);
    void              setCallName(const Utf8& n);
    Utf8              getCallName();
    TypeInfoFuncAttr* getCallType();
    bool              canEmit();
    static void       getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force = false);

    VectorNative<uint32_t>            availableRegistersRC;
    VectorNative<pair<void*, size_t>> autoFree;

    Mutex                  mutexCallName;
    Utf8                   name;
    Utf8                   callName;
    VectorNative<AstNode*> localVars;
    VectorNative<AstNode*> dependentCalls;

    ByteCodeInstruction* out          = nullptr;
    SourceFile*          sourceFile   = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc = nullptr;
    AstNode*             node         = nullptr;

    uint32_t numInstructions       = 0;
    uint32_t maxInstructions       = 0;
    int32_t  maxCallResults        = 0;
    uint32_t maxCallParams         = 0;
    uint32_t maxSPVaargs           = 0;
    uint32_t maxReservedRegisterRC = 0;
    uint32_t numJumps              = 0;
    uint32_t crc                   = 0;
    uint32_t registerGetContext    = UINT32_MAX;

    bool isCompilerGenerated = false;
    bool isAddedToList       = false;
    bool isDirtyRegistersRC  = false;
    bool isUsed              = false;
    bool isDuplicated        = false;
    bool forceEmit           = false;
    bool hasFunctionCalls    = false;

    atomic<bool> isEmpty;
};
