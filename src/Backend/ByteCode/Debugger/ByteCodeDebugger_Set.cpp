#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

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
            ByteCodeDebugger::printCmdError(form("invalid on/off argument [[%s]]", arg.split[3].c_str()));
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
    g_Log.messageHeaderDot("bytecode source code", g_ByteCodeDebugger.printBcCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("backtrace source code", g_ByteCodeDebugger.printBtCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
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

        printCmdError(form("invalid [[set print]] argument [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[1] == "bytecode" || arg.split[1] == "bc")
    {
        if (arg.split.size() < 3)
            return BcDbgCommandResult::NotEnoughArguments;
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split[2] == "code")
            return setOnOff("bytecode source code", arg, g_ByteCodeDebugger.printBcCode);

        printCmdError(form("invalid [[set bytecode]] argument [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    if (arg.split[1] == "backtrace" || arg.split[1] == "bt")
    {
        if (arg.split.size() < 3)
            return BcDbgCommandResult::NotEnoughArguments;
        if (arg.split.size() > 4)
            return BcDbgCommandResult::TooManyArguments;

        if (arg.split[2] == "code")
            return setOnOff("bytecode source code", arg, g_ByteCodeDebugger.printBtCode);

        printCmdError(form("invalid [[set backtrace]] argument [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    printCmdError(form("invalid [[set]] command [[%s]]", arg.split[1].c_str()));
    return BcDbgCommandResult::BadArguments;
}

BcDbgCommandResult ByteCodeDebugger::cmdBc(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    g_ByteCodeDebugger.bcMode = !g_ByteCodeDebugger.bcMode;
    if (g_ByteCodeDebugger.bcMode)
        printCmdResult("bytecode mode");
    else
        printCmdResult("source code mode");
    return BcDbgCommandResult::Continue;
}
