#pragma once
#include "Register.h"
#include "Log.h"
#include "SourceFile.h"
#include "Module.h"
#include "Job.h"
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

struct ByteCodeRunContext: public JobContext
{
    Register* registersRR    = nullptr;
    uint32_t  numRegistersRR = 0;

    uint8_t*             stack     = nullptr;
    uint8_t*             sp        = nullptr;
    uint8_t*             bp        = nullptr;
    ByteCode*            bc        = nullptr;
    uint32_t             stackSize = 0;
    ByteCodeInstruction* ip        = nullptr;

    AstNode*    node       = nullptr;
    bool        hasError   = false;
    Utf8        errorMsg;

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
            errorMsg = format("stack overflow during bytecode execution (stack size is '--bc-stack-size:%d' bytes)", sourceFile->module->buildParameters.buildCfg->byteCodeStackSize);
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
};
