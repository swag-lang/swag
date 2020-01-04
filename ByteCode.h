#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
#include "Assert.h"
#include "Vector.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;

struct ByteCodeInstruction
{
    Register   a;
    Register   b;
    Register   c;
    Register   d;
    AstNode*   node;
    ByteCodeOp op;
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

    bool compilerGenerated = false;
    bool addedToList       = false;
};
