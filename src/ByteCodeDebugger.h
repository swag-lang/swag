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

    function<BcDbgCommandResult(ByteCodeRunContext*, const vector<Utf8>&, const Utf8&)> cb;
};

struct ByteCodeDebugger
{
    static constexpr const char* COLOR_TYPE    = Log::VDarkCyan;
    static constexpr const char* COLOR_NAME    = Log::VDarkYellow;
    static constexpr const char* COLOR_DEFAULT = Log::VDarkWhite;

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

    static bool evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent = false);
    static bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent = false);

    static bool getValueFormat(const Utf8& cmd, ValueFormat& fmt);

    static void printBreakpoints(ByteCodeRunContext* context);
    static void checkBreakpoints(ByteCodeRunContext* context);
    static bool addBreakpoint(ByteCodeRunContext* context, const ByteCodeRunContext::DebugBreakpoint& bkp);

    static void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    static void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent);
    static void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    static void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    static void printSourceLines(SourceFile* file, SourceLocation* curLocation, int startLine, int endLine);
    static void printSourceLines(SourceFile* file, SourceLocation* curLocation, uint32_t offset = 3);
    static void printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1);

    static BcDbgCommandResult cmdBackTrace(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrame(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrameUp(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFrameDown(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdExecute(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdPrint(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdStep(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdNext(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdFinish(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdContinue(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdJump(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdUntil(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdMemory(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdWhere(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInstruction(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInstructionDump(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdList(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdLongList(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBcMode(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdQuit(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdHelp(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreak(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfo(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);

    static BcDbgCommandResult cmdInfoFuncs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoModules(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoLocals(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoRegs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdInfoArgs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakEnable(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakDisable(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakClear(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakPrint(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakFunc(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakLine(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdBreakFileLine(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static BcDbgCommandResult cmdEmpty(ByteCodeRunContext* context, bool shift, const vector<Utf8>& cmds, const Utf8& cmdExpr);

    static void setup();
    static bool getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN);
    static void printDebugContext(ByteCodeRunContext* context, bool force = false);
    static void computeDebugContext(ByteCodeRunContext* context);
    static Utf8 completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete);
    static Utf8 getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift);
    static bool processCommandLine(ByteCodeRunContext* context, vector<Utf8>& cmds, Utf8& line, Utf8& cmdExpr);
    static bool mustBreak(ByteCodeRunContext* context);
    static bool step(ByteCodeRunContext* context);

    static void printHelp();
    static void printHelp(const BcDbgCommand& cmd);

    static vector<BcDbgCommand> commands;
};
