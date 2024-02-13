#include "pch.h"
#include "ByteCodeDebugger.h"
#include "ByteCodeStack.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdBackTrace(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 1)
        return BcDbgCommandResult::BadArguments;
    printLong(g_ByteCodeStackTrace->log(context));
    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdFrame(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (!Utf8::isNumber(arg.split[1].c_str()))
    {
        printCmdError("invalid 'frame' number");
        return BcDbgCommandResult::Continue;
    }

    uint32_t       off      = atoi(arg.split[1].c_str());
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

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameUp(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    uint32_t off = 1;
    if (arg.split.size() == 2)
        off = atoi(arg.split[1].c_str());
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

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameDown(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    uint32_t off = 1;
    if (arg.split.size() == 2)
        off = atoi(arg.split[1].c_str());
    if (context->debugStackFrameOffset == 0)
    {
        printCmdError("bottom(innermost) frame selected; you cannot go down");
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

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}
