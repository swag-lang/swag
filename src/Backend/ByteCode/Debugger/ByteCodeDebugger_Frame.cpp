#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdBackTrace(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    uint32_t maxSteps = 0;
    if (arg.split.size() == 2)
    {
        if (!Utf8::isNumber(arg.split[1].cstr()))
        {
            printCmdError(form("invalid backtrace number [[%s]]", arg.split[1].cstr()));
            return BcDbgCommandResult::Error;
        }

        maxSteps = arg.split[1].toInt();
        maxSteps = min(maxSteps, g_ByteCodeStackTrace->maxLevel(context));
    }

    printLong(g_ByteCodeStackTrace->log(context, maxSteps, g_ByteCodeDebugger.printBtCode));
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrame(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[1].cstr()))
    {
        printCmdError(form("invalid frame number [[%s]]", arg.split[1].cstr()));
        return BcDbgCommandResult::Continue;
    }

    uint32_t       off      = arg.split[1].toInt();
    const uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    off                     = min(off, maxLevel);

    const auto oldIndex            = context->debugStackFrameOffset;
    context->debugStackFrameOffset = maxLevel - off;
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.cxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        printCmdError("this frame is external; you cannot go there");
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printOneStepContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameUp(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].cstr()))
    {
        printCmdError(form("invalid frame number [[%s]]", arg.split[1].cstr()));
        return BcDbgCommandResult::Error;
    }
    uint32_t off = 1;
    if (arg.split.size() == 2)
        off = arg.split[1].toInt();
    const uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    if (context->debugStackFrameOffset == maxLevel)
    {
        printCmdError("initial frame selected; you cannot go up");
        return BcDbgCommandResult::Continue;
    }

    const auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset += off;
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.cxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        printCmdError("the up frame is external; you cannot go there");
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printOneStepContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameDown(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].cstr()))
    {
        printCmdError(form("invalid frame number [[%s]]", arg.split[1].cstr()));
        return BcDbgCommandResult::Error;
    }

    uint32_t off = 1;
    if (arg.split.size() == 2)
        off = arg.split[1].toInt();
    if (context->debugStackFrameOffset == 0)
    {
        printCmdError("bottom (innermost) frame selected; you cannot go down");
        return BcDbgCommandResult::Continue;
    }

    const auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.cxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        printCmdError("the down frame is external; you cannot go there");
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printOneStepContext(context);
    return BcDbgCommandResult::Continue;
}
