#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
#include "Assert.h"
#include "VectorNative.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;

static const uint32_t BCI_JUMP_DEST = 0x00000001;

struct ByteCodeInstruction
{
    Register   a;
    Register   b;
    Register   c;
    Register   d;
    AstNode*   node;
    ByteCodeOp op;
    uint32_t   flags;
};

struct ByteCode
{
    void enterByteCode(ByteCodeRunContext* context);
    void leaveByteCode();

    void              print();
    Utf8              callName();
    TypeInfoFuncAttr* callType();

    static const int ALIGN_RIGHT_OPCODE = 25;

    VectorNative<uint32_t>  availableRegistersRC;
    VectorNative<uint32_t>  availableRegistersRC2;
    VectorNative<Register*> registersRC;
    VectorNative<void*>     autoFree;

    Utf8 name;

    ByteCodeInstruction* out          = nullptr;
    SourceFile*          sourceFile   = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc = nullptr;
    AstNode*             node         = nullptr;

    uint32_t numInstructions       = 0;
    uint32_t maxInstructions       = 0;
    int32_t  maxCallResults        = 0;
    uint32_t maxReservedRegisterRC = 0;
    int32_t  curRC                 = -1;
    uint32_t numJumps              = 0;

    bool compilerGenerated = false;
    bool addedToList       = false;
};
