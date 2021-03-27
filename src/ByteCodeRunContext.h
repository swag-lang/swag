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
struct SwagCompilerSourceLocation;
struct JobContext;
struct ConcreteCompilerMessage;

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext : public JobContext
{
    ~ByteCodeRunContext();
    void setup(SourceFile* sf, AstNode* node);
    void error(const Utf8& msg, SwagCompilerSourceLocation* loc = nullptr);
    void addCallStack();

    template<typename T>
    inline T pop()
    {
        auto popResult = *(T*) sp;
        sp += sizeof(T);
        return popResult;
    }

    void stackOverflow();

    template<typename T>
    inline void push(const T& value)
    {
        if (sp - sizeof(T) < stack)
        {
            stackOverflow();
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
        if (sp - offset < stack)
        {
            stackOverflow();
            return;
        }

        sp -= offset;
    }

    VectorNative<ffi_type*>         ffiArgs;
    VectorNative<void*>             ffiArgsValues;
    Utf8                            errorMsg;
    vector<VectorNative<Register>*> registersRC;
    vector<uint32_t>                popParamsOnRet;
    vector<uint32_t>                returnRegOnRet;

    SwagCompilerSourceLocation* errorLoc      = nullptr;
    JobContext*                 callerContext = nullptr;

    AstNode*             node         = nullptr;
    uint8_t*             stack        = nullptr;
    uint8_t*             sp           = nullptr;
    uint8_t*             bp           = nullptr;
    ByteCode*            bc           = nullptr;
    ByteCodeInstruction* ip           = nullptr;
    static const int     MAX_ALLOC_RR = 2;
    Register*            registersRR  = nullptr;

    uint32_t stackSize = 0;
    int32_t  curRC     = -1;
    int32_t  firstRC   = -1;
    bool     hasError  = false;

    uint32_t             debugPrevCurRC = 0;
    ByteCodeInstruction* debugPrevIp    = nullptr;
    bool                 debugOn        = false;

    const ConcreteCompilerMessage* currentCompilerMessage = nullptr;
    Job*                           currentCompilerJob     = nullptr;
};
