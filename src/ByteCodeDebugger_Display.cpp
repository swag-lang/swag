#include "pch.h"
#include "ByteCodeDebugger.h"
#include "Log.h"

BcDbgCommandResult ByteCodeDebugger::cmdDisplayAdd(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::BadArguments;

    const auto line = arg.cmd + " " + arg.cmdExpr;
    g_ByteCodeDebugger.display.push_back(line);
    g_ByteCodeDebugger.printDisplay(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplayClear(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 2)
    {
        if (g_ByteCodeDebugger.display.empty())
            printCmdError("no expression to remove");
        else
            printCmdResult(FMT("%d expression(s) have been removed", g_ByteCodeDebugger.display.size()));
        g_ByteCodeDebugger.display.clear();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= (int) g_ByteCodeDebugger.display.size())
        printCmdError("invalid expression number");
    else
    {
        g_ByteCodeDebugger.display.erase(g_ByteCodeDebugger.display.begin() + numB - 1);
        printCmdResult(FMT("expression #%d has been removed", numB));
    }

    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::printDisplayList() const
{
    if (display.empty())
    {
        printCmdError("no expression");
        return;
    }

    g_Log.setColor(LogColor::Gray);
    for (size_t i = 0; i < display.size(); i++)
    {
        g_Log.print(FMT("#%d: %s", i + 1, display[i].c_str()));
        g_Log.eol();
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplayPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    g_ByteCodeDebugger.printDisplayList();
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplay(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return cmdDisplayPrint(context, arg);
    if (arg.split[1] == "clear" || arg.split[1] == "cl")
        return cmdDisplayClear(context, arg);
    return cmdDisplayAdd(context, arg);
}

void ByteCodeDebugger::printDisplay(ByteCodeRunContext* context) const
{
    if (display.empty())
        return;

    for (const auto& line : display)
    {
        BcDbgCommandArg arg;
        tokenizeCommand(context, line, arg);
        cmdPrint(context, arg);
    }
}
