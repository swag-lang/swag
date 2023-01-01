#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printBreakpoints(ByteCodeRunContext* context)
{
    if (context->debugBreakpoints.empty())
    {
        g_Log.printColor("no breakpoint\n", LogColor::Red);
        return;
    }

    g_Log.setColor(LogColor::Gray);
    for (int i = 0; i < context->debugBreakpoints.size(); i++)
    {
        const auto& bkp = context->debugBreakpoints[i];
        g_Log.print(Fmt("#%d: ", i + 1));
        switch (bkp.type)
        {
        case ByteCodeRunContext::DebugBkpType::FuncName:
            g_Log.print(Fmt("entering function '%s'", bkp.name.c_str()));
            break;
        case ByteCodeRunContext::DebugBkpType::FileLine:
            g_Log.print(Fmt("file %s, line '%d'", bkp.name.c_str(), bkp.line));
            break;
        case ByteCodeRunContext::DebugBkpType::InstructionIndex:
            g_Log.print(Fmt("instruction '%d'", bkp.line));
            break;
        }

        if (bkp.disabled)
            g_Log.print(" (DISABLED)");
        if (bkp.autoRemove)
            g_Log.print(" (ONE SHOT)");
        g_Log.eol();
    }
}

void ByteCodeDebugger::checkBreakpoints(ByteCodeRunContext* context)
{
    for (auto it = context->debugBreakpoints.begin(); it != context->debugBreakpoints.end(); it++)
    {
        auto& bkp = *it;
        if (bkp.disabled)
            continue;

        if (bkp.type == ByteCodeRunContext::DebugBkpType::FuncName)
        {
            if ((context->ip == context->bc->out) && (context->bc->name == bkp.name))
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.printColor(Fmt("#### breakpoint hit entering function '%s' ####\n", context->bc->name.c_str()), LogColor::Magenta);
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn       = true;
                    bkp.autoDisabled       = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    break;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
        }
        else if (bkp.type == ByteCodeRunContext::DebugBkpType::FileLine)
        {
            SourceFile*     file;
            SourceLocation* location;
            ByteCode::getLocation(context->bc, context->ip, &file, &location);
            if (file && location && file->name == bkp.name && location->line == bkp.line)
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.printColor(Fmt("#### breakpoint hit at line '%d' ####\n", bkp.line), LogColor::Magenta);
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn       = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    break;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
        }
        else if (bkp.type == ByteCodeRunContext::DebugBkpType::InstructionIndex)
        {
            uint32_t offset = (uint32_t) (context->ip - context->bc->out);
            if (offset == bkp.line)
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.printColor(Fmt("#### breakpoint hit at instruction '%d' ####\n", bkp.line), LogColor::Magenta);
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn       = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    break;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
        }
    }
}

bool ByteCodeDebugger::addBreakpoint(ByteCodeRunContext* context, const ByteCodeRunContext::DebugBreakpoint& bkp)
{
    for (const auto& b : context->debugBreakpoints)
    {
        if (b.type == bkp.type && b.name == bkp.name && b.line == bkp.line && b.autoRemove == bkp.autoRemove)
        {
            g_Log.printColor("breakpoint already exists\n", LogColor::Red);
            return false;
        }
    }

    context->debugBreakpoints.push_back(bkp);
    return true;
}
