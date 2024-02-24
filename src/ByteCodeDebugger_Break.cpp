#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Log.h"
#include "Workspace.h"

void ByteCodeDebugger::printBreakpoints(ByteCodeRunContext* /*context*/) const
{
    if (breakpoints.empty())
    {
        printCmdError("no breakpoint");
        return;
    }

    g_Log.setColor(LogColor::Gray);
    for (uint32_t i = 0; i < breakpoints.size(); i++)
    {
        const auto& bkp = breakpoints[i];
        g_Log.print(form("#%d: ", i + 1));
        switch (bkp.type)
        {
            case DebugBkpType::FuncName:
                g_Log.print(form("function with a match on [[%s]]", bkp.name.c_str()));
                break;
            case DebugBkpType::FileLine:
                g_Log.print(form("file %s, line [[%d]]", bkp.name.c_str(), bkp.line));
                break;
            case DebugBkpType::InstructionIndex:
                g_Log.print(form("instruction [[%d]]", bkp.line));
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
    for (auto it = breakpoints.begin(); it != breakpoints.end(); ++it, idxBkp++)
    {
        auto& bkp = *it;
        if (bkp.disabled)
            continue;

        switch (bkp.type)
        {
            case DebugBkpType::FuncName:
            {
                if (context->ip == context->bc->out && testNameFilter(context->bc->getPrintName(), bkp.name))
                {
                    if (!bkp.autoDisabled)
                    {
                        printMsgBkp(form("breakpoint hit #%d function with a match on [[%s]]", idxBkp, bkp.name.c_str()));
                        stepMode          = DebugStepMode::None;
                        context->debugOn  = true;
                        forcePrintContext = true;
                        bkp.autoDisabled  = true;
                        if (bkp.autoRemove)
                            breakpoints.erase(it);
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

            case DebugBkpType::FileLine:
            {
                const auto loc = ByteCode::getLocation(context->bc, context->ip, true);
                if (loc.file && loc.location && loc.file->name == bkp.name && loc.location->line + 1 == bkp.line)
                {
                    if (!bkp.autoDisabled)
                    {
                        printMsgBkp(form("breakpoint hit #%d at line [[%d]]", idxBkp, bkp.line));
                        stepMode          = DebugStepMode::None;
                        context->debugOn  = true;
                        forcePrintContext = true;
                        if (bkp.autoRemove)
                            breakpoints.erase(it);
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

            case DebugBkpType::InstructionIndex:
            {
                const uint32_t offset = static_cast<uint32_t>(context->ip - context->bc->out);
                if (offset == bkp.line && context->bc == bkp.bc)
                {
                    if (!bkp.autoDisabled)
                    {
                        printMsgBkp(form("breakpoint hit #%d at instruction [[%d]]", idxBkp, bkp.line));
                        stepMode          = DebugStepMode::None;
                        context->debugOn  = true;
                        forcePrintContext = true;
                        if (bkp.autoRemove)
                            breakpoints.erase(it);
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

bool ByteCodeDebugger::addBreakpoint(ByteCodeRunContext* /*context*/, const DebugBreakpoint& bkp)
{
    for (const auto& b : breakpoints)
    {
        if (b.type == bkp.type && b.name == bkp.name && b.line == bkp.line && b.autoRemove == bkp.autoRemove)
        {
            printCmdError("breakpoint already exists");
            return false;
        }
    }

    breakpoints.push_back(bkp);
    return true;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakEnable(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.breakpoints.size()))
        printCmdError("invalid breakpoint number");
    else
    {
        g_ByteCodeDebugger.breakpoints[numB - 1].disabled = false;
        printCmdResult(form("breakpoint #%d has been enabled", numB));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakDisable(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.breakpoints.size()))
        printCmdError("invalid breakpoint number");
    else
    {
        g_ByteCodeDebugger.breakpoints[numB - 1].disabled = true;
        printCmdResult(form("breakpoint #%d has been disabled", numB));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakClear(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 2)
    {
        if (g_ByteCodeDebugger.breakpoints.empty())
            printCmdError("no breakpoint to remove");
        else
            printCmdResult(form("%d breakpoint(s) have been removed", g_ByteCodeDebugger.breakpoints.size()));
        g_ByteCodeDebugger.breakpoints.clear();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.breakpoints.size()))
        printCmdError("invalid breakpoint number");
    else
    {
        g_ByteCodeDebugger.breakpoints.erase(g_ByteCodeDebugger.breakpoints.begin() + numB - 1);
        printCmdResult(form("breakpoint #%d has been removed", numB));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakPrint(ByteCodeRunContext* context, const BcDbgCommandArg&)
{
    g_ByteCodeDebugger.printBreakpoints(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFunc(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    DebugBreakpoint bkp;
    bkp.name = arg.split[2];

    bkp.type       = DebugBkpType::FuncName;
    bkp.autoRemove = oneShot;

    if (g_ByteCodeDebugger.addBreakpoint(context, bkp))
    {
        printCmdResult(form("breakpoint #%d function with a match on [[%s]]", g_ByteCodeDebugger.breakpoints.size(), bkp.name.c_str()));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 3)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[2].c_str()))
        return BcDbgCommandResult::BadArguments;

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    const auto loc = ByteCode::getLocation(g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp);

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::FileLine;
    bkp.name       = loc.file->name;
    bkp.line       = arg.split[2].toInt();
    bkp.autoRemove = oneShot;
    if (g_ByteCodeDebugger.addBreakpoint(context, bkp))
    {
        printCmdResult(form("breakpoint #%d, file [[%s]], line [[%d]]", g_ByteCodeDebugger.breakpoints.size(), bkp.name.c_str(), bkp.line));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFileLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 4)
        return BcDbgCommandResult::BadArguments;
    if (!Utf8::isNumber(arg.split[3].c_str()))
        return BcDbgCommandResult::BadArguments;

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    auto curFile = g_Workspace->findFile(arg.split[2].c_str());
    if (!curFile)
        curFile = g_Workspace->findFile(arg.split[2] + ".swg");
    if (!curFile)
        curFile = g_Workspace->findFile(arg.split[2] + ".swgs");
    if (!curFile)
    {
        printCmdError(form("cannot find file [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Continue;
    }

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::FileLine;
    bkp.name       = curFile->name;
    bkp.line       = arg.split[3].toInt();
    bkp.autoRemove = oneShot;
    if (g_ByteCodeDebugger.addBreakpoint(context, bkp))
    {
        printCmdResult(form("breakpoint #%d, file [[%s]], line [[%d]]", g_ByteCodeDebugger.breakpoints.size(), bkp.name.c_str(), bkp.line));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreak(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return cmdBreakPrint(context, arg);
    if (arg.split[1] == "enable" || arg.split[1] == "en")
        return cmdBreakEnable(context, arg);
    if (arg.split[1] == "disable" || arg.split[1] == "di")
        return cmdBreakDisable(context, arg);
    if (arg.split[1] == "clear" || arg.split[1] == "cl")
        return cmdBreakClear(context, arg);
    if (arg.split[1] == "func" || arg.split[1] == "f")
        return cmdBreakFunc(context, arg);
    if (arg.split[1] == "line")
        return cmdBreakLine(context, arg);
    if (arg.split[1] == "file")
        return cmdBreakFileLine(context, arg);

    return BcDbgCommandResult::BadArguments;
}
