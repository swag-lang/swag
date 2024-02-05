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

    void        setup(SourceFile* sf, AstNode* node, ByteCode* nodeBC);
    void        releaseStack();
    static void stackOverflow();
    int         getRegCount(int cur);

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

    // Keep 'ip' first to derefence it in the runner without an offset
    ByteCodeInstruction* ip                        = nullptr;
    Register*            curRegistersRC            = nullptr;
    static constexpr int MAX_ALLOC_RR              = 2;
    Register             registersRR[MAX_ALLOC_RR] = {{0}, {0}};
    uint8_t*             stack                     = nullptr;
    uint8_t*             sp                        = nullptr;
    uint8_t*             spAlt                     = nullptr;
    uint8_t*             bp                        = nullptr;
    int                  curRC                     = -1;
    int                  firstRC                   = -1;
    int                  maxRecurse                = 0;
    ByteCode*            bc                        = nullptr;

    JobContext jc;

    VectorNative<int>      registersRC;
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
