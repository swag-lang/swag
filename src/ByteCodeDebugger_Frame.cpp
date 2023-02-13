#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCodeDebugger.h"

BcDbgCommandResult ByteCodeDebugger::cmdBackTrace(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 1)
        return BcDbgCommandResult::BadArguments;
    g_ByteCodeStackTrace->log(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrame(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() == 1)
        return BcDbgCommandResult::BadArguments;
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (!Utf8::isNumber(cmds[1].c_str()))
    {
        g_Log.printColor("invalid 'frame' number\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    uint32_t off      = atoi(cmds[1].c_str());
    uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    off               = min(off, maxLevel);

    auto oldIndex                  = context->debugStackFrameOffset;
    context->debugStackFrameOffset = maxLevel - off;
    computeDebugContext(context);
    if (!context->debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        computeDebugContext(context);
        g_Log.printColor("this frame is external; you cannot go there\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameUp(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (cmds.size() != 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    uint32_t off = 1;
    if (cmds.size() == 2)
        off = atoi(cmds[1].c_str());
    uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    if (context->debugStackFrameOffset == maxLevel)
    {
        g_Log.printColor("initial frame selected; you cannot go up\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset += off;
    computeDebugContext(context);
    if (!context->debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        computeDebugContext(context);
        g_Log.printColor("the up frame is external; you cannot go there\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdFrameDown(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (cmds.size() != 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    uint32_t off = 1;
    if (cmds.size() == 2)
        off = atoi(cmds[1].c_str());
    if (context->debugStackFrameOffset == 0)
    {
        g_Log.printColor("bottom(innermost) frame selected; you cannot go down\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    auto oldIndex = context->debugStackFrameOffset;
    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
    computeDebugContext(context);
    if (!context->debugCxtIp)
    {
        context->debugStackFrameOffset = oldIndex;
        computeDebugContext(context);
        g_Log.printColor("the down frame is external; you cannot go there\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    printDebugContext(context);
    return BcDbgCommandResult::Continue;
}
