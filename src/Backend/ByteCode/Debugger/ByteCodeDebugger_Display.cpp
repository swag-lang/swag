#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

BcDbgCommandResult ByteCodeDebugger::cmdDisplayAdd(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    const auto line = arg.cmd + " " + arg.cmdExpr;
    g_ByteCodeDebugger.display.push_back(line);
    g_ByteCodeDebugger.printDisplayExpressions(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplayClear(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() == 2)
    {
        if (g_ByteCodeDebugger.display.empty())
            printCmdError("no expression to remove");
        else
            printCmdResult(form("%d expression(s) have been removed", g_ByteCodeDebugger.display.size()));
        g_ByteCodeDebugger.display.clear();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[2].c_str()))
    {
        printCmdError(form("invalid expression number [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.display.size()))
    {
        printCmdError(form("expression number [[%d]] does not exist", numB));
        return BcDbgCommandResult::Error;
    }

    g_ByteCodeDebugger.display.erase(g_ByteCodeDebugger.display.begin() + numB - 1);
    printCmdResult(form("expression #%d has been removed", numB));
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
    for (uint32_t i = 0; i < display.size(); i++)
    {
        g_Log.print(form("#%d: %s", i + 1, display[i].c_str()));
        g_Log.writeEol();
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplayPrint(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    g_ByteCodeDebugger.printDisplayList();
    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::printDisplayExpressions(ByteCodeRunContext* context) const
{
    if (display.empty())
        return;

    for (const auto& line : display)
    {
        BcDbgCommandArg arg;
        tokenizeCommand(line, arg);
        cmdPrint(context, arg);
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplay(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return cmdDisplayPrint(context, arg);
    if (arg.split[1] == "clear" || arg.split[1] == "cl")
        return cmdDisplayClear(context, arg);
    return cmdDisplayAdd(context, arg);
}
