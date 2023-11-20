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

struct BcDbgCommand
{
    const char* name;
    const char* shortname;
    const char* args;
    const char* help;

    function<BcDbgCommandResult(ByteCodeRunContext*, const Vector<Utf8>&, const Utf8&)> cb;
};

struct ByteCodeDebugger
{
    static const LogColor COLOR_CUR_INSTRUCTION = LogColor::Red;
    static const LogColor COLOR_SRC_NAME        = LogColor::DarkMagenta;
    static const LogColor COLOR_TYPE            = LogColor::DarkCyan;

    static constexpr const char* COLOR_VTS_TYPE    = Log::colorToVTS(LogColor::DarkCyan);
    static constexpr const char* COLOR_VTS_NAME    = Log::colorToVTS(LogColor::DarkYellow);
    static constexpr const char* COLOR_VTS_DEFAULT = Log::colorToVTS(LogColor::Gray);
    static const int             LINE_W            = 71;

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

    void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    void printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, int startLine, int endLine);
    void printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, uint32_t offset = 3);
    void printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1);

    static BcDbgCommandResult cmdBackTrace(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrame(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrameUp(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrameDown(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdExecute(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdPrint(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdStep(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdStepi(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdNext(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdNexti(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFinish(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdContinue(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdJump(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdJumpi(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdUntil(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdUntili(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdMemory(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdWhere(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInstruction(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInstructionDump(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdList(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdLongList(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdMode(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdQuit(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdHelp(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreak(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfo(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);

    static BcDbgCommandResult cmdInfoFuncs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoModules(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoLocals(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoRegs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoArgs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakEnable(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakDisable(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakClear(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakPrint(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakFunc(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakLine(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakFileLine(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr);

    Utf8 getByteCodeName(ByteCode* bc);
    Utf8 getByteCodeFileName(ByteCode* bc);
    void setup();
    void printSeparator();
    bool getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN);
    void printDebugContext(ByteCodeRunContext* context, bool force = false);
    void computeDebugContext(ByteCodeRunContext* context);
    Utf8 completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete);
    Utf8 getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift);
    bool processCommandLine(ByteCodeRunContext* context, Vector<Utf8>& cmds, Utf8& line, Utf8& cmdExpr);
    bool mustBreak(ByteCodeRunContext* context);
    bool step(ByteCodeRunContext* context);

    void printTitleNameType(const Utf8& title, const Utf8& name, const Utf8& type);
    void printCmdError(const Utf8& msg);
    void printCmdResult(const Utf8& msg);
    void printMsgBkp(const Utf8& msg);
    void printHelp();
    void printHelp(const BcDbgCommand& cmd);

    Vector<BcDbgCommand> commands;

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
    uint32_t                debugBcMode            = false;
    bool                    debugForcePrintContext = false;
    Vector<DebugBreakpoint> debugBreakpoints;
    Utf8                    debugLastLine;
};

extern ByteCodeDebugger g_ByteCodeDebugger;