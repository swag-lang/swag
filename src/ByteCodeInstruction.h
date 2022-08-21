#pragma once
#include "Register.h"
#include "ByteCodeOp.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct ByteCode;
struct SourceLocation;

struct ByteCodeInstruction
{
    Register        a;
    Register        b;
    Register        c;
    Register        d;
    AstNode*        node;
    SourceLocation* location;
    uint16_t        flags;
    ByteCodeOp      op;
#ifdef SWAG_DEV_MODE
    const char* sourceFile;
    int         sourceLine;
    int         serial;
    int         treeNode;
    uint32_t    crc;
#endif
};
