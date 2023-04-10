#include "pch.h"
#include "ByteCode.h"
#include "Workspace.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printBreakpoints(ByteCodeRunContext* context)
{
    if (context->debugBreakpoints.empty())
    {
        g_Log.print("no breakpoint\n", LogColor::Red);
        return;
    }

    g_Log.setColor(LogColor::Gray);
    for (size_t i = 0; i < context->debugBreakpoints.size(); i++)
    {
        const auto& bkp = context->debugBreakpoints[i];
        g_Log.print(Fmt("#%d: ", i + 1));
        switch (bkp.type)
        {
        case ByteCodeRunContext::DebugBkpType::FuncName:
            g_Log.print(Fmt("function with a match on '%s'", bkp.name.c_str()));
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
    int idxBkp = 1;
    for (auto it = context->debugBreakpoints.begin(); it != context->debugBreakpoints.end(); it++, idxBkp++)
    {
        auto& bkp = *it;
        if (bkp.disabled)
            continue;

        switch (bkp.type)
        {
        case ByteCodeRunContext::DebugBkpType::FuncName:
        {
            if ((context->ip == context->bc->out) && (getByteCodeName(context->bc).find(bkp.name) != -1 || getByteCodeFileName(context->bc).find(bkp.name) != -1))
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.print(Fmt("#### breakpoint hit #%d function with a match on '%s' ####\n", idxBkp, bkp.name.c_str()), LogColor::Magenta);
                    context->debugStepMode          = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn                = true;
                    context->debugForcePrintContext = true;
                    bkp.autoDisabled                = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    return;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
            break;
        }

        case ByteCodeRunContext::DebugBkpType::FileLine:
        {
            auto loc = ByteCode::getLocation(context->bc, context->ip, true);
            if (loc.file && loc.location && loc.file->name == bkp.name && loc.location->line + 1 == bkp.line)
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.print(Fmt("#### breakpoint hit #%d at line '%d' ####\n", idxBkp, bkp.line), LogColor::Magenta);
                    context->debugStepMode          = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn                = true;
                    context->debugForcePrintContext = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    return;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
            break;
        }

        case ByteCodeRunContext::DebugBkpType::InstructionIndex:
        {
            uint32_t offset = (uint32_t) (context->ip - context->bc->out);
            if (offset == bkp.line && context->bc == bkp.bc)
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.print(Fmt("#### breakpoint hit #%d at instruction '%d' ####\n", idxBkp, bkp.line), LogColor::Magenta);
                    context->debugStepMode          = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn                = true;
                    context->debugForcePrintContext = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    return;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
            break;
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
            g_Log.print("breakpoint already exists\n", LogColor::Red);
            return false;
        }
    }

    context->debugBreakpoints.push_back(bkp);
    return true;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakEnable(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    int numB = atoi(cmds[2].c_str());
    if (!numB || numB - 1 >= (int) context->debugBreakpoints.size())
        g_Log.print("invalid breakpoint number\n", LogColor::Red);
    else
    {
        context->debugBreakpoints[numB - 1].disabled = false;
        g_Log.print(Fmt("breakpoint #%d has been enabled\n", numB), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakDisable(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    int numB = atoi(cmds[2].c_str());
    if (!numB || numB - 1 >= (int) context->debugBreakpoints.size())
        g_Log.print("invalid breakpoint number\n", LogColor::Red);
    else
    {
        context->debugBreakpoints[numB - 1].disabled = true;
        g_Log.print(Fmt("breakpoint #%d has been disabled\n", numB), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakClear(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() == 2)
    {
        if (context->debugBreakpoints.empty())
            g_Log.print("no breakpoint to remove\n", LogColor::Red);
        else
            g_Log.print(Fmt("%d breakpoint(s) have been removed\n", context->debugBreakpoints.size()), LogColor::Gray);
        context->debugBreakpoints.clear();
        return BcDbgCommandResult::Continue;
    }

    if (cmds.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    int numB = atoi(cmds[2].c_str());
    if (!numB || numB - 1 >= (int) context->debugBreakpoints.size())
        g_Log.print("invalid breakpoint number\n", LogColor::Red);
    else
    {
        context->debugBreakpoints.erase(context->debugBreakpoints.begin() + numB - 1);
        g_Log.print(Fmt("breakpoint #%d has been removed\n", numB), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakPrint(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    printBreakpoints(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFunc(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 3)
        return BcDbgCommandResult::BadArguments;

    auto cmd     = cmds[0];
    bool oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    ByteCodeRunContext::DebugBreakpoint bkp;
    bkp.name = cmds[2];

    bkp.type       = ByteCodeRunContext::DebugBkpType::FuncName;
    bkp.autoRemove = oneShot;

    if (addBreakpoint(context, bkp))
    {
        g_Log.print(Fmt("breakpoint #%d function with a match on '%s'\n", context->debugBreakpoints.size(), bkp.name.c_str()), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakLine(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    auto cmd     = cmds[0];
    bool oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    auto loc = ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp);

    ByteCodeRunContext::DebugBreakpoint bkp;
    bkp.type       = ByteCodeRunContext::DebugBkpType::FileLine;
    bkp.name       = loc.file->name;
    bkp.line       = atoi(cmds[2].c_str());
    bkp.autoRemove = oneShot;
    if (addBreakpoint(context, bkp))
    {
        g_Log.print(Fmt("breakpoint #%d, file '%s', line '%d'\n", context->debugBreakpoints.size(), bkp.name.c_str(), bkp.line), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFileLine(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 4)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(cmds[3].c_str()))
        return BcDbgCommandResult::BadArguments;

    auto cmd     = cmds[0];
    bool oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    auto curFile = g_Workspace->findFile(cmds[2].c_str());
    if (!curFile)
        curFile = g_Workspace->findFile(cmds[2] + ".swg");
    if (!curFile)
        curFile = g_Workspace->findFile(cmds[2] + ".swgs");
    if (!curFile)
    {
        g_Log.print(Fmt("cannot find file '%s'\n", cmds[2].c_str()), LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    ByteCodeRunContext::DebugBreakpoint bkp;
    bkp.type       = ByteCodeRunContext::DebugBkpType::FileLine;
    bkp.name       = curFile->name;
    bkp.line       = atoi(cmds[3].c_str());
    bkp.autoRemove = oneShot;
    if (addBreakpoint(context, bkp))
    {
        g_Log.print(Fmt("breakpoint #%d, file '%s', line '%d'\n", context->debugBreakpoints.size(), bkp.name.c_str(), bkp.line), LogColor::Gray);
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreak(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() == 1)
        return cmdBreakPrint(context, cmds, cmdExpr);
    if (cmds[1] == "enable" || cmds[1] == "en")
        return cmdBreakEnable(context, cmds, cmdExpr);
    if (cmds[1] == "disable" || cmds[1] == "di")
        return cmdBreakDisable(context, cmds, cmdExpr);
    if (cmds[1] == "clear" || cmds[1] == "cl")
        return cmdBreakClear(context, cmds, cmdExpr);
    if (cmds[1] == "func" || cmds[1] == "f")
        return cmdBreakFunc(context, cmds, cmdExpr);
    if (cmds[1] == "line")
        return cmdBreakLine(context, cmds, cmdExpr);
    if (cmds[1] == "file")
        return cmdBreakFileLine(context, cmds, cmdExpr);

    return BcDbgCommandResult::BadArguments;
}