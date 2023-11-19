#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Module.h"

BcDbgCommandResult ByteCodeDebugger::cmdStep(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextLine;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdNext(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextLineStepOut;
    g_ByteCodeDebugger.debugStepRC   = context->curRC;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdFinish(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::FinishedFunction;
    if (g_ByteCodeDebugger.debugLastBreakIp->node->ownerInline)
        g_ByteCodeDebugger.debugStepRC = context->curRC;
    else
        g_ByteCodeDebugger.debugStepRC = context->curRC - 1;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdContinue(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::BadArguments;

    g_Log.print("continue...\n", LogColor::Gray);
    context->debugOn                 = false;
    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdJump(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset = 0;

    uint32_t to = (uint32_t) atoi(cmds[1].c_str());
    if (g_ByteCodeDebugger.debugBcMode)
    {
        if (to >= context->bc->numInstructions - 1)
        {
            g_ByteCodeDebugger.printCmdError("cannot reach this 'jump' destination");
            return BcDbgCommandResult::Continue;
        }

        context->ip                   = context->bc->out + to;
        g_ByteCodeDebugger.debugCxtIp = context->ip;
    }
    else
    {
        auto curIp = context->bc->out;
        while (true)
        {
            if (curIp >= context->bc->out + context->bc->numInstructions - 1)
            {
                g_ByteCodeDebugger.printCmdError("cannot reach this 'jump' destination");
                return BcDbgCommandResult::Continue;
            }

            auto loc = ByteCode::getLocation(context->bc, curIp);
            if (loc.location && loc.location->line + 1 == to)
            {
                context->ip                   = curIp;
                g_ByteCodeDebugger.debugCxtIp = context->ip;
                break;
            }

            curIp++;
        }
    }

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdUntil(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    DebugBreakpoint bkp;
    if (g_ByteCodeDebugger.debugBcMode)
        bkp.type = DebugBkpType::InstructionIndex;
    else
        bkp.type = DebugBkpType::FileLine;
    bkp.bc         = g_ByteCodeDebugger.debugCxtBc;
    bkp.name       = g_ByteCodeDebugger.debugStepLastFile->name;
    bkp.line       = atoi(cmds[1].c_str());
    bkp.autoRemove = true;
    g_ByteCodeDebugger.addBreakpoint(context, bkp);
    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdMode(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 2)
        return BcDbgCommandResult::BadArguments;

    if (cmds[1] == "bc")
    {
        g_ByteCodeDebugger.debugBcMode = !g_ByteCodeDebugger.debugBcMode;
        if (g_ByteCodeDebugger.debugBcMode)
            g_ByteCodeDebugger.printCmdResult("bytecode mode");
        else
            g_ByteCodeDebugger.printCmdResult("source code mode");
        g_ByteCodeDebugger.printDebugContext(context, true);
    }
    else if (cmds[1] == "inline")
    {
        context->bc->sourceFile->module->buildCfg.byteCodeDebugInline = !context->bc->sourceFile->module->buildCfg.byteCodeDebugInline;
        if (context->bc->sourceFile->module->buildCfg.byteCodeDebugInline)
            g_ByteCodeDebugger.printCmdResult("inline mode");
        else
            g_ByteCodeDebugger.printCmdResult("no inline mode");
        g_ByteCodeDebugger.printDebugContext(context, true);
    }
    else if (cmds[1] == "bkp")
    {
        g_CommandLine.dbgOff = !g_CommandLine.dbgOff;
        if (g_CommandLine.dbgOff)
            g_ByteCodeDebugger.printCmdResult("@breakpoint() are disabled");
        else
            g_ByteCodeDebugger.printCmdResult("@breakpoint() are enabled");
    }
    else
        return BcDbgCommandResult::BadArguments;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdQuit(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 1)
        return BcDbgCommandResult::BadArguments;

    g_Log.setDefaultColor();
    return BcDbgCommandResult::Return;
}
