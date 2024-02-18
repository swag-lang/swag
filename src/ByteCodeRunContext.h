#pragma once
#include "Job.h"
#include "Register.h"
#include "SourceFile.h"

struct SemanticContext;
struct AstNode;
struct ByteCodeRunContext;
struct ConcatBucket;
struct ByteCode;
struct ByteCodeInstruction;
struct SwagSourceCodeLocation;
struct JobContext;
struct ExportedCompilerMessage;
struct SourceLocation;
struct SymbolOverload;

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext
{
    ~ByteCodeRunContext();

    void        setup(SourceFile* sf, AstNode* nd, ByteCode* nodeBC);
    void        releaseStack();
    static void stackOverflow();
    uint32_t    getRegCount(uint32_t cur);

    template<typename T>
    T pop()
    {
        auto popResult = *reinterpret_cast<T*>(sp);
        sp += sizeof(T);
        return popResult;
    }

    template<typename T>
    T popAlt()
    {
        spAlt -= sizeof(T);
        return *reinterpret_cast<T*>(spAlt);
    }

    template<typename T>
    void push(const T& value)
    {
        if (sp - sizeof(T) < spAlt)
        {
            stackOverflow();
            return;
        }

        sp -= sizeof(T);
        *reinterpret_cast<T*>(sp) = value;
    }

    template<typename T>
    void pushAlt(const T& value)
    {
        if (spAlt + sizeof(T) >= sp)
        {
            stackOverflow();
            return;
        }

        *reinterpret_cast<T*>(spAlt) = value;
        spAlt += sizeof(T);
    }

    void incSP(uint32_t offset)
    {
        sp += offset;
    }

    void decSP(uint32_t offset)
    {
        if (sp - offset < stack)
        {
            stackOverflow();
            return;
        }

        sp -= offset;
    }

    Register* getRegBuffer(uint32_t cur)
    {
        return registers.buffer + registersRC[cur];
    }

    // Keep 'ip' first to dereference it in the runner without an offset
    ByteCodeInstruction* ip                        = nullptr;
    Register*            curRegistersRC            = nullptr;
    static constexpr int MAX_ALLOC_RR              = 2;
    Register             registersRR[MAX_ALLOC_RR] = {{0}, {0}};
    uint8_t*             stack                     = nullptr;
    uint8_t*             sp                        = nullptr;
    uint8_t*             spAlt                     = nullptr;
    uint8_t*             bp                        = nullptr;
    uint32_t             curRC                     = UINT32_MAX;
    uint32_t             firstRC                   = UINT32_MAX;
    uint32_t             maxRecurse                = 0;
    ByteCode*            bc                        = nullptr;

    JobContext jc;

    VectorNative<uint32_t> registersRC;
    VectorNative<Register> registers;

    VectorNative<uint32_t> ffiPushRAParam;

    JobContext* callerContext = nullptr;

    AstNode*  node  = nullptr;
    ByteCode* oldBc = nullptr;

    bool debugOnFirstError = false;
    bool sharedStack       = false;
    bool forConstExpr      = false;
    bool forDebugger       = false;

    const ExportedCompilerMessage* currentCompilerMessage = nullptr;
    Job*                           currentCompilerJob     = nullptr;
    SymbolOverload*                internalPanicSymbol    = nullptr;
    Utf8                           internalPanicHint;

    uint32_t debugStackFrameOffset = 0;
    bool     debugRaiseStart       = false;
    bool     debugOn               = false;
    bool     debugAccept           = true;
    bool     debugEntry            = false;
};

extern bool g_Exiting;
