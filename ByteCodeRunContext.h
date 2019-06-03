#pragma once
#include "Register.h"
struct SemanticContext;
struct AstNode;
struct ByteCodeRunContext;
struct ConcatBucket;
struct SourceFile;
struct ByteCode;
struct ByteCodeInstruction;

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext
{
    ByteCode*                    bc = nullptr; // Current executed bytecode
    vector<ByteCode*>            stack_bc;
    vector<ByteCodeInstruction*> stack_ip;
    vector<StackValue>           stack_storage;
    int                          sp   = 0;
    ByteCodeInstruction*         ip   = nullptr; // Execution pointer inside current bytecode
    int                          epbc = 0;       // Execution pointer on bytecode array

    SourceFile* sourceFile;
    AstNode*    node;
    bool        hasError = false;
    string      errorMsg;

    void error(const string& msg)
    {
        hasError = true;
        errorMsg = msg;
    }

    bool popBool()
    {
        auto& ref = stack_storage[--sp];
        return *(bool*) ref.addr;
    }

    int32_t popS32()
    {
        auto& ref = stack_storage[--sp];
        return *(int32_t*) ref.addr;
    }

    int64_t popS64()
    {
        auto& ref = stack_storage[--sp];
        return *(int64_t*) ref.addr;
    }

    uint32_t popU32()
    {
        auto& ref = stack_storage[--sp];
        return *(uint32_t*) ref.addr;
    }

    uint64_t popU64()
    {
        auto& ref = stack_storage[--sp];
        return *(uint64_t*) ref.addr;
    }

    float popF32()
    {
        auto& ref = stack_storage[--sp];
        return *(float*) ref.addr;
    }

    double popF64()
    {
        auto& ref = stack_storage[--sp];
        return *(double*) ref.addr;
    }

    void push(bool val)
    {
        assert(sp != stack_storage.size());
        auto& ref = stack_storage[sp++];
        ref.reg.b = val;
        ref.addr  = &ref.reg.b;
    }

    void push(int32_t val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.s32 = val;
        ref.addr    = &ref.reg.s32;
    }

    void push(int64_t val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.s64 = val;
        ref.addr    = &ref.reg.s64;
    }

    void push(uint32_t val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.u32 = val;
        ref.addr    = &ref.reg.u32;
    }

    void push(uint64_t val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.u64 = val;
        ref.addr    = &ref.reg.u64;
    }

    void push(float val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.f64 = val;
        ref.addr    = &ref.reg.f64;
    }

    void push(double val)
    {
        assert(sp != stack_storage.size());
        auto& ref   = stack_storage[sp++];
        ref.reg.f64 = val;
        ref.addr    = &ref.reg.f64;
    }
};
