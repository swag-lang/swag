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
    Register* registersRC    = nullptr;
    uint32_t  numRegistersRC = 0;
    Register* registersRR    = nullptr;
    uint32_t  numRegistersRR = 0;

    uint8_t*             stack     = nullptr;
    uint8_t*             sp        = nullptr;
    uint8_t*             bp        = nullptr;
    ByteCode*            bc        = nullptr;
    uint32_t             stackSize = 0;
    ByteCodeInstruction* ip        = nullptr;

    SourceFile* sourceFile = nullptr;
    AstNode*    node       = nullptr;
    bool        hasError   = false;
    string      errorMsg;

    void setup(uint32_t numRC, uint32_t numRR, uint32_t stackS);

    void error(const string& msg)
    {
        hasError = true;
        errorMsg = msg;
    }

    template<typename T>
    T pop()
    {
        assert(sp + sizeof(T) <= bp);
        auto result = *(T*) sp;
        sp += sizeof(T);
        return result;
    }

    template<typename T>
    void push(const T& value)
    {
        assert(sp - sizeof(T) > stack);
        sp -= sizeof(T);
        *(T*) sp = value;
    }
};
