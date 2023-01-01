#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "ByteCodeStack.h"
#include "AstNode.h"
#include "Ast.h"
#include "AstOutput.h"
#include "Concat.h"
#include "Os.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Report.h"
#include "ByteCodeDebugger.h"

BcDbgCommandResult ByteCodeDebugger::cmdStep(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::EvalDefault;

    context->debugStackFrameOffset = 0;
    context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdNext(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::EvalDefault;

    context->debugStackFrameOffset = 0;
    context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
    context->debugStepRC           = context->curRC;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdFinish(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::EvalDefault;

    context->debugStackFrameOffset = 0;
    context->debugStepMode         = ByteCodeRunContext::DebugStepMode::FinishedFunction;
    context->debugStepRC           = context->curRC - 1;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdContinue(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::EvalDefault;

    g_Log.printColor("continue...\n", LogColor::Gray);
    context->debugStackFrameOffset = 0;
    context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
    context->debugOn               = false;
    return BcDbgCommandResult::Break;
}

BcDbgCommandResult ByteCodeDebugger::cmdJump(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::EvalDefault;
    if (cmds.size() > 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::EvalDefault;

    context->debugStackFrameOffset = 0;

    uint32_t to = (uint32_t) atoi(cmds[1].c_str());
    if (context->debugBcMode)
    {
        if (to >= context->bc->numInstructions - 1)
        {
            g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
            return BcDbgCommandResult::Continue;
        }

        context->ip         = context->bc->out + to;
        context->debugCxtIp = context->ip;
    }
    else
    {
        auto curIp = context->bc->out;
        while (true)
        {
            if (curIp >= context->bc->out + context->bc->numInstructions - 1)
            {
                g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
                return BcDbgCommandResult::Continue;
            }

            SourceFile*     file;
            SourceLocation* location;
            ByteCode::getLocation(context->bc, curIp, &file, &location);
            if (location && location->line == to)
            {
                context->ip         = curIp;
                context->debugCxtIp = context->ip;
                break;
            }

            curIp++;
        }
    }

    printContextInstruction(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdUntil(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::EvalDefault;
    if (cmds.size() > 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::EvalDefault;

    ByteCodeRunContext::DebugBreakpoint bkp;
    if (context->debugBcMode)
        bkp.type = ByteCodeRunContext::DebugBkpType::InstructionIndex;
    else
        bkp.type = ByteCodeRunContext::DebugBkpType::FileLine;
    bkp.name       = context->debugStepLastFile->name;
    bkp.line       = atoi(cmds[1].c_str());
    bkp.autoRemove = true;
    addBreakpoint(context, bkp);
    context->debugStackFrameOffset = 0;
    context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
    return BcDbgCommandResult::Break;
}