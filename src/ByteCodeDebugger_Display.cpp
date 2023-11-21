#include "pch.h"
#include "ByteCode.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Report.h"
#include "ByteCodeDebugger.h"
#include "Parser.h"
#include "Ast.h"

BcDbgCommandResult ByteCodeDebugger::cmdDisplayAdd(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::BadArguments;

    auto line = arg.cmd + " " + arg.cmdExpr;
    g_ByteCodeDebugger.debugDisplay.push_back(line);
    g_ByteCodeDebugger.printDisplay(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdDisplayClear(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 2)
    {
        if (g_ByteCodeDebugger.debugDisplay.empty())
            g_ByteCodeDebugger.printCmdError("no expression to remove");
        else
            g_ByteCodeDebugger.printCmdResult(Fmt("%d expression(s) have been removed", g_ByteCodeDebugger.debugDisplay.size()));
        g_ByteCodeDebugger.debugDisplay.clear();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    int numB = atoi(arg.split[2].c_str());
    if (!numB || numB - 1 >= (int) g_ByteCodeDebugger.debugDisplay.size())
        g_ByteCodeDebugger.printCmdError("invalid expression number");
    else
    {
        g_ByteCodeDebugger.debugDisplay.erase(g_ByteCodeDebugger.debugDisplay.begin() + numB - 1);
        g_ByteCodeDebugger.printCmdResult(Fmt("expression #%d has been removed", numB));
    }

    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::printDisplayList()
{
    if (debugDisplay.empty())
    {
        g_ByteCodeDebugger.printCmdError("no expression");
        return;
    }

    g_Log.setColor(LogColor::Gray);
    for (size_t i = 0; i < debugDisplay.size(); i++)
    {
        g_Log.print(Fmt("#%d: %s", i + 1, debugDisplay[i].c_str()));
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

void ByteCodeDebugger::printDisplay(ByteCodeRunContext* context)
{
    if (debugDisplay.empty())
        return;

    for (const auto& line : debugDisplay)
    {
        BcDbgCommandArg arg;
        tokenizeCommand(context, line, arg);
        cmdPrint(context, arg);
    }
}