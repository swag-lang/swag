#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Os/Os.h"
#include "Report/Log.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

BcDbgCommandResult ByteCodeDebugger::cmdStep(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    g_ByteCodeDebugger.stepCount = 0;
    if (arg.split.size() > 1)
    {
        if (!Utf8::isNumber(arg.split[1].c_str()))
        {
            printCmdError(form("invalid step count [[%s]]", arg.split[1].c_str()));
            return BcDbgCommandResult::Continue;
        }

        g_ByteCodeDebugger.stepCount = arg.split[1].toInt();
    }

    context->debugStackFrameOffset = 0;
    g_ByteCodeDebugger.stepMode    = g_ByteCodeDebugger.bcMode ? DebugStepMode::NextInstructionStepIn : DebugStepMode::NextLineStepIn;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdNext(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    g_ByteCodeDebugger.stepCount = 0;
    if (arg.split.size() > 1)
    {
        if (!Utf8::isNumber(arg.split[1].c_str()))
        {
            printCmdError(form("invalid next count [[%s]]", arg.split[1].c_str()));
            return BcDbgCommandResult::Error;
        }

        g_ByteCodeDebugger.stepCount = arg.split[1].toInt();
    }

    context->debugStackFrameOffset = 0;
    g_ByteCodeDebugger.stepMode    = g_ByteCodeDebugger.bcMode ? DebugStepMode::NextInstructionStepOut : DebugStepMode::NextLineStepOut;
    g_ByteCodeDebugger.stepRc      = context->curRC;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdFinish(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    context->debugStackFrameOffset = 0;
    g_ByteCodeDebugger.stepMode    = DebugStepMode::FinishedFunction;
    if (g_ByteCodeDebugger.lastBreakIp->node->hasOwnerInline())
        g_ByteCodeDebugger.stepRc = context->curRC;
    else
        g_ByteCodeDebugger.stepRc = context->curRC - 1;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdContinue(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    printCmdResult("continue...");
    context->debugOn               = false;
    context->debugStackFrameOffset = 0;
    g_ByteCodeDebugger.stepMode    = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdJump(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[1].c_str()))
    {
        if (g_ByteCodeDebugger.bcMode)
            printCmdError(form("invalid instruction index [[%s]]", arg.split[1].c_str()));
        else
            printCmdError(form("invalid line number [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    context->debugStackFrameOffset = 0;

    const auto to = static_cast<uint32_t>(arg.split[1].toInt());

    if (g_ByteCodeDebugger.bcMode)
    {
        if (to >= context->bc->numInstructions - 1)
        {
            printCmdError("cannot reach this jump destination");
            return BcDbgCommandResult::Error;
        }

        context->ip              = context->bc->out + to;
        g_ByteCodeDebugger.cxtIp = context->ip;
    }
    else
    {
        auto curIp = context->bc->out;
        while (true)
        {
            if (curIp >= context->bc->out + context->bc->numInstructions - 1)
            {
                printCmdError("cannot reach this jump destination");
                return BcDbgCommandResult::Error;
            }

            const auto loc = ByteCode::getLocation(context->bc, curIp);
            if (loc.location && loc.location->line + 1 == to)
            {
                context->ip              = curIp;
                g_ByteCodeDebugger.cxtIp = context->ip;
                break;
            }

            curIp++;
        }
    }

    g_ByteCodeDebugger.printDebugContext(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdUntil(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[1].c_str()))
    {
        if (g_ByteCodeDebugger.bcMode)
            printCmdError(form("invalid instruction index [[%s]]", arg.split[1].c_str()));
        else
            printCmdError(form("invalid line number [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    DebugBreakpoint bkp;
    bkp.type       = g_ByteCodeDebugger.bcMode ? DebugBkpType::InstructionIndex : DebugBkpType::FileLine;
    bkp.bc         = g_ByteCodeDebugger.cxtBc;
    bkp.name       = g_ByteCodeDebugger.stepLastFile->name;
    bkp.line       = arg.split[1].toInt();
    bkp.autoRemove = true;
    g_ByteCodeDebugger.addBreakpoint(context, bkp);
    context->debugStackFrameOffset = 0;
    g_ByteCodeDebugger.stepMode    = DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdClear(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    OS::cls();
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdQuit(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    g_Log.setDefaultColor();
    return BcDbgCommandResult::Return;
}
