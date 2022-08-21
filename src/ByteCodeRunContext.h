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

    void setup(SourceFile* sf, AstNode* node, ByteCode* nodebc);
    void releaseStack();
    void stackOverflow();
    void raiseError(const char* msg, SwagCompilerSourceLocation* loc = nullptr);
    int  getRegCount(int cur);

    template<typename T>
    inline T pop()
    {
        auto popResult = *(T*) sp;
        sp += sizeof(T);
        return popResult;
    }

    template<typename T>
    inline T popAlt()
    {
        spAlt -= sizeof(T);
        return *(T*) spAlt;
    }

    template<typename T>
    inline void push(const T& value)
    {
        if (sp - sizeof(T) < spAlt)
        {
            stackOverflow();
            return;
        }

        sp -= sizeof(T);
        *(T*) sp = value;
    }

    template<typename T>
    inline void pushAlt(const T& value)
    {
        if (spAlt + sizeof(T) >= sp)
        {
            stackOverflow();
            return;
        }

        *(T*) spAlt = value;
        spAlt += sizeof(T);
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

    inline Register* getRegBuffer(int cur)
    {
        return registers.buffer + registersRC[cur];
    }

    VectorNative<ffi_type*> ffiArgs;
    VectorNative<void*>     ffiArgsValues;
    Utf8                    errorMsg;
    VectorNative<int>       registersRC;
    VectorNative<Register>  registers;

    bool      ffi_StructByCopyDone = false;
    ffi_type  ffi_StructByCopy1;
    ffi_type* ffi_StructByCopy1T[1];
    ffi_type  ffi_StructByCopy2;
    ffi_type* ffi_StructByCopy2T[1];
    ffi_type  ffi_StructByCopy4;
    ffi_type* ffi_StructByCopy4T[1];
    ffi_type  ffi_StructByCopy8;
    ffi_type* ffi_StructByCopy8T[1];

    SwagCompilerSourceLocation* errorLoc      = nullptr;
    JobContext*                 callerContext = nullptr;

    bool canCatchError = false;

    AstNode*             node           = nullptr;
    uint8_t*             stack          = nullptr;
    uint8_t*             sp             = nullptr;
    uint8_t*             spAlt          = nullptr;
    uint8_t*             bp             = nullptr;
    ByteCode*            bc             = nullptr;
    ByteCodeInstruction* ip             = nullptr;
    static const int     MAX_ALLOC_RR   = 2;
    Register*            registersRR    = nullptr;
    Register*            curRegistersRC = nullptr;

    int  curRC    = -1;
    int  firstRC  = -1;
    bool hasError = false;

    const ConcreteCompilerMessage* currentCompilerMessage = nullptr;
    Job*                           currentCompilerJob     = nullptr;

    // Debugger
    enum class DebugStepMode
    {
        None,
        NextLine,
        NextLineStepOut,
        FinishedFunction,
    };

    bool raiseDebugStart = false;

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