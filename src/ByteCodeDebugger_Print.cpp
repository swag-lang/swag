#include "pch.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Log.h"
#include "SyntaxColor.h"
#include "TypeInfo.h"
#include "Workspace.h"

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
            static auto MSG = "-- Type <RET> for more, 'q' to quit, 'c' to continue without paging --";
            g_Log.print(MSG);
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
            (void) fputs(FMT("\x1B[%dD", strlen(MSG)), stdout);
            (void) fputs(FMT("\x1B[%dX", strlen(MSG)), stdout);
        }

        cpt++;

        g_Log.print(i);
        g_Log.eol();
    }
}

void ByteCodeDebugger::printSeparator()
{
    g_Log.eol();
}

void ByteCodeDebugger::printTitleNameType(const Utf8& title, const Utf8& name, const Utf8& type)
{
    g_Log.print(title.c_str(), LogColor::Gray);
    g_Log.print(" ");
    int len = title.length();
    while (len++ < 25)
        g_Log.print(".");
    g_Log.print(" ");
    g_Log.print(name.c_str(), LogColor::Name);
    g_Log.print(" ");
    g_Log.print(type.c_str(), LogColor::Type);
    g_Log.eol();
}

void ByteCodeDebugger::printCmdError(const Utf8& msg)
{
    g_Log.print(msg.c_str(), LogColor::Red);
    g_Log.eol();
}

void ByteCodeDebugger::printCmdResult(const Utf8& msg)
{
    g_Log.print(msg.c_str(), LogColor::Gray);
    g_Log.eol();
}

void ByteCodeDebugger::printMsgBkp(const Utf8& msg)
{
    g_Log.eol();
    g_Log.print("#### ", LogColor::Green);
    g_Log.print(msg.c_str(), LogColor::Green);
    g_Log.print(" ####", LogColor::Green);
    g_Log.eol();
    g_Log.eol();
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
        g_Log.eol();

    // Print instruction
    if (bcMode)
    {
        printInstructions(context, cxtBc, cxtIp, 4);
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
        g_Log.eol();
    printDisplay(context);
}

BcDbgCommandResult ByteCodeDebugger::cmdWhere(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() != 1)
        return BcDbgCommandResult::BadArguments;

    const auto ipNode = g_ByteCodeDebugger.cxtIp->node;
    const auto bc     = g_ByteCodeDebugger.cxtBc;

    if (ipNode && ipNode->ownerFct)
    {
        auto inlined = ipNode->hasExtOwner() ? ipNode->extOwner()->ownerInline : nullptr;
        while (inlined)
        {
            g_ByteCodeDebugger.printTitleNameType("inlined", inlined->func->getScopedName(), inlined->func->typeInfo->getDisplayNameC());
            inlined = inlined->safeOwnerInline();
        }

        g_ByteCodeDebugger.printTitleNameType("function", ipNode->ownerFct->getScopedName(), ipNode->ownerFct->typeInfo->getDisplayNameC());
    }

    g_ByteCodeDebugger.printTitleNameType("bytecode", bc->getPrintName(), bc->typeInfoFunc->getDisplayNameC());
    if (bc->sourceFile && bc->node)
    {
        const auto loc = FMT("%s:%u:%u", bc->sourceFile->path.string().c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("bytecode location", loc, "");
    }
    else if (bc->sourceFile)
    {
        g_Log.print("bytecode source file: ", LogColor::Gray);
        g_Log.print(bc->sourceFile->path.string().c_str(), LogColor::DarkYellow);
        g_Log.eol();
    }

    if (ipNode && ipNode->token.sourceFile)
    {
        const auto loc = FMT("%s:%u:%u", ipNode->token.sourceFile->path.string().c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("instruction location", loc, "");
    }

    g_ByteCodeDebugger.printTitleNameType("stack level", FMT("%u", context->debugStackFrameOffset), "");
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
        oneLine += FMT("%-5u ", startLine + lineIdx + 1);
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
            oneLine += syntaxColor(l, cxt).c_str();
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
    while (--num)
    {
        if (ip == bc->out)
            break;
        cpt++;
        ip--;
    }

    ByteCodePrintOptions opt;
    opt.curIp = cxtIp;
    bc->print(opt, static_cast<uint32_t>(ip - bc->out), cpt + count - 1);
}

BcDbgCommandResult ByteCodeDebugger::cmdMemory(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::BadArguments;

    Vector<Utf8> exprCmds;
    Utf8::tokenize(arg.cmdExpr, ' ', exprCmds);

    // Print format
    ValueFormat fmt;
    size_t      startIdx = 0;
    if (!exprCmds.empty() && getValueFormat(exprCmds[0], fmt))
        startIdx++;
    fmt.print0X = false;

    // Count
    int count = 64;
    if (startIdx < exprCmds.size() &&
        exprCmds[startIdx].length() > 1 &&
        exprCmds[startIdx][0] == '/' &&
        Utf8::isNumber(exprCmds[startIdx] + 1) &&
        exprCmds.size() != 1)
    {
        count = exprCmds[startIdx].toInt(1);
        startIdx++;
    }

    count = max(count, 1);
    count = min(count, 4096);

    // Expression
    Utf8 expr;
    for (size_t i = startIdx; i < exprCmds.size(); i++)
    {
        expr += exprCmds[i];
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
        return BcDbgCommandResult::Continue;

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
        default:
            break;
    }

    auto addrB = reinterpret_cast<const uint8_t*>(addrVal);
    while (count > 0)
    {
        const auto addrLine = addrB;

        g_Log.print(FMT("0x%016llx ", addrLine), LogColor::DarkYellow);
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
                    g_Log.print("   ");
                else if (!fmt.isSigned)
                    g_Log.print("    ");
                else
                    g_Log.print("     ");
            }

            // Print as 'char'
            g_Log.print(" ");
            for (int i = 0; i < min(count, perLine); i++)
            {
                auto c = getAddrValue<uint8_t>(addrB);
                if (c < 32 || c > 127)
                    c = '.';
                g_Log.print(FMT("%c", c));
                addrB += 1;
            }
        }

        g_Log.eol();

        addrB = addrLine;
        addrB += min(count, perLine) * (fmt.bitCount / 8);
        count -= min(count, perLine);
        if (!count)
            break;
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstruction(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    int regN = 4;
    if (arg.split.size() == 2)
        regN = arg.split[1].toInt();
    g_ByteCodeDebugger.bcMode = true;

    g_Log.setStoreMode(true);
    g_ByteCodeDebugger.printInstructions(context, g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, regN);
    g_Log.setStoreMode(false);
    g_ByteCodeDebugger.printLong(g_Log.store);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstructionDump(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    auto toLogBc              = g_ByteCodeDebugger.cxtBc;
    auto toLogIp              = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = true;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Continue;
        toLogIp = toLogBc->out;
    }

    ByteCodePrintOptions opt;
    opt.curIp = toLogIp;

    g_Log.setStoreMode(true);
    toLogBc->print(opt);
    g_Log.setStoreMode(false);
    g_ByteCodeDebugger.printLong(g_Log.store);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (arg.split.size() > 1 && !Utf8::isNumber(arg.split[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    const auto toLogBc        = g_ByteCodeDebugger.cxtBc;
    const auto toLogIp        = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->token.sourceFile)
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

BcDbgCommandResult ByteCodeDebugger::cmdLongList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    auto toLogBc              = g_ByteCodeDebugger.cxtBc;
    auto toLogIp              = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Continue;
        toLogIp = toLogBc->out;
    }

    if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->token.sourceFile)
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
