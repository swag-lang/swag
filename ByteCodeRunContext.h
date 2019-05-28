#pragma once
#include "ByteCode.h"
struct SemanticContext;
struct AstNode;
struct ByteCodeRunContext;
struct ConcatBucket;

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext
{
    ByteCode           bc;
    vector<StackValue> stack;
    int                sp;
    uint8_t*           ep;

    void push(int32_t val)
    {
        auto& ref   = stack[sp++];
        ref.reg.s32 = val;
        ref.addr    = nullptr;
    }
};
