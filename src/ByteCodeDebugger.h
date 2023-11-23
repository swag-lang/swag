#pragma once
#include "ByteCodeRunContext.h"
#include "Log.h"
#include "Os.h"

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
};

struct BcDbgCommand
{
    const char* name;
    const char* shortname;
    const char* args;
    const char* help;

    function<BcDbgCommandResult(ByteCodeRunContext*, const BcDbgCommandArg&)> cb;
};

struct ByteCodeDebugger
{
    static const LogColor COLOR_BREAKPOINT      = LogColor::Red;
    static const LogColor COLOR_CUR_INSTRUCTION = LogColor::Red;
    static const LogColor COLOR_NAME            = LogColor::DarkYellow;
    static const LogColor COLOR_TYPE            = LogColor::DarkCyan;
    static const LogColor COLOR_LOCATION        = LogColor::DarkMagenta;
    static const LogColor COLOR_INDEX           = LogColor::Cyan;
    static const LogColor COLOR_DEFAULT         = LogColor::Gray;

    static constexpr const char* COLOR_VTS_CUR_INSTRUCTION = Log::colorToVTS(COLOR_CUR_INSTRUCTION);
    static constexpr const char* COLOR_VTS_NAME            = Log::colorToVTS(COLOR_NAME);
    static constexpr const char* COLOR_VTS_TYPE            = Log::colorToVTS(COLOR_TYPE);
    static constexpr const char* COLOR_VTS_LOCATION        = Log::colorToVTS(COLOR_LOCATION);
    static constexpr const char* COLOR_VTS_INDEX           = Log::colorToVTS(COLOR_INDEX);
    static constexpr const char* COLOR_VTS_DEFAULT         = Log::colorToVTS(COLOR_DEFAULT);
    static constexpr const char* COLOR_VTS_BREAKPOINT      = Log::colorToVTS(COLOR_BREAKPOINT);
    static const int             LINE_W                    = 71;

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
        bool print0x  = true;
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
            return *(T*) addr;
        }
        SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
        {
            return 0;
        }
    }

    bool evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent = false);
    bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent = false);

    bool getValueFormat(const Utf8& cmd, ValueFormat& fmt);

    void printBreakpoints(ByteCodeRunContext* context);
    void checkBreakpoints(ByteCodeRunContext* context);
    bool addBreakpoint(ByteCodeRunContext* context, const DebugBreakpoint& bkp);

    void appendTypedValue(ByteCodeRunContext* context, const Utf8& filter, AstNode* node, uint8_t* baseAddr, uint8_t* realAddr, Utf8& result);
    void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    void printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, int startLine, int endLine);
    void printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, uint32_t offset = 3);
    void printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1);

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
    static BcDbgCommandResult cmdStepi(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdNext(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdNexti(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdFinish(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdContinue(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdJump(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdJumpi(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdUntil(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdUntili(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
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

    static BcDbgCommandResult cmdInfoFuncs(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoModules(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdInfoVariables(ByteCodeRunContext* context, const BcDbgCommandArg& arg);

    static BcDbgCommandResult cmdBreakEnable(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakDisable(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakClear(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakFunc(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg);
    static BcDbgCommandResult cmdBreakFileLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg);

    void                    commandSubstitution(ByteCodeRunContext* context, Utf8& cmdExpr);
    void                    tokenizeCommand(ByteCodeRunContext* context, const Utf8& line, BcDbgCommandArg& arg);
    void                    setup();
    void                    printLong(const Utf8& all);
    void                    printLong(const Vector<Utf8>& all);
    void                    printSeparator();
    bool                    getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN);
    void                    printDebugContext(ByteCodeRunContext* context, bool force = false);
    void                    computeDebugContext(ByteCodeRunContext* context);
    Utf8                    completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete);
    Utf8                    getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift);
    bool                    mustBreak(ByteCodeRunContext* context);
    bool                    step(ByteCodeRunContext* context);
    ByteCode*               findCmdBc(const Utf8& name);
    VectorNative<ByteCode*> findBc(const char* name);
    bool                    testNameFilter(const Utf8& name, const Utf8& filter);

    void printTitleNameType(const Utf8& title, const Utf8& name, const Utf8& type);
    void printCmdError(const Utf8& msg);
    void printCmdResult(const Utf8& msg);
    void printMsgBkp(const Utf8& msg);
    void printHelp();
    void printHelp(const BcDbgCommand& cmd);
    void printDisplayList();
    void printDisplay(ByteCodeRunContext* context);
    void printSet(ByteCodeRunContext* context);

    Vector<BcDbgCommand> commands;

    Vector<DebugBreakpoint> debugBreakpoints;
    Vector<Utf8>            debugDisplay;
    Utf8                    debugLastLine;
    ByteCodeInstruction*    debugLastIp            = nullptr;
    ByteCodeInstruction*    debugLastBreakIp       = nullptr;
    SourceFile*             debugStepLastFile      = nullptr;
    SourceLocation*         debugStepLastLocation  = nullptr;
    AstNode*                debugStepLastFunc      = nullptr;
    ByteCode*               debugCxtBc             = nullptr;
    ByteCode*               debugLastBc            = nullptr;
    ByteCodeInstruction*    debugCxtIp             = nullptr;
    uint8_t*                debugCxtBp             = nullptr;
    uint8_t*                debugCxtSp             = nullptr;
    uint8_t*                debugCxtSpAlt          = nullptr;
    uint8_t*                debugCxtStack          = nullptr;
    uint32_t                debugLastCurRC         = 0;
    int32_t                 debugStepRC            = 0;
    DebugStepMode           debugStepMode          = DebugStepMode::None;
    uint32_t                debugCxtRc             = 0;
    int                     debugStepCount         = 0;
    uint32_t                debugBcMode            = false;
    bool                    debugForcePrintContext = false;
    bool                    debugPrintStruct       = true;
    bool                    debugPrintArray        = true;
};

extern ByteCodeDebugger g_ByteCodeDebugger;