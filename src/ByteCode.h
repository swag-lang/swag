#pragma once
#include "Utf8.h"
#include "Register.h"
#include "Assert.h"
#include "VectorNative.h"
#include "ByteCodeOp.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;
struct ByteCode;
struct SourceLocation;

static const uint16_t BCI_JUMP_DEST     = 0x0001;
static const uint16_t BCI_DEBUG         = 0x0002;
static const uint16_t BCI_CSTDONE       = 0x0004;
static const uint16_t BCI_SAFETY        = 0x0008;
static const uint16_t BCI_IMM_A         = 0x0010;
static const uint16_t BCI_IMM_B         = 0x0020;
static const uint16_t BCI_IMM_C         = 0x0040;
static const uint16_t BCI_IMM_D         = 0x0080;
static const uint16_t BCI_OPT_FLAG      = 0x0100;
static const uint16_t BCI_START_STMT    = 0x0200;
static const uint16_t BCI_POST_COPYMOVE = 0x0400;
static const uint16_t BCI_UNPURE        = 0x0800;
static const uint16_t BCI_TRYCATCH      = 0x1000;
static const uint16_t BCI_SHIFT_SMALL   = 0x8000;

struct ByteCodeInstruction
{
    Register        a;
    Register        b;
    Register        c;
    Register        d;
    AstNode*        node;
    SourceLocation* location;
    ByteCodeOp      op;
    uint16_t        flags;
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

    // clang-format off
    static uint32_t isCopyRBtoRA(ByteCodeInstruction* inst) { return g_ByteCodeOpFlags[(int)inst->op] & OPFLAG_IS_COPY_RBRA; }
    static bool     isMemCpy(ByteCodeInstruction* inst)     { return g_ByteCodeOpFlags[(int) inst->op] & OPFLAG_IS_MEMCPY; }
    static bool     isJump(ByteCodeInstruction* inst)       { return g_ByteCodeOpFlags[(int) inst->op] & OPFLAG_IS_JUMP; }
    // clang-format on

    void addCallStack(ByteCodeRunContext* context);
    void enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet = 0, uint32_t returnReg = UINT32_MAX);
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
    Utf8              callName();
    TypeInfoFuncAttr* callType();
    static void       getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force = false);

    VectorNative<uint32_t>            availableRegistersRC;
    VectorNative<pair<void*, size_t>> autoFree;

    Utf8                   name;
    VectorNative<AstNode*> localVars;

    ByteCodeInstruction* out          = nullptr;
    SourceFile*          sourceFile   = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc = nullptr;
    AstNode*             node         = nullptr;

    uint32_t numInstructions       = 0;
    uint32_t maxInstructions       = 0;
    int32_t  maxCallResults        = 0;
    uint32_t maxCallParams         = 0;
    uint32_t maxReservedRegisterRC = 0;
    uint32_t numJumps              = 0;

    bool isCompilerGenerated = false;
    bool isAddedToList       = false;
    bool isDirtyRegistersRC  = false;

    atomic<bool> isRunning;
    atomic<bool> isEmpty;
};
