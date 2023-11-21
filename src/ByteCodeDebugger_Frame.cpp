#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCodeDebugger.h"

BcDbgCommandResult ByteCodeDebugger::cmdBackTrace(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 1)
        return BcDbgCommandResult::BadArguments;
    g_Log.print(g_ByteCodeStackTrace->log(context));
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrame(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (!Utf8::isNumber(arg.split[1].c_str()))
    {
        g_ByteCodeDebugger.printCmdError("invalid 'frame' number");
        return BcDbgCommandResult::Continue;
    }

    uint32_t off      = atoi(arg.split[1].c_str());
    uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    off               = min(off, maxLevel);

    auto oldIndex                  = context->debugStackFrameOffset;
    context->debugStackFrameOffset = maxLevel - off;
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        g_ByteCodeDebugger.printCmdError("this frame is external; you cannot go there");
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
    uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    if (context->debugStackFrameOffset == maxLevel)
    {
        g_ByteCodeDebugger.printCmdError("initial frame selected; you cannot go up");
        return BcDbgCommandResult::Continue;
    }

    auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset += off;
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        g_ByteCodeDebugger.printCmdError("the up frame is external; you cannot go there");
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
        g_ByteCodeDebugger.printCmdError("bottom(innermost) frame selected; you cannot go down");
        return BcDbgCommandResult::Continue;
    }

    auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
    g_ByteCodeDebugger.computeDebugContext(context);
    if (!g_ByteCodeDebugger.debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        g_ByteCodeDebugger.computeDebugContext(context);
        g_ByteCodeDebugger.printCmdError("the down frame is external; you cannot go there");
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}
