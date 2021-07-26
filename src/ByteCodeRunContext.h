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
struct SourceLocation;

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
    vector<uint64_t>                returnRegOnRetRR;

    SwagCompilerSourceLocation* errorLoc      = nullptr;
    JobContext*                 callerContext = nullptr;
    bool                        canCatchError = false;

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

    enum class DebugStepMode
    {
        None,
        NextLine,
        NextLineStepOut,
        FinishedFunction,
    };

    const ConcreteCompilerMessage* currentCompilerMessage = nullptr;
    Job*                           currentCompilerJob     = nullptr;

    // Debugger
    bool                 debugEntry            = false;
    bool                 debugOn               = false;
    uint32_t             debugLastCurRC        = 0;
    ByteCodeInstruction* debugLastIp           = nullptr;
    int32_t              debugStepRC           = 0;
    DebugStepMode        debugStepMode         = DebugStepMode::None;
    SourceFile*          debugStepLastFile     = nullptr;
    SourceLocation*      debugStepLastLocation = nullptr;
    uint32_t             debugStackFrameOffset = 0;
    ByteCode*            debugCxtBc            = nullptr;
    uint32_t             debugCxtRc            = 0;
    ByteCodeInstruction* debugCxtIp            = nullptr;
    uint8_t*             debugCxtBp            = nullptr;
};

extern bool g_Exiting;