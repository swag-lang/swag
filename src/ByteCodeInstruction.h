#pragma once
#include "Register.h"
#include "ByteCodeOp.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct ByteCode;
struct SourceLocation;

struct ByteCodeInstruction
{
    // Keep 'op' first to derefence it in the runner without an offset
    ByteCodeOp op;
    uint16_t   flags;
    uint32_t   padding;

    Register        a;
    Register        b;
    Register        c;
    Register        d;
    AstNode*        node;
    SourceLocation* location;

#ifdef SWAG_DEV_MODE
    const char* sourceFile;
    int         sourceLine;
    int         serial;
    int         treeNode;
    uint32_t    crc;
#endif
};
