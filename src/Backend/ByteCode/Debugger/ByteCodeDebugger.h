#pragma once
#include "Backend/ByteCode/Run/ByteCodeRunContext.h"
#include "Os/Os.h"

struct TypeInfo;
struct ComputedValue;
struct Utf8;
enum class CompilerAstKind;

enum class BcDbgCommandResult
{
    Invalid,
    Break,
    Continue,
    BadArguments,
    Return,
};

struct BcDbgCommandArg
{
    Utf8         cmd;
    Utf8         cmdExpr;
    Vector<Utf8> split;
    bool         help = false;
};

using BcDbgCommandCB = std::function<BcDbgCommandResult(ByteCodeRunContext*, const BcDbgCommandArg&)>;
struct BcDbgCommand
{
    const char* name;
    const char* shortname;
    const char* args;
    const char* help;

    BcDbgCommandCB cb;
};

struct ByteCodeDebugger
{
    struct EvaluateResult
    {
        TypeInfo*      type  = nullptr;
        void*          addr  = nullptr;
        ComputedValue* value = nullptr;
        void*          storage[2];
    };

    struct ValueFormat
    {
        int  bitCount = 8;
        bool isSigned = false;
        bool isFloat  = false;
        bool isHexa   = true;
        bool print0X  = true;
    };

    enum class DebugStepMode
    {
        None,
        NextLineStepIn,
        NextLineStepOut,
        NextInstructionStepIn,
        NextInstructionStepOut,
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

    template<typename T>
    static T getAddrValue(const void* addr)
    {
        SWAG_TRY
        {
            return *static_cast<const T*>(addr);
        }
        SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
        {
            return 0;
        }
    }

    bool evalDynExpression(ByteCodeRunContext* context, const Utf8& inExpr, EvaluateResult& res, CompilerAstKind kind, bool silent = false) const;
    bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent = false) const;

    static bool getValueFormat(const Utf8& cmd, ValueFormat& fmt);

    void printBreakpoints(ByteCodeRunContext* context) const;
    void checkBreakpoints(ByteCodeRunContext* context);
    bool addBreakpoint(ByteCodeRunContext* context, const DebugBreakpoint& bkp);

    static void appendTypedValue(ByteCodeRunContext* context, const Utf8& filter, const AstNode* node, uint8_t* baseAddr, uint8_t* realAddr, Utf8& result);
    static void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, uint32_t level, uint32_t indent);
    static void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, uint32_t level, uint32_t indent);
    static void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    static void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    void printSourceLines(const ByteCodeRunContext* context, const ByteCode* bc, SourceFile* file, const SourceLocation* curLocation, int startLine, int endLine) const;
    void printSourceLines(const ByteCodeRunContext* context, const ByteCode* bc, SourceFile* file, const SourceLocation* curLocation, uint32_t offset = 3) const;
    void printInstructions(const ByteCodeRunContext* context, const ByteCode* bc, const ByteCodeInstruction* ip, int num = 1) const;

    static BcDbgCommandResult cmdBackTrace(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdFrame(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdFrameUp(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdFrameDown(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdExecute(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdDisplayPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdDisplayAdd(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdDisplayClear(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdDisplay(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdStep(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdNext(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdFinish(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdContinue(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdJump(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdUntil(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdMemory(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdWhere(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInstruction(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInstructionDump(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdList(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdLongList(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdSet(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdQuit(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdHelp(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreak(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfo(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBc(ByteCodeRunContext* context, const BcDbgCommandArg& arg);

    static BcDbgCommandResult cmdInfoFunctions(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoModules(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoGlobals(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoExpressions(ByteCodeRunContext* context, const BcDbgCommandArg& arg);

    static BcDbgCommandResult cmdBreakEnable(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakDisable(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakClear(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakFunc(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakFileLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg);

    bool                           commandSubstitution(ByteCodeRunContext* context, Utf8& cmdExpr) const;
    static void                    tokenizeCommand(const Utf8& line, BcDbgCommandArg& arg);
    void                           setup();
    static void                    printLong(const Utf8& all);
    static void                    printLong(const Vector<Utf8>& all);
    static void                    printSeparator();
    bool                           getRegIdx(ByteCodeRunContext* context, const Utf8& arg, uint32_t& regN) const;
    static Utf8                    printSymbols(ByteCodeRunContext* context, const Utf8& filter, uint8_t* baseAddr, const VectorNative<AstNode*>& nodes, const VectorNative<uint8_t*>& addrs);
    void                           printDebugContext(ByteCodeRunContext* context, bool force = false);
    void                           computeDebugContext(ByteCodeRunContext* context);
    Utf8                           completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete) const;
    Utf8                           getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift) const;
    bool                           mustBreak(ByteCodeRunContext* context);
    bool                           step(ByteCodeRunContext* context);
    static ByteCode*               findCmdBc(const Utf8& name);
    static VectorNative<ByteCode*> findBc(const char* bcName);
    static bool                    testNameFilter(const Utf8& name, const Utf8& filter);

    static void printTitleNameType(const Utf8& title, const Utf8& name, const Utf8& type);
    static void printCmdError(const Utf8& msg);
    static void printCmdResult(const Utf8& msg);
    static void printMsgBkp(const Utf8& msg);
    void        printHelp() const;
    static void printHelp(const BcDbgCommand& cmd);
    void        printDisplayList() const;
    void        printDisplay(ByteCodeRunContext* context) const;
    static void printSet(ByteCodeRunContext* context);

    Vector<BcDbgCommand>    commands;
    Vector<DebugBreakpoint> breakpoints;
    Vector<Utf8>            display;
    Vector<Utf8>            evalExpr;
    Vector<Utf8>            evalExprResult;
    Utf8                    lastLine;

    ByteCodeInstruction* lastIp           = nullptr;
    ByteCodeInstruction* lastBreakIp      = nullptr;
    SourceFile*          stepLastFile     = nullptr;
    SourceLocation*      stepLastLocation = nullptr;
    AstNode*             stepLastFunc     = nullptr;
    ByteCode*            cxtBc            = nullptr;
    ByteCode*            lastBc           = nullptr;
    ByteCodeInstruction* cxtIp            = nullptr;
    uint8_t*             cxtBp            = nullptr;
    uint8_t*             cxtSp            = nullptr;
    uint8_t*             cxtSpAlt         = nullptr;
    uint8_t*             cxtStack         = nullptr;

    uint32_t      lastCurRc = 0;
    uint32_t      stepRc    = 0;
    DebugStepMode stepMode  = DebugStepMode::None;
    uint32_t      cxtRc     = 0;
    uint32_t      stepCount = 0;
    uint32_t      bcMode    = false;

    bool forcePrintContext = false;
    bool printStruct       = true;
    bool printArray        = true;
    bool printBcCode       = true;
};

extern ByteCodeDebugger g_ByteCodeDebugger;
