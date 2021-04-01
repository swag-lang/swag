#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
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
    inline static uint32_t isSetZeroAtPointer(ByteCodeInstruction* inst, uint32_t& offset)
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

    inline static uint32_t isSetZeroStack(ByteCodeInstruction* inst, uint32_t& offset)
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

    inline static bool isMemCpy(ByteCodeInstruction* inst)
    {
        return inst->op == ByteCodeOp::MemCpy8 ||
               inst->op == ByteCodeOp::MemCpy16 ||
               inst->op == ByteCodeOp::MemCpy32 ||
               inst->op == ByteCodeOp::MemCpy64 ||
               inst->op == ByteCodeOp::MemCpyX;
    }

    inline static bool isJump(ByteCodeInstruction* inst)
    {
        return inst->op == ByteCodeOp::Jump ||
               inst->op == ByteCodeOp::JumpIfTrue ||
               inst->op == ByteCodeOp::JumpIfFalse ||
               inst->op == ByteCodeOp::JumpIfNotZero8 ||
               inst->op == ByteCodeOp::JumpIfNotZero16 ||
               inst->op == ByteCodeOp::JumpIfNotZero32 ||
               inst->op == ByteCodeOp::JumpIfNotZero64 ||
               inst->op == ByteCodeOp::JumpIfZero8 ||
               inst->op == ByteCodeOp::JumpIfZero16 ||
               inst->op == ByteCodeOp::JumpIfZero32 ||
               inst->op == ByteCodeOp::JumpIfZero64 ||
               inst->op == ByteCodeOp::JumpIfLowerU32 ||
               inst->op == ByteCodeOp::JumpIfLowerU64 ||
               inst->op == ByteCodeOp::JumpIfLowerS32 ||
               inst->op == ByteCodeOp::JumpIfLowerS64 ||
               inst->op == ByteCodeOp::JumpIfLowerF32 ||
               inst->op == ByteCodeOp::JumpIfLowerF64 ||
               inst->op == ByteCodeOp::JumpIfLowerEqU32 ||
               inst->op == ByteCodeOp::JumpIfLowerEqU64 ||
               inst->op == ByteCodeOp::JumpIfLowerEqS32 ||
               inst->op == ByteCodeOp::JumpIfLowerEqS64 ||
               inst->op == ByteCodeOp::JumpIfLowerEqF32 ||
               inst->op == ByteCodeOp::JumpIfLowerEqF64 ||
               inst->op == ByteCodeOp::JumpIfGreaterU32 ||
               inst->op == ByteCodeOp::JumpIfGreaterU64 ||
               inst->op == ByteCodeOp::JumpIfGreaterS32 ||
               inst->op == ByteCodeOp::JumpIfGreaterS64 ||
               inst->op == ByteCodeOp::JumpIfGreaterF32 ||
               inst->op == ByteCodeOp::JumpIfGreaterF64 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqU32 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqU64 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqS32 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqS64 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqF32 ||
               inst->op == ByteCodeOp::JumpIfGreaterEqF64 ||
               inst->op == ByteCodeOp::JumpIfNotEqual8 ||
               inst->op == ByteCodeOp::JumpIfNotEqual16 ||
               inst->op == ByteCodeOp::JumpIfNotEqual32 ||
               inst->op == ByteCodeOp::JumpIfNotEqual64 ||
               inst->op == ByteCodeOp::JumpIfEqual8 ||
               inst->op == ByteCodeOp::JumpIfEqual16 ||
               inst->op == ByteCodeOp::JumpIfEqual32 ||
               inst->op == ByteCodeOp::JumpIfEqual64;
    }

    void addCallStack(ByteCodeRunContext* context);
    void enterByteCode(ByteCodeRunContext* context, uint32_t popParamsOnRet = 0, uint32_t returnReg = UINT32_MAX);
    void leaveByteCode(ByteCodeRunContext* context, bool popCallStack = true);
    void markLabels();
    bool isDoingNothing();

    void              printSourceCode(ByteCodeInstruction* ip, uint32_t* lastLine = nullptr, SourceFile** lastFile = nullptr);
    void              printPrettyInstruction(ByteCodeInstruction* ip);
    void              printInstruction(ByteCodeInstruction* ip, ByteCodeInstruction* curIp = nullptr);
    void              print(ByteCodeInstruction* curIp = nullptr);
    Utf8              callName();
    TypeInfoFuncAttr* callType();
    static void       getLocation(ByteCode* bc, ByteCodeInstruction* ip, SourceFile** file, SourceLocation** location, bool force = false);

    static const int ALIGN_RIGHT_OPCODE = 25;

    VectorNative<uint32_t>            availableRegistersRC;
    bool                              dirtyRegistersRC = false;
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

    bool         compilerGenerated = false;
    bool         isPostMove        = false;
    bool         addedToList       = false;
    atomic<bool> running;
    atomic<bool> isEmpty;
};
