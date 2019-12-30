#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SourceLocation.h"
#include "Register.h"
#include "Assert.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct SourceFile;
struct TypeInfoFuncAttr;
struct ByteCodeRunContext;

static const uint16_t INSTRUCTION_NO_BACKEND = 0x00000001;

struct ByteCodeInstruction
{
    Register       a;
    Register       b;
    Register       c;
    Register       d;
    SourceLocation startLocation;
    SourceLocation endLocation;
    uint32_t       sourceFileIdx;
    ByteCodeOp     op;
    uint16_t       flags;
};

struct ByteCode
{
    void enterByteCode(ByteCodeRunContext* context);
    void leaveByteCode();

    void              print();
    Utf8              callName();
    TypeInfoFuncAttr* callType();

    static const int     ALIGN_RIGHT_OPCODE = 25;
    string               name;
    ByteCodeInstruction* out               = nullptr;
    SourceFile*          sourceFile        = nullptr;
    TypeInfoFuncAttr*    typeInfoFunc      = nullptr;
    AstNode*             node              = nullptr;
    uint32_t             numInstructions   = 0;
    uint32_t             maxInstructions   = 0;
    int32_t              maxCallResults    = 0;
    bool                 compilerGenerated = false;
    bool                 addedToList       = false;

    uint32_t          maxReservedRegisterRC = 0;
    vector<uint32_t>  availableRegistersRC;
    vector<uint32_t>  availableRegistersRC2;
    vector<Register*> registersRC;
    int32_t           curRC = -1;
};
