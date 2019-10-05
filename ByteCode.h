#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;

struct ByteCodeInstruction
{
    Register       a;
    Register       b;
    Register       c;
    Register       cache;
    uint32_t       sourceFileIdx;
    SourceLocation startLocation;
    SourceLocation endLocation;
    ByteCodeOp     op;
};

struct ByteCode : public PoolElement
{
    void print();

    static const int     ALIGN_RIGHT_OPCODE = 25;
    ByteCodeInstruction* out                = nullptr;
    uint32_t             numInstructions    = 0;
    uint32_t             maxInstructions    = 0;
    int                  maxCallResults     = 0;
    SourceFile*          sourceFile         = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc       = nullptr;
    AstNode*             node               = nullptr;
    string               name;
    bool                 compilerGenerated = false;

    uint32_t          maxReservedRegisterRC = 0;
    vector<uint32_t>  availableRegistersRC;
    vector<Register*> registersRC;
    int               curRC = -1;
    void              enterByteCode(ByteCodeRunContext* context);
    void              leaveByteCode();

    string            callName();
    TypeInfoFuncAttr* callType();
};

extern Pool<ByteCode> g_Pool_byteCode;