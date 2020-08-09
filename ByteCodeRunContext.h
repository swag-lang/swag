#pragma once
#include "Register.h"
#include "Log.h"
#include "SourceFile.h"
#include "Job.h"
#include "ffi/ffi.h"
struct SemanticContext;
struct AstNode;
struct ByteCodeRunContext;
struct ConcatBucket;
struct ByteCode;
struct ByteCodeInstruction;

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext : public JobContext
{
    void setup(SourceFile* sf, AstNode* node, uint32_t numRR, uint32_t stackS);
    void error(const Utf8& msg);

    template<typename T>
    inline T pop()
    {
        auto popResult = *(T*) sp;
        sp += sizeof(T);
        return popResult;
    }

    template<typename T>
    inline void push(const T& value)
    {
        if (sp - sizeof(T) < stack)
        {
            hasError = true;
            errorMsg = "stack overflow during bytecode execution";
            return;
        }

        sp -= sizeof(T);
        *(T*) sp = value;
    }

    inline void incSP(uint32_t offset)
    {
        sp += offset;
    }

    inline void decSP(uint32_t offset)
    {
        sp -= offset;
    }

    VectorNative<ffi_type*> ffiArgs;
    VectorNative<void*>     ffiArgsValues;
    Utf8                    errorMsg;

    AstNode*             node        = nullptr;
    uint8_t*             stack       = nullptr;
    uint8_t*             sp          = nullptr;
    uint8_t*             bp          = nullptr;
    ByteCode*            bc          = nullptr;
    ByteCodeInstruction* ip          = nullptr;
    Register*            registersRR = nullptr;

    uint32_t stackSize      = 0;
    uint32_t numRegistersRR = 0;

    bool hasError = false;
};
