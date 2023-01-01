#pragma once
#include "ByteCodeRunContext.h"
#include "Log.h"
#include "Os.h"

struct TypeInfo;
struct ComputedValue;
struct Utf8;
enum class CompilerAstKind;

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

    enum class CommandResult
    {
        Break,
        Continue,
        EvalDefault,
    };

#define CHECK_CMD_RESULT(__cmd)                 \
    auto __result = __cmd;                      \
    if (__result == CommandResult::Break)       \
        break;                                  \
    if (__result == CommandResult::EvalDefault) \
        goto evalDefault;

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

    static void appendTypedValue(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent, ValueFormat* fmt = nullptr);
    static void appendTypedValueProtected(ByteCodeRunContext* context, Utf8& str, const EvaluateResult& res, int indent, ValueFormat* fmt = nullptr);
    static void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);
    static void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr);

    static void printMemory(ByteCodeRunContext* context, const Utf8& arg);
    static void printWhere(ByteCodeRunContext* context);
    static void printSourceLines(SourceFile* file, SourceLocation* curLocation, int startLine, int endLine);
    static void printSourceLines(SourceFile* file, SourceLocation* curLocation, uint32_t offset = 3);
    static void printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1);

    static CommandResult cmdExecute(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static CommandResult cmdInfoFuncs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static CommandResult cmdInfoModules(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static CommandResult cmdInfoLocals(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static CommandResult cmdInfoRegs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);
    static CommandResult cmdInfoArgs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr);

    static bool getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN);
    static void printContextInstruction(ByteCodeRunContext* context, bool force = false);
    static void computeDebugContext(ByteCodeRunContext* context);
    static Utf8 completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete);
    static Utf8 getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift);
    static bool step(ByteCodeRunContext* context);

    static void printHelp();
};
