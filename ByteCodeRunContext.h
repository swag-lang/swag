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
    SourceFile*        sourceFile;
    AstNode*           node;
    bool               hasError = false;
    string             errorMsg;

    void error(const string& msg)
    {
        hasError = true;
        errorMsg = msg;
    }

    int32_t popS32()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(int32_t*) ref.addr : ref.reg.s32;
    }

    int64_t popS64()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(int64_t*) ref.addr : ref.reg.s64;
    }

    uint32_t popU32()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(uint32_t*) ref.addr : ref.reg.u32;
    }

    uint64_t popU64()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(uint64_t*) ref.addr : ref.reg.u64;
    }

    float popF32()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(float*) ref.addr : (float) ref.reg.f64;
    }

    double popF64()
    {
        auto& ref = stack[--sp];
        return ref.addr ? *(double*) ref.addr : ref.reg.f64;
    }

    void push(int32_t val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.s32 = val;
        ref.addr    = nullptr;
    }

    void push(int64_t val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.s64 = val;
        ref.addr    = nullptr;
    }

    void push(uint32_t val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.u32 = val;
        ref.addr    = nullptr;
    }

    void push(uint64_t val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.u64 = val;
        ref.addr    = nullptr;
    }

    void push(float val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.f64 = val;
        ref.addr    = nullptr;
    }

    void push(double val)
    {
        assert(sp != stack.size());
        auto& ref   = stack[sp++];
        ref.reg.f64 = val;
        ref.addr    = nullptr;
    }
};
