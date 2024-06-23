#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/AstNode.h"
#include "Wmf/Workspace.h"

namespace
{
    uint64_t readWatchValue(void* addr, uint32_t count, bool& err)
    {
        SWAG_TRY
        {
            err = false;
            switch (count)
            {
                case 1:
                    return *(uint8_t*) addr;
                case 2:
                    return *(uint16_t*) addr;
                case 4:
                default:
                    return *(uint32_t*) addr;
                case 8:
                    return *(uint64_t*) addr;
            }
        }
        SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
        {
            err = true;
            return 0;
        }
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
                if (context->ip == context->bc->out && testNameFilter(context->bc->getPrintName(), bkp.name, ""))
                {
                    if (!bkp.autoDisabled)
                    {
                        printMsgBkp(form("breakpoint hit #%d: function with a match on [[%s]]", idxBkp, bkp.name.c_str()));
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
                        printMsgBkp(form("breakpoint hit #%d: line [[%d]]", idxBkp, bkp.line));
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
                else if (context->curRC <= stepRc)
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
                        printMsgBkp(form("breakpoint hit #%d: instruction [[%d]]", idxBkp, bkp.line));
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

            case DebugBkpType::Memory:
            {
                bool           err      = false;
                const uint64_t newValue = readWatchValue(bkp.addr, bkp.addrCount, err);
                if (err)
                    bkp.disabled = true;
                else if (newValue != bkp.lastValue)
                {
                    printMsgBkp(form("memory breakpoint hit #%d: last value is [[0x%llx]], new value is [[0x%llx]]", idxBkp, bkp.lastValue, newValue));
                    stepMode          = DebugStepMode::None;
                    context->debugOn  = true;
                    forcePrintContext = true;
                    if (bkp.autoRemove)
                        breakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    bkp.lastValue = newValue;
                    return;
                }
            }
        }
    }
}

bool ByteCodeDebugger::addBreakpoint(ByteCodeRunContext*, const DebugBreakpoint& bkp)
{
    for (const auto& b : breakpoints)
    {
        if (b.type != bkp.type)
            continue;

        bool exists = false;
        switch (b.type)
        {
            case DebugBkpType::Memory:
                if (b.addr == bkp.addr && b.addrCount == bkp.addrCount && b.autoRemove == bkp.autoRemove)
                    exists = true;
                break;
            default:
                if (b.name == bkp.name && b.line == bkp.line && b.autoRemove == bkp.autoRemove)
                    exists = true;
                break;
        }

        if (exists)
        {
            printCmdError("breakpoint already exists");
            return false;
        }
    }

    breakpoints.push_back(bkp);
    return true;
}

void ByteCodeDebugger::printBreakpoint(const ByteCodeRunContext* context, uint32_t index) const
{
    g_Log.setColor(LogColor::Gray);
    g_Log.print(form("#%d: ", index + 1));

    const auto& bkp = breakpoints[index];
    switch (bkp.type)
    {
        case DebugBkpType::FuncName:
            if (bkp.funcNameCount == 1)
                g_Log.print(form("start of function [[%s]] in file [[%s]]:[[%d]]", bkp.name.c_str(), bkp.bc->node->token.sourceFile->path.c_str(), bkp.bc->node->token.startLocation.line + 1));
            else
                g_Log.print(form("function with a match on [[%s]] (%d matches)", bkp.name.c_str(), bkp.funcNameCount));
            break;

        case DebugBkpType::FileLine:
            g_Log.print(form("location [[%s]]:[[%d]]", bkp.file->path.c_str(), bkp.line));
            break;

        case DebugBkpType::InstructionIndex:
            g_Log.print(form("instruction index [[%d]] in [[%s]]", bkp.line, bkp.bc->getPrintName().c_str()));
            break;

        case DebugBkpType::Memory:
            g_Log.print(form("breakpoint on memory change [[0x%llx]], size [[%d]]", bkp.addr, bkp.addrCount));
            break;
    }

    if (bkp.disabled)
        g_Log.write(" (DISABLED)");
    if (bkp.autoRemove)
        g_Log.write(" (ONE SHOT)");

    switch (bkp.type)
    {
        case DebugBkpType::FileLine:
            g_Log.writeEol();
            g_ByteCodeDebugger.printSourceLinesAround(context, bkp.file, static_cast<int>(bkp.line) - 1, 0);
            break;

        case DebugBkpType::FuncName:
            g_Log.writeEol();
            if (bkp.funcNameCount == 1 && bkp.bc)
                g_ByteCodeDebugger.printSourceLinesAround(context, bkp.bc->node->token.sourceFile, static_cast<int>(bkp.bc->node->token.startLocation.line), 0);
            break;

        case DebugBkpType::InstructionIndex:
        {
            g_Log.writeEol();
            g_Log.setStoreMode(true);
            ByteCodePrintOptions opt;
            opt.curIp           = cxtIp;
            opt.printSourceCode = false;
            bkp.bc->print(opt, bkp.line, 1);
            g_Log.setStoreMode(false);
            printLong(g_Log.store);
        }
        break;

        case DebugBkpType::Memory:
            g_Log.writeEol();
            break;
    }
}

void ByteCodeDebugger::printBreakpoints(const ByteCodeRunContext* context) const
{
    if (breakpoints.empty())
    {
        printCmdError("no breakpoint");
        return;
    }

    for (uint32_t i = 0; i < breakpoints.size(); i++)
    {
        printBreakpoint(context, i);
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakEnableDisable(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg, int& numB, bool enable)
{
    if (g_ByteCodeDebugger.breakpoints.empty())
    {
        printCmdError("no breakpoint");
        return BcDbgCommandResult::Error;
    }

    if (arg.split.size() < 3)
    {
        for (auto& bkp : g_ByteCodeDebugger.breakpoints)
            bkp.disabled = !enable;
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[2].c_str()))
    {
        printCmdError(form("invalid breakpoint number [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.breakpoints.size()))
    {
        printCmdError(form("breakpoint number [[%d]] does not exist", numB));
        return BcDbgCommandResult::Error;
    }

    g_ByteCodeDebugger.breakpoints[numB - 1].disabled = !enable;
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakEnable(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    int        numB = 0;
    const auto res  = cmdBreakEnableDisable(context, arg, numB, true);
    if (res != BcDbgCommandResult::Continue)
        return res;

    if (numB)
        printCmdResult(form("breakpoint [[#%d]] has been [[enabled]]", numB));
    else
        printCmdResult(form("all breakpoints have been [[enabled]]", numB));
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakDisable(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    int        numB = 0;
    const auto res  = cmdBreakEnableDisable(context, arg, numB, false);
    if (res != BcDbgCommandResult::Continue)
        return res;
    if (numB)
        printCmdResult(form("breakpoint [[#%d]] has been [[disabled]]", numB));
    else
        printCmdResult(form("all breakpoints have been [[disabled]]", numB));
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakClear(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() == 2)
    {
        if (g_ByteCodeDebugger.breakpoints.empty())
            printCmdError("no breakpoint to remove");
        else
            printCmdResult(form("[[%d]] breakpoint(s) have been removed", g_ByteCodeDebugger.breakpoints.size()));
        g_ByteCodeDebugger.breakpoints.clear();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[2].c_str()))
    {
        printCmdError(form("invalid breakpoint number [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    const int numB = arg.split[2].toInt();
    if (!numB || numB - 1 >= static_cast<int>(g_ByteCodeDebugger.breakpoints.size()))
    {
        printCmdError(form("breakpoint number [[%d]] does not exist", numB));
        return BcDbgCommandResult::Error;
    }

    g_ByteCodeDebugger.breakpoints.erase(g_ByteCodeDebugger.breakpoints.begin() + numB - 1);
    printCmdResult(form("breakpoint [[#%d]] has been removed", numB));
    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdBreakPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    g_ByteCodeDebugger.printBreakpoints(context);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFunc(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    DebugBreakpoint bkp;
    bkp.name       = arg.split[2];
    bkp.type       = DebugBkpType::FuncName;
    bkp.autoRemove = oneShot;

    VectorNative<ByteCode*> all;
    VectorNative<ByteCode*> match;
    g_Workspace->getAllByteCodes(all);
    for (const auto bc : all)
    {
        if (testNameFilter(bc->getPrintName(), bkp.name, bc->getPrintRefName()))
            match.push_back(bc);
    }

    if (match.empty())
    {
        printCmdError(form("no function match for [[%s]]", bkp.name.c_str()));
        return BcDbgCommandResult::Error;
    }

    bkp.funcNameCount = match.size();
    bkp.bc            = match[0];
    if (!g_ByteCodeDebugger.addBreakpoint(context, bkp))
        return BcDbgCommandResult::Error;
    g_ByteCodeDebugger.printBreakpoint(context, g_ByteCodeDebugger.breakpoints.size() - 1);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[2].c_str()))
    {
        printCmdError(form("invalid line number [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    const auto loc = ByteCode::getLocation(g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp);

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::FileLine;
    bkp.file       = loc.file;
    bkp.name       = loc.file->name;
    bkp.line       = arg.split[2].toInt();
    bkp.autoRemove = oneShot;

    loc.file->getLine(0);
    if (bkp.line == 0 || bkp.line >= loc.file->allLines.size() + 1)
    {
        printCmdError(form("out of range line number [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    if (!g_ByteCodeDebugger.addBreakpoint(context, bkp))
        return BcDbgCommandResult::Error;
    g_ByteCodeDebugger.printBreakpoint(context, g_ByteCodeDebugger.breakpoints.size() - 1);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakInstruction(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[2].c_str()))
    {
        printCmdError(form("invalid instruction index [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::InstructionIndex;
    bkp.bc         = g_ByteCodeDebugger.cxtBc;
    bkp.line       = arg.split[2].toInt();
    bkp.autoRemove = oneShot;

    if (bkp.line >= g_ByteCodeDebugger.cxtBc->numInstructions)
    {
        printCmdError(form("out of range instructionindex [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    if (!g_ByteCodeDebugger.addBreakpoint(context, bkp))
        return BcDbgCommandResult::Error;
    g_ByteCodeDebugger.printBreakpoint(context, g_ByteCodeDebugger.breakpoints.size() - 1);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakFileLine(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 4)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 4)
        return BcDbgCommandResult::TooManyArguments;

    if (!Utf8::isNumber(arg.split[3].c_str()))
    {
        printCmdError(form("invalid line number [[%s]]", arg.split[3].c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    auto curFile = g_Workspace->getFileByName(arg.split[2].c_str());
    if (!curFile)
        curFile = g_Workspace->getFileByName(arg.split[2] + ".swg");
    if (!curFile)
        curFile = g_Workspace->getFileByName(arg.split[2] + ".swgs");
    if (!curFile)
    {
        printCmdError(form("cannot find file [[%s]]", arg.split[2].c_str()));
        return BcDbgCommandResult::Error;
    }

    DebugBreakpoint bkp;
    bkp.type       = DebugBkpType::FileLine;
    bkp.file       = curFile;
    bkp.name       = curFile->name;
    bkp.line       = arg.split[3].toInt();
    bkp.autoRemove = oneShot;

    curFile->getLine(0);
    if (bkp.line == 0 || bkp.line >= curFile->allLines.size() + 1)
    {
        printCmdError(form("out of range line number [[%s]]", arg.split[3].c_str()));
        return BcDbgCommandResult::Error;
    }

    if (!g_ByteCodeDebugger.addBreakpoint(context, bkp))
        return BcDbgCommandResult::Error;
    g_ByteCodeDebugger.printBreakpoint(context, g_ByteCodeDebugger.breakpoints.size() - 1);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreakMemory(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 3)
        return BcDbgCommandResult::NotEnoughArguments;
    if (arg.split.size() > 4)
        return BcDbgCommandResult::TooManyArguments;

    const auto cmd     = arg.split[0];
    bool       oneShot = false;
    if (cmd == "tb" || cmd == "tbreak")
        oneShot = true;

    auto endExpr = arg.split.size();

    // Count
    int count = 0;
    if (arg.split.back().length() > 2 &&
        arg.split.back()[0] == '-' &&
        arg.split.back()[1] == '-' &&
        Utf8::isNumber(arg.split.back() + 2))
    {
        count   = arg.split.back().toInt(2);
        endExpr = arg.split.size() - 1;
    }

    // Expression
    Utf8 expr;
    for (uint32_t i = 2; i < endExpr; i++)
    {
        expr += arg.split[i];
        expr += " ";
    }

    expr.trim();
    if (expr.empty())
    {
        printCmdError("missing breakpoint address");
        return BcDbgCommandResult::Continue;
    }

    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    void* addr = res.addr;
    if (!addr)
    {
        addr = res.value->reg.pointer;
        if (count == 0 && res.type)
        {
            count = res.type->sizeOf;
            count &= ~1;
            count = min(count, 8);
        }
    }
    else if (res.type->isPointer())
    {
        addr = *static_cast<void**>(res.addr);
        if (count == 0)
        {
            const auto typePtr = castTypeInfo<TypeInfoPointer>(res.type, TypeInfoKind::Pointer);
            count              = typePtr->pointedType->sizeOf;
            count &= ~1;
            count = min(count, 8);
        }
    }

    if (count != 1 && count != 2 && count != 4 && count != 8)
    {
        printCmdError(form("invalid memory size [[%d]] (must be 1, 2, 4 or 8)", count));
        return BcDbgCommandResult::Error;
    }

    DebugBreakpoint bkp;

    bool err      = false;
    bkp.lastValue = readWatchValue(addr, count, err);
    if (err)
    {
        printCmdError(form("cannot read address [[0x%llx]], size [[%d]]", addr, count));
        return BcDbgCommandResult::Error;
    }

    bkp.type       = DebugBkpType::Memory;
    bkp.addr       = addr;
    bkp.addrCount  = count;
    bkp.autoRemove = oneShot;
    if (!g_ByteCodeDebugger.addBreakpoint(context, bkp))
        return BcDbgCommandResult::Error;
    g_ByteCodeDebugger.printBreakpoint(context, g_ByteCodeDebugger.breakpoints.size() - 1);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdBreak(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
        return cmdBreakPrint(context, arg);
    if (arg.split[1] == "enable" || arg.split[1] == "en")
        return cmdBreakEnable(context, arg);
    if (arg.split[1] == "disable" || arg.split[1] == "dis")
        return cmdBreakDisable(context, arg);
    if (arg.split[1] == "clear" || arg.split[1] == "cl")
        return cmdBreakClear(context, arg);
    if (arg.split[1] == "func" || arg.split[1] == "f")
        return cmdBreakFunc(context, arg);
    if (arg.split[1] == "instruction" || arg.split[1] == "inst")
        return cmdBreakInstruction(context, arg);
    if (arg.split[1] == "memory" || arg.split[1] == "mem")
        return cmdBreakMemory(context, arg);
    if (arg.split[1] == "file")
        return cmdBreakFileLine(context, arg);
    if (arg.split[1] == "line")
        return cmdBreakLine(context, arg);

    printCmdError(form("invalid [[break]] command [[%s]]", arg.split[1].c_str()));
    return BcDbgCommandResult::Error;
}
