#pragma once
#include "Register.h"
#include "Log.h"
#include "SourceFile.h"
#include "Job.h"
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

struct StackValue
{
    Register reg;
    void*    addr;
};

struct ByteCodeRunContext
{
    ~ByteCodeRunContext();

    void setup(SourceFile* sf, AstNode* node, ByteCode* nodebc);
    void releaseStack();
    void stackOverflow();
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

    // Keep 'ip' first to derefence it in the runner without an offset
    ByteCodeInstruction* ip                        = nullptr;
    Register*            curRegistersRC            = nullptr;
    static const int     MAX_ALLOC_RR              = 2;
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

    const ExportedCompilerMessage* currentCompilerMessage = nullptr;
    Job*                           currentCompilerJob     = nullptr;

    // Debugger
    enum class DebugStepMode
    {
        None,
        NextLine,
        NextLineStepOut,
        FinishedFunction,
        ToNextBreakpoint,
    };

    enum class DebugBkpType
    {
        FuncName,
        FileLine,
        InstructionIndex,
    };

    struct DebugBreakpoint
    {
        DebugBkpType type;
        Utf8         name;
        ByteCode*    bc           = nullptr;
        uint32_t     line         = 0;
        bool         disabled     = false;
        bool         autoDisabled = false;
        bool         autoRemove   = false;
    };

    ByteCodeInstruction*    debugLastIp           = nullptr;
    ByteCodeInstruction*    debugLastBreakIp      = nullptr;
    SourceFile*             debugStepLastFile     = nullptr;
    SourceLocation*         debugStepLastLocation = nullptr;
    AstNode*                debugStepLastFunc     = nullptr;
    ByteCode*               debugCxtBc            = nullptr;
    ByteCode*               debugLastBc           = nullptr;
    ByteCodeInstruction*    debugCxtIp            = nullptr;
    uint8_t*                debugCxtBp            = nullptr;
    uint8_t*                debugCxtSp            = nullptr;
    uint8_t*                debugCxtSpAlt         = nullptr;
    uint8_t*                debugCxtStack         = nullptr;
    uint32_t                debugLastCurRC        = 0;
    int32_t                 debugStepRC           = 0;
    DebugStepMode           debugStepMode         = DebugStepMode::None;
    uint32_t                debugStackFrameOffset = 0;
    uint32_t                debugCxtRc            = 0;
    bool                    debugOn;
    uint32_t                debugBcMode            = false;
    bool                    debugRaiseStart        = false;
    bool                    debugEntry             = false;
    bool                    debugAccept            = true;
    bool                    debugForcePrintContext = false;
    Vector<DebugBreakpoint> debugBreakpoints;
    Utf8                    debugLastLine;
};

extern bool g_Exiting;