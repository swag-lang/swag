
#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Module.h"

BcDbgCommandResult ByteCodeDebugger::cmdStep(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextLineStepIn;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdStepi(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextInstructionStepIn;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdNext(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextLineStepOut;
    g_ByteCodeDebugger.debugStepRC   = context->curRC;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdNexti(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::NextInstructionStepOut;
    g_ByteCodeDebugger.debugStepRC   = context->curRC;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdFinish(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::FinishedFunction;
    if (g_ByteCodeDebugger.debugLastBreakIp->node->ownerInline)
        g_ByteCodeDebugger.debugStepRC = context->curRC;
    else
        g_ByteCodeDebugger.debugStepRC = context->curRC - 1;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdContinue(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 1)
        return BcDbgCommandResult::BadArguments;

    g_ByteCodeDebugger.printCmdResult("continue...");
    context->debugOn                 = false;
    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdJump(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset = 0;

    uint32_t to = (uint32_t) atoi(arg.split[1].c_str());

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

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdJumpi(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    context->debugStackFrameOffset = 0;

    uint32_t to = (uint32_t) atoi(arg.split[1].c_str());

    if (to >= context->bc->numInstructions - 1)
    {
        g_ByteCodeDebugger.printCmdError("cannot reach this 'jump' destination");
        return BcDbgCommandResult::Continue;
    }

    context->ip                   = context->bc->out + to;
    g_ByteCodeDebugger.debugCxtIp = context->ip;

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdUntil(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::FileLine;
    bkp.bc         = g_ByteCodeDebugger.debugCxtBc;
    bkp.name       = g_ByteCodeDebugger.debugStepLastFile->name;
    bkp.line       = atoi(arg.split[1].c_str());
    bkp.autoRemove = true;
    g_ByteCodeDebugger.addBreakpoint(context, bkp);
    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdUntili(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 2)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::InstructionIndex;
    bkp.bc         = g_ByteCodeDebugger.debugCxtBc;
    bkp.name       = g_ByteCodeDebugger.debugStepLastFile->name;
    bkp.line       = atoi(arg.split[1].c_str());
    bkp.autoRemove = true;
    g_ByteCodeDebugger.addBreakpoint(context, bkp);
    context->debugStackFrameOffset   = 0;
    g_ByteCodeDebugger.debugStepMode = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdMode(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 2)
        return BcDbgCommandResult::BadArguments;

    if (arg.split[1] == "inline")
    {
        context->bc->sourceFile->module->buildCfg.byteCodeDebugInline = !context->bc->sourceFile->module->buildCfg.byteCodeDebugInline;
        if (context->bc->sourceFile->module->buildCfg.byteCodeDebugInline)
            g_ByteCodeDebugger.printCmdResult("inline mode");
        else
            g_ByteCodeDebugger.printCmdResult("no inline mode");
        g_ByteCodeDebugger.printDebugContext(context, true);
    }
    else if (arg.split[1] == "bkp")
    {
        g_CommandLine.dbgOff = !g_CommandLine.dbgOff;
        if (g_CommandLine.dbgOff)
            g_ByteCodeDebugger.printCmdResult("@breakpoint() is disabled");
        else
            g_ByteCodeDebugger.printCmdResult("@breakpoint() is enabled");
    }
    else
        return BcDbgCommandResult::BadArguments;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdQuit(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 1)
        return BcDbgCommandResult::BadArguments;

    g_Log.setDefaultColor();
    return BcDbgCommandResult::Return;
}
