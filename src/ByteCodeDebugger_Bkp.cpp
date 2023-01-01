#include "pch.h"
#include "ByteCode.h"
#include "Workspace.h"
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

BcDbgCommandResult ByteCodeDebugger::cmdBreakpoint(ByteCodeRunContext* context, const Utf8& cmd, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    if (cmds.size() == 1)
    {
        printBreakpoints(context);
        return BcDbgCommandResult::Continue;
    }

    bool oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    // At line
    vector<Utf8> fileFunc;
    Utf8::tokenize(cmds[1], ':', fileFunc);
    if (cmds.size() == 2 && ((fileFunc.size() == 1 && Utf8::isNumber(cmds[1].c_str())) || (fileFunc.size() == 2 && Utf8::isNumber(fileFunc[1]))))
    {
        SourceFile*     curFile;
        SourceLocation* curLocation;
        ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &curFile, &curLocation);

        ByteCodeRunContext::DebugBreakpoint bkp;
        bkp.name = curFile->name;

        if (fileFunc.size() == 2)
        {
            if (fileFunc[0] != curFile->name && fileFunc[0] + ".swg" != curFile->name && fileFunc[0] + ".swgs" != curFile->name)
            {
                auto f = g_Workspace->findFile(fileFunc[0]);
                if (!f)
                    f = g_Workspace->findFile(fileFunc[0] + ".swg");
                if (!f)
                    f = g_Workspace->findFile(fileFunc[0] + ".swgs");
                if (!f)
                {
                    g_Log.printColor(Fmt("cannot find file '%s'\n", fileFunc[0].c_str()), LogColor::Red);
                    return BcDbgCommandResult::Continue;
                }
                bkp.name = f->name;
            }
        }

        int lineNo     = fileFunc.size() == 2 ? atoi(fileFunc[1]) : atoi(cmds[1].c_str());
        bkp.type       = ByteCodeRunContext::DebugBkpType::FileLine;
        bkp.line       = lineNo;
        bkp.autoRemove = oneShot;
        if (addBreakpoint(context, bkp))
            g_Log.printColor(Fmt("breakpoint #%d, file '%s', line '%d'\n", context->debugBreakpoints.size(), curFile->name.c_str(), lineNo), LogColor::Gray);
        return BcDbgCommandResult::Continue;
    }

    // Clear all breakpoints
    if (cmds.size() == 2 && cmds[1] == "clear")
    {
        if (context->debugBreakpoints.empty())
            g_Log.printColor("no breakpoint\n", LogColor::Red);
        else
        {
            g_Log.printColor(Fmt("%d breakpoint(s) have been removed\n", context->debugBreakpoints.size()), LogColor::Gray);
            context->debugBreakpoints.clear();
        }

        return BcDbgCommandResult::Continue;
    }

    if (cmds.size() == 2)
        return BcDbgCommandResult::EvalDefault;

    // Break on function
    if (cmds[1] == "func")
    {
        ByteCodeRunContext::DebugBreakpoint bkp;
        bkp.type = ByteCodeRunContext::DebugBkpType::FuncName;
        auto bc  = g_Workspace->findBc(cmds[2].c_str());
        if (!bc)
        {
            g_Log.printColor(Fmt("cannot find function '%s'\n", cmds[2].c_str()), LogColor::Red);
            return BcDbgCommandResult::Continue;
        }

        bkp.name       = cmds[2];
        bkp.autoRemove = oneShot;
        if (addBreakpoint(context, bkp))
            g_Log.printColor(Fmt("breakpoint #%d entering function '%s'\n", context->debugBreakpoints.size(), bkp.name.c_str()), LogColor::Gray);
        return BcDbgCommandResult::Continue;
    }

    if (!Utf8::isNumber(cmds[2].c_str()))
        return BcDbgCommandResult::EvalDefault;

    // Clear one breakpoint
    if (cmds[1] == "clear")
    {
        int numB = atoi(cmds[2].c_str());
        if (!numB || numB - 1 >= context->debugBreakpoints.size())
            g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
        else
        {
            context->debugBreakpoints.erase(context->debugBreakpoints.begin() + numB - 1);
            g_Log.printColor(Fmt("breakpoint #%d has been removed\n", numB), LogColor::Gray);
        }

        return BcDbgCommandResult::Continue;
    }

    // Disable one breakpoint
    if (cmds[1] == "disable")
    {
        int numB = atoi(cmds[2].c_str());
        if (!numB || numB - 1 >= context->debugBreakpoints.size())
            g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
        else
        {
            context->debugBreakpoints[numB - 1].disabled = true;
            g_Log.printColor(Fmt("breakpoint #%d has been disabled\n", numB), LogColor::Gray);
        }

        return BcDbgCommandResult::Continue;
    }

    // Enable one breakpoint
    if (cmds[1] == "enable")
    {
        int numB = atoi(cmds[2].c_str());
        if (!numB || numB - 1 >= context->debugBreakpoints.size())
            g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
        else
        {
            context->debugBreakpoints[numB - 1].disabled = false;
            g_Log.printColor(Fmt("breakpoint #%d has been enabled\n", numB), LogColor::Gray);
        }

        return BcDbgCommandResult::Continue;
    }

    return BcDbgCommandResult::Continue;
}