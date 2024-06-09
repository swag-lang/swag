#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Workspace.h"

void ByteCodeDebugger::printLong(const Utf8& all)
{
    Vector<Utf8> lines;
    Utf8::tokenize(all, '\n', lines, true);
    printLong(lines);
}

void ByteCodeDebugger::printLong(const Vector<Utf8>& all)
{
    if (all.empty())
    {
        printCmdError("nothing to display");
        return;
    }

    g_Log.setColor(LogColor::Gray);

    int  cpt     = 0;
    bool canStop = true;
    for (const auto& i : all)
    {
        if (cpt == 60 && canStop)
        {
            cpt = 0;
            g_Log.setColor(LogColor::Gray);
            static constexpr auto MSG = "-- Type <RET> for more, 'q' to quit, 'c' to continue without paging --";
            g_Log.write(MSG);
            bool ctrl, shift;
            int  c = 0;
            while (true)
            {
                const auto key = OS::promptChar(c, ctrl, shift);
                if (key == OS::Key::Return)
                    break;
                if (key == OS::Key::Ascii && c == 'q')
                    return;
                if (key == OS::Key::Ascii && c == 'c')
                {
                    canStop = false;
                    break;
                }
            }

            // Erase the message
            (void) fputs(form("\x1B[%dD", strlen(MSG)), stdout);
            (void) fputs(form("\x1B[%dX", strlen(MSG)), stdout);
        }

        cpt++;

        g_Log.print(i);
        g_Log.writeEol();
    }
}

void ByteCodeDebugger::printSeparator()
{
    g_Log.writeEol();
}

void ByteCodeDebugger::printTitleNameType(const Utf8& title, const Utf8& name, const Utf8& type)
{
    g_Log.print(title, LogColor::Gray);
    g_Log.write(" ");
    uint32_t len = title.length();
    while (len++ < 25)
        g_Log.write(".");
    g_Log.write(" ");
    g_Log.print(name, LogColor::Name);
    g_Log.write(" ");
    g_Log.print(type, LogColor::Type);
    g_Log.writeEol();
}

void ByteCodeDebugger::printCmdError(const Utf8& msg)
{
    g_Log.print(msg, LogColor::Red);
    g_Log.writeEol();
}

void ByteCodeDebugger::printCmdResult(const Utf8& msg)
{
    g_Log.print(msg, LogColor::Gray);
    g_Log.writeEol();
}

void ByteCodeDebugger::printMsgBkp(const Utf8& msg)
{
    g_Log.writeEol();
    g_Log.print("#### ", LogColor::Green);
    g_Log.print(msg, LogColor::Green);
    g_Log.print(" ####", LogColor::Green);
    g_Log.writeEol();
    g_Log.writeEol();
}

void ByteCodeDebugger::printDebugContext(ByteCodeRunContext* context, bool force)
{
    SWAG_ASSERT(cxtBc);
    SWAG_ASSERT(cxtIp);

    if (forcePrintContext)
        force = true;
    forcePrintContext = false;

    const auto loc = ByteCode::getLocation(cxtBc, cxtIp, true);

    // Print file
    bool printSomething = false;
    if (loc.file)
    {
        if (force || loc.file != stepLastFile)
        {
            printTitleNameType("=> file", loc.file->name, "");
            printSomething = true;
        }
    }

    // Get current function
    AstNode*   newFunc   = nullptr;
    bool       isInlined = false;
    const auto node      = cxtIp->node;
    if (node)
    {
        if (node->hasExtOwner() && node->extOwner()->ownerInline)
        {
            isInlined = true;
            newFunc   = node->extOwner()->ownerInline->func;
        }
        else
        {
            newFunc = node->ownerFct;
        }
    }

    // Print function name
    if (newFunc)
    {
        if (force || newFunc != stepLastFunc)
        {
            if (isInlined)
                printTitleNameType("=> inlined", newFunc->getScopedName(), newFunc->typeInfo->getDisplayNameC());
            printTitleNameType("=> function", node->ownerFct->getScopedName(), node->ownerFct->typeInfo->getDisplayNameC());
            printSomething = true;
        }
    }
    else if (force || lastBc != cxtBc)
    {
        printTitleNameType("=> generated", cxtBc->name, cxtBc->typeInfoFunc ? cxtBc->typeInfoFunc->getDisplayNameC() : "");
        printSomething = true;
    }

    // Separation
    if (printSomething)
        g_Log.writeEol();

    // Print instruction
    if (bcMode)
    {
        printInstructions(context, cxtBc, cxtIp, 3);
    }

    // Print source line
    else if (loc.location && loc.file)
    {
        if (force ||
            stepLastFile != loc.file ||
            (stepLastLocation && stepLastLocation->line != loc.location->line))
        {
            printSourceLines(context, cxtBc, loc.file, loc.location, 3);
        }
    }

    lastBc           = cxtBc;
    stepLastFile     = loc.file;
    stepLastLocation = loc.location;
    stepLastFunc     = newFunc;

    if (!display.empty())
        g_Log.writeEol();
    printDisplay(context);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdWhere(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    const auto ipNode = g_ByteCodeDebugger.cxtIp->node;
    const auto bc     = g_ByteCodeDebugger.cxtBc;

    // Node
    if (ipNode && ipNode->ownerFct)
    {
        auto inlined = ipNode->hasExtOwner() ? ipNode->extOwner()->ownerInline : nullptr;
        while (inlined)
        {
            g_ByteCodeDebugger.printTitleNameType("inlined", inlined->func->getScopedName(), inlined->func->typeInfo->getDisplayNameC());
            inlined = inlined->safeOwnerInline();
        }

        g_ByteCodeDebugger.printTitleNameType("function", ipNode->ownerFct->getScopedName(), ipNode->ownerFct->typeInfo->getDisplayNameC());

        if(ipNode->ownerFct->token.sourceFile)
        {
            const auto loc = form("%s:%u:%u", ipNode->ownerFct->token.sourceFile->path.c_str(), ipNode->ownerFct->token.startLocation.line + 1, ipNode->ownerFct->token.startLocation.column + 1);
            g_ByteCodeDebugger.printTitleNameType("function location", loc, "");
        }
    }

    // ByteCode
    g_ByteCodeDebugger.printTitleNameType("bytecode", bc->getPrintName(), bc->typeInfoFunc->getDisplayNameC());
    if (bc->sourceFile && bc->node)
    {
        const auto loc = form("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("bytecode location", loc, "");
    }
    else if (bc->sourceFile)
    {
        g_Log.print("bytecode source file: ", LogColor::Gray);
        g_Log.print(bc->sourceFile->path, LogColor::DarkYellow);
        g_Log.writeEol();
    }

    if (ipNode && ipNode->token.sourceFile)
    {
        const auto loc = form("%s:%u:%u", ipNode->token.sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("instruction location", loc, "");
    }

    const uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    g_ByteCodeDebugger.printTitleNameType("stack frame", form("%u", maxLevel - context->debugStackFrameOffset), "");
    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::printSourceLines(const ByteCodeRunContext* context, const ByteCode* bc, SourceFile* file, const SourceLocation* curLocation, int startLine, int endLine) const
{
    Vector<Utf8> lines;
    bool         eof = false;
    for (int l = startLine; l <= endLine && !eof; l++)
        lines.push_back(file->getLine(l, &eof));

    SyntaxColorContext cxt;
    uint32_t           lineIdx = 0;
    Vector<Utf8>       toPrint;
    for (const auto& l : lines)
    {
        Utf8       oneLine;
        const bool currentLine = curLocation->line == startLine + lineIdx;

        // Current line
        if (currentLine)
        {
            oneLine += Log::colorToVTS(LogColor::CurInstruction);
            oneLine += "-> ";
        }
        else
        {
            oneLine += Log::colorToVTS(LogColor::Index);
            oneLine += "   ";
        }

        // Line
        oneLine += form("%-5u ", startLine + lineIdx + 1);
        oneLine += Log::colorToVTS(LogColor::Default);

        // Line breakpoint
        const DebugBreakpoint* hasBkp = nullptr;
        for (auto& bkp : breakpoints)
        {
            switch (bkp.type)
            {
                case DebugBkpType::FuncName:
                {
                    const auto loc = ByteCode::getLocation(bc, bc->out);
                    if (context->bc->getPrintName().find(bkp.name) != -1 && loc.location && startLine + lineIdx + 1 == loc.location->line)
                        hasBkp = &bkp;
                    break;
                }

                case DebugBkpType::FileLine:
                    if (file->name == bkp.name && startLine + lineIdx + 1 == bkp.line)
                        hasBkp = &bkp;
                    break;

                default:
                    break;
            }
        }

        if (hasBkp)
        {
            if (hasBkp->disabled)
                oneLine += Log::colorToVTS(LogColor::Default);
            else
                oneLine += Log::colorToVTS(LogColor::Breakpoint);
            oneLine += Utf8("\xe2\x96\xa0");
        }
        else
            oneLine += " ";
        oneLine += " ";

        // Code
        if (currentLine)
        {
            oneLine += Log::colorToVTS(LogColor::CurInstruction);
            oneLine += l;
        }
        else if (g_CommandLine.logColors)
        {
            oneLine += syntaxColor(l, cxt);
        }
        else
        {
            oneLine += Log::colorToVTS(LogColor::Default);
            oneLine += l;
        }

        toPrint.push_back(oneLine);
        lineIdx++;
    }

    printLong(toPrint);
}

void ByteCodeDebugger::printSourceLines(const ByteCodeRunContext* context, const ByteCode* bc, SourceFile* file, const SourceLocation* curLocation, uint32_t offset) const
{
    int startLine;
    if (offset > curLocation->line)
        startLine = 0;
    else
        startLine = curLocation->line - offset;
    const int endLine = curLocation->line + offset;
    printSourceLines(context, bc, file, curLocation, startLine, endLine);
}

void ByteCodeDebugger::printInstructions(const ByteCodeRunContext*, const ByteCode* bc, const ByteCodeInstruction* ip, int num) const
{
    const int count = num;
    int       cpt   = 1;
    while (num--)
    {
        if (ip == bc->out)
            break;
        cpt++;
        ip--;
    }

    ByteCodePrintOptions opt;
    opt.curIp           = cxtIp;
    opt.printSourceCode = printBcCode;
    bc->print(opt, static_cast<uint32_t>(ip - bc->out), cpt + count);
}

BcDbgCommandResult ByteCodeDebugger::cmdMemory(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--num\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The number of values to display\n");

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of each value. Can be one of the following values:\n");
        g_Log.setColor(LogColor::Type);
        g_Log.print("    s8 s16 s32 s64 u8 u16 u32 u64 x8 x16 x32 x64 f32 f64\n");

        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    Vector<Utf8> exprCmds;
    Utf8::tokenize(arg.cmdExpr, ' ', exprCmds);

    // Print format
    ValueFormat fmt;
    if (!exprCmds.empty() && getValueFormat(exprCmds.back(), fmt))
        exprCmds.pop_back();
    fmt.print0X = false;

    // Count
    int count = 64;
    if (exprCmds.size() != 1 &&
        exprCmds.back().length() > 2 &&
        exprCmds.back()[0] == '-' &&
        exprCmds.back()[1] == '-' &&
        Utf8::isNumber(exprCmds.back() + 2))
    {
        count = exprCmds.back().toInt(2);
        exprCmds.pop_back();
    }

    count = max(count, 1);
    count = min(count, 4096);

    // Expression
    Utf8 expr;
    for (const auto& e : exprCmds)
    {
        expr += e;
        expr += " ";
    }

    expr.trim();
    if (expr.empty())
    {
        printCmdError("invalid 'x' expression");
        return BcDbgCommandResult::Continue;
    }

    uint64_t       addrVal;
    EvaluateResult res;

    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    if (!res.addr)
    {
        res.addr = res.value->reg.pointer;
        addrVal  = reinterpret_cast<uint64_t>(res.addr);
    }
    else if (res.type->isPointer())
        addrVal = *static_cast<uint64_t*>(res.addr);
    else
        addrVal = reinterpret_cast<uint64_t>(res.addr);

    int perLine = 8;
    switch (fmt.bitCount)
    {
        case 8:
            perLine = 16;
            break;
        case 16:
            perLine = 8;
            break;
        case 32:
            perLine = 8;
            break;
        case 64:
            perLine = 4;
            break;
    }

    auto addrB = reinterpret_cast<const uint8_t*>(addrVal);
    while (count > 0)
    {
        const auto addrLine = addrB;

        g_Log.print(form("0x%016llx ", addrLine), LogColor::DarkYellow);
        g_Log.setColor(LogColor::Gray);

        for (int i = 0; i < min(count, perLine); i++)
        {
            Utf8 str;
            appendLiteralValue(context, str, fmt, addrB);
            g_Log.print(str);
            addrB += fmt.bitCount / 8;
        }

        addrB = addrLine;
        if (fmt.bitCount == 8)
        {
            // Align to the right
            for (int i = 0; i < perLine - min(count, perLine); i++)
            {
                if (fmt.isHexa)
                    g_Log.write("   ");
                else if (!fmt.isSigned)
                    g_Log.write("    ");
                else
                    g_Log.write("     ");
            }

            // Print as 'char'
            g_Log.write(" ");
            for (int i = 0; i < min(count, perLine); i++)
            {
                auto c = getAddrValue<uint8_t>(addrB);
                if (c < 32 || c > 127)
                    c = '.';
                g_Log.print(form("%c", c));
                addrB += 1;
            }
        }

        g_Log.writeEol();

        addrB = addrLine;
        addrB += static_cast<int64_t>(min(count, perLine)) * (fmt.bitCount / 8);
        count -= min(count, perLine);
        if (!count)
            break;
    }

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdInstruction(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].c_str()))
    {
        printCmdError(form("invalid instruction count [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    int regN = 3;
    if (arg.split.size() == 2)
        regN = arg.split[1].toInt();

    g_Log.setStoreMode(true);
    g_ByteCodeDebugger.printInstructions(context, g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, regN);
    g_Log.setStoreMode(false);
    g_ByteCodeDebugger.printLong(g_Log.store);
    g_ByteCodeDebugger.bcMode = true;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstructionDump(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    auto toLogBc = g_ByteCodeDebugger.cxtBc;
    auto toLogIp = g_ByteCodeDebugger.cxtIp;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Error;
        toLogIp = toLogBc->out;
    }

    ByteCodePrintOptions opt;
    opt.curIp           = toLogIp;
    opt.printSourceCode = g_ByteCodeDebugger.printBcCode;

    g_Log.setStoreMode(true);
    toLogBc->print(opt);
    g_Log.setStoreMode(false);
    g_ByteCodeDebugger.printLong(g_Log.store);
    g_ByteCodeDebugger.bcMode = true;

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() > 1 && !Utf8::isNumber(arg.split[1].c_str()))
    {
        printCmdError(form("invalid line count [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto toLogBc        = g_ByteCodeDebugger.cxtBc;
    const auto toLogIp        = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (toLogBc->node && toLogBc->node->is(AstNodeKind::FuncDecl) && toLogBc->node->token.sourceFile)
    {
        uint32_t offset = 3;
        if (arg.split.size() == 2)
            offset = arg.split[1].toInt();

        const auto inl = g_ByteCodeDebugger.lastBreakIp->node->safeOwnerInline();
        if (inl)
        {
            const auto loc = ByteCode::getLocation(toLogBc, toLogIp, true);
            g_ByteCodeDebugger.printSourceLines(context, toLogBc, inl->func->token.sourceFile, loc.location, offset);
        }
        else
        {
            const auto loc = ByteCode::getLocation(toLogBc, toLogIp, false);
            g_ByteCodeDebugger.printSourceLines(context, toLogBc, toLogBc->node->token.sourceFile, loc.location, offset);
        }
    }
    else
        printCmdError("no source code");

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdLongList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    auto toLogBc              = g_ByteCodeDebugger.cxtBc;
    auto toLogIp              = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Error;
        toLogIp = toLogBc->out;
    }

    if (toLogBc->node && toLogBc->node->is(AstNodeKind::FuncDecl) && toLogBc->node->token.sourceFile)
    {
        const auto funcNode = castAst<AstFuncDecl>(toLogBc->node, AstNodeKind::FuncDecl);
        if (funcNode->content)
        {
            toLogBc->printName();

            const auto inl = g_ByteCodeDebugger.lastBreakIp->node->safeOwnerInline();
            if (inl)
            {
                const auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, true);
                const uint32_t startLine = inl->func->token.startLocation.line;
                const uint32_t endLine   = inl->func->content->token.endLocation.line;
                g_ByteCodeDebugger.printSourceLines(context, toLogBc, inl->func->token.sourceFile, loc.location, startLine, endLine);
            }
            else
            {
                const auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, false);
                const uint32_t startLine = toLogBc->node->token.startLocation.line;
                const uint32_t endLine   = funcNode->content->token.endLocation.line;
                g_ByteCodeDebugger.printSourceLines(context, toLogBc, toLogBc->node->token.sourceFile, loc.location, startLine, endLine);
            }
        }
        else
            printCmdError("no source code");
    }
    else
        printCmdError("no source code");

    return BcDbgCommandResult::Continue;
}
