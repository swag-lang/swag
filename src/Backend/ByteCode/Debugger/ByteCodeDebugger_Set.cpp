#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Syntax/ComputedValue.h"

namespace
{
    BcDbgCommandResult setOnOff(const Utf8& msg, const BcDbgCommandArg& arg, bool& val)
    {
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split.size() == 3)
        {
            val = !val;
        }
        else if (arg.split[3] != "on" && arg.split[3] != "off")
        {
            ByteCodeDebugger::printCmdError(form("invalid on/off argument [[%s]]", arg.split[3].cstr()));
            return BcDbgCommandResult::Error;
        }
        else
        {
            val = arg.split[3] == "on";
        }

        g_Log.print(msg, LogColor::Gray);
        if (val)
            ByteCodeDebugger::printCmdResult(": on");
        else
            ByteCodeDebugger::printCmdResult(": off");
        return BcDbgCommandResult::Continue;
    }
}

void ByteCodeDebugger::printSet(ByteCodeRunContext*)
{
    g_Log.messageHeaderDot("stop on @breakpoint()", g_CommandLine.dbgStopOnBreakpoint ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print struct content", g_ByteCodeDebugger.printStruct ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print array content", g_ByteCodeDebugger.printArray ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print eval bytecode", g_ByteCodeDebugger.printEvalBc ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print compiler symbols", g_ByteCodeDebugger.printCompilerSymbols ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("bytecode source code", g_ByteCodeDebugger.printBcCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("backtrace source code", g_ByteCodeDebugger.printBtCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
}

BcDbgCommandResult ByteCodeDebugger::cmdSetRegister(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;

    if (arg.split[2][0] != '$')
    {
        printCmdError(form("invalid register [[%s]]", arg.split[2].cstr()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[2] == "$sp")
    {
        printCmdError("cannot modify stack address [[$sp]]");
        return BcDbgCommandResult::Error;
    }

    if (arg.split[2] == "$bp")
    {
        printCmdError("cannot modify base address [[$bp]]");
        return BcDbgCommandResult::Error;
    }

    uint32_t regN;
    if (!g_ByteCodeDebugger.getRegIdx(context, arg.split[2].cstr() + 1, regN))
        return BcDbgCommandResult::Error;

    Utf8 expr;
    for (uint32_t i = 3; i < arg.split.size(); i++)
        expr += arg.split[i];
    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    if (!res.value)
    {
        printCmdError("cannot change register with that expression");
        return BcDbgCommandResult::Error;
    }

    auto& regP = context->getRegBuffer(g_ByteCodeDebugger.cxtRc)[regN];
    regP       = res.value->reg;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdSet(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() == 1)
    {
        g_ByteCodeDebugger.printSet(context);
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "bkp")
    {
        return setOnOff("stop on @breakpoint()", arg, g_CommandLine.dbgStopOnBreakpoint);
    }

    if (arg.split[1] == "print")
    {
        if (arg.split.size() < 3)
            return BcDbgCommandResult::NotEnoughArguments;
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split[2] == "struct")
            return setOnOff("print struct content", arg, g_ByteCodeDebugger.printStruct);
        if (arg.split[2] == "array")
            return setOnOff("print array content", arg, g_ByteCodeDebugger.printArray);
        if (arg.split[2] == "eval")
            return setOnOff("print eval expression bytecode", arg, g_ByteCodeDebugger.printEvalBc);
        if (arg.split[2] == "compsym")
            return setOnOff("print compiler symbols", arg, g_ByteCodeDebugger.printCompilerSymbols);

        printCmdError(form("invalid [[set print]] argument [[%s]]", arg.split[2].cstr()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[1] == "bytecode" || arg.split[1] == "bc")
    {
        if (arg.split.size() < 3)
            return BcDbgCommandResult::NotEnoughArguments;
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split[2] == "source")
            return setOnOff("bytecode source code", arg, g_ByteCodeDebugger.printBcCode);

        printCmdError(form("invalid [[set bytecode]] argument [[%s]]", arg.split[2].cstr()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[1] == "backtrace" || arg.split[1] == "bt")
    {
        if (arg.split.size() < 3)
            return BcDbgCommandResult::NotEnoughArguments;
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split[2] == "source")
            return setOnOff("bytecode source code", arg, g_ByteCodeDebugger.printBtCode);

        printCmdError(form("invalid [[set backtrace]] argument [[%s]]", arg.split[2].cstr()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[1] == "register" || arg.split[1] == "reg")
        return cmdSetRegister(context, arg);

    printCmdError(form("invalid [[set]] command [[%s]]", arg.split[1].cstr()));
    return BcDbgCommandResult::Error;
}
