#include "pch.h"
#include "TypeInfo.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Workspace.h"
#include "Ast.h"
#include "SyntaxColor.h"

void ByteCodeDebugger::printDebugContext(ByteCodeRunContext* context, bool force)
{
    SWAG_ASSERT(context->debugCxtBc);
    SWAG_ASSERT(context->debugCxtIp);

    if (context->debugForcePrintContext)
        force = true;
    context->debugForcePrintContext = false;

    auto loc = ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, true);

    // Print file
    bool printSomething = false;
    if (loc.file)
    {
        if (force || loc.file != context->debugStepLastFile)
        {
            g_Log.print("=> ", LogColor::DarkYellow);
            g_Log.print("file: ", LogColor::DarkYellow);
            g_Log.print(loc.file->name.c_str(), LogColor::Cyan);
            g_Log.eol();
            printSomething = true;
        }
    }

    // Get current function
    AstNode* newFunc   = nullptr;
    bool     isInlined = false;
    auto     node      = context->debugCxtIp->node;
    if (node)
    {
        if (node->ownerInline)
        {
            isInlined = true;
            newFunc   = node->ownerInline->func;
        }
        else
        {
            newFunc = node->ownerFct;
        }
    }

    // Print function name
    if (newFunc)
    {
        if (force || newFunc != context->debugStepLastFunc)
        {
            if (isInlined)
            {
                g_Log.print("=> ", LogColor::DarkYellow);
                g_Log.print("inlined: ", LogColor::DarkYellow);
                g_Log.print(newFunc->getScopedName().c_str(), LogColor::Cyan);
                g_Log.print(" ");
                g_Log.print(newFunc->typeInfo->getDisplayNameC(), LogColor::DarkCyan);
                g_Log.eol();
            }

            g_Log.print("=> ", LogColor::DarkYellow);
            g_Log.print("function: ", LogColor::DarkYellow);
            g_Log.print(node->ownerFct->getScopedName().c_str(), LogColor::Cyan);
            g_Log.print(" ");
            g_Log.print(node->ownerFct->typeInfo->getDisplayNameC(), LogColor::DarkCyan);
            g_Log.eol();
            printSomething = true;
        }
    }
    else if (force || (context->debugLastBc != context->debugCxtBc))
    {
        g_Log.print("=> ", LogColor::DarkYellow);
        g_Log.print("generated: ", LogColor::DarkYellow);
        g_Log.print(context->debugCxtBc->name.c_str(), LogColor::Cyan);
        g_Log.print(" ");
        if (context->debugCxtBc->typeInfoFunc)
            g_Log.print(context->debugCxtBc->typeInfoFunc->getDisplayNameC(), LogColor::DarkCyan);
        g_Log.eol();
        printSomething = true;
    }

    // Separation
    if (printSomething)
        printSeparator();

    // Print instruction
    if (context->debugBcMode)
    {
        printInstructions(context, context->debugCxtBc, context->debugCxtIp, 4);
    }

    // Print source line
    else if (loc.location && loc.file)
    {
        if ((force) ||
            (context->debugStepLastFile != loc.file) ||
            (context->debugStepLastLocation && context->debugStepLastLocation->line != loc.location->line))
        {
            printSourceLines(context, context->debugCxtBc, loc.file, loc.location, 3);
        }
    }

    context->debugLastBc           = context->debugCxtBc;
    context->debugStepLastFile     = loc.file;
    context->debugStepLastLocation = loc.location;
    context->debugStepLastFunc     = newFunc;
}

BcDbgCommandResult ByteCodeDebugger::cmdWhere(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() != 1)
        return BcDbgCommandResult::BadArguments;

    auto ipNode = context->debugCxtIp->node;
    auto bc     = context->debugCxtBc;

    if (ipNode && ipNode->ownerFct)
    {
        auto inlined = ipNode->ownerInline;
        while (inlined)
        {
            g_Log.print("inlined: ", LogColor::Gray);
            g_Log.print(inlined->func->getScopedName(), LogColor::DarkYellow);
            g_Log.print(" ");
            g_Log.print(inlined->func->typeInfo->getDisplayNameC(), LogColor::DarkCyan);
            g_Log.eol();
            inlined = inlined->ownerInline;
        }

        g_Log.print("function: ", LogColor::Gray);
        g_Log.print(ipNode->ownerFct->getScopedName(), LogColor::DarkYellow);
        g_Log.print(" ");
        g_Log.print(ipNode->ownerFct->typeInfo->getDisplayNameC(), LogColor::DarkCyan);
        g_Log.eol();
    }

    g_Log.eol();
    g_Log.print("bytecode: ", LogColor::Gray);
    g_Log.print(bc->name, LogColor::DarkYellow);
    g_Log.print(" ");
    g_Log.print(bc->typeInfoFunc->getDisplayNameC(), LogColor::DarkCyan);
    g_Log.eol();

    if (bc->sourceFile && bc->node)
    {
        g_Log.print("bytecode location: ", LogColor::Gray);
        g_Log.print(Fmt("%s:%u:%u", bc->sourceFile->path.string().c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::DarkYellow);
        g_Log.eol();
    }
    else if (bc->sourceFile)
    {
        g_Log.print("bytecode source file: ", LogColor::Gray);
        g_Log.print(bc->sourceFile->path.string().c_str(), LogColor::DarkYellow);
        g_Log.eol();
    }

    g_Log.eol();
    if (ipNode && ipNode->sourceFile)
    {
        g_Log.print("instruction location: ", LogColor::Gray);
        g_Log.print(Fmt("%s:%u:%u", ipNode->sourceFile->path.string().c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::DarkYellow);
        g_Log.eol();
    }

    g_Log.print("stack level: ", LogColor::Gray);
    g_Log.print(Fmt("%u", context->debugStackFrameOffset), LogColor::DarkYellow);
    g_Log.eol();

    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, int startLine, int endLine)
{
    Vector<Utf8> lines;
    bool         eof = false;
    for (int l = startLine; l <= endLine && !eof; l++)
        lines.push_back(file->getLine(l, &eof));

    SyntaxColorContext cxt;
    uint32_t           lineIdx = 0;
    for (const auto& l : lines)
    {
        // Current line
        if (curLocation->line == startLine + lineIdx)
            g_Log.setColor(LogColor::Green);
        else
            g_Log.setColor(LogColor::Gray);

        if (curLocation->line == startLine + lineIdx)
            g_Log.print("-> ");
        else
            g_Log.print("   ");

        // Line
        g_Log.print(Fmt("%-5u ", startLine + lineIdx + 1));

        // Line breakpoint
        ByteCodeRunContext::DebugBreakpoint* hasBkp = nullptr;
        for (auto it = context->debugBreakpoints.begin(); it != context->debugBreakpoints.end(); it++)
        {
            auto& bkp = *it;
            switch (bkp.type)
            {
            case ByteCodeRunContext::DebugBkpType::FuncName:
            {
                auto loc = ByteCode::getLocation(bc, bc->out);
                if (getByteCodeName(context->bc).find(bkp.name) != -1 && loc.location && startLine + lineIdx + 1 == loc.location->line)
                    hasBkp = &bkp;
                break;
            }

            case ByteCodeRunContext::DebugBkpType::FileLine:
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
                g_Log.setColor(LogColor::Gray);
            else
                g_Log.setColor(LogColor::Red);
            g_Log.print(Utf8("\xe2\x96\xa0"));
        }
        else
            g_Log.print(" ");
        g_Log.print(" ");

        // Code
        if (curLocation->line == startLine + lineIdx)
            g_Log.setColor(LogColor::Green);
        else
            g_Log.setColor(LogColor::Gray);

        if (g_CommandLine.logColors)
            g_Log.print(syntaxColor(l, cxt).c_str());
        else
            g_Log.print(l.c_str());

        g_Log.eol();

        lineIdx++;
    }
}

void ByteCodeDebugger::printSourceLines(ByteCodeRunContext* context, ByteCode* bc, SourceFile* file, SourceLocation* curLocation, uint32_t offset)
{
    int startLine, endLine;
    if (offset > curLocation->line)
        startLine = 0;
    else
        startLine = curLocation->line - offset;
    endLine = curLocation->line + offset;
    printSourceLines(context, bc, file, curLocation, startLine, endLine);
}

void ByteCodeDebugger::printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num)
{
    int count = num;
    int cpt   = 1;
    while (--num)
    {
        if (ip == bc->out)
            break;
        cpt++;
        ip--;
    }

    ByteCodePrintOptions opt;
    opt.curIp = context->debugCxtIp;
    bc->print(opt, (uint32_t) (ip - bc->out), cpt + count - 1);
}

BcDbgCommandResult ByteCodeDebugger::cmdMemory(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() < 2)
        return BcDbgCommandResult::BadArguments;

    Vector<Utf8> exprCmds;
    Utf8::tokenize(cmdExpr, ' ', exprCmds);

    // Print format
    ValueFormat fmt;
    size_t      startIdx = 0;
    if (exprCmds.size() && getValueFormat(exprCmds[0], fmt))
        startIdx++;
    fmt.print0x = false;

    // Count
    int count = 64;
    if (startIdx < exprCmds.size() &&
        exprCmds[startIdx].length() > 1 &&
        exprCmds[startIdx][0] == '/' &&
        Utf8::isNumber(exprCmds[startIdx] + 1) &&
        exprCmds.size() != 1)
    {
        count = atoi(exprCmds[startIdx] + 1);
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
        g_Log.print("invalid 'x' expression\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    uint64_t       addrVal = 0;
    EvaluateResult res;
    if (!evalExpression(context, expr, res))
        return BcDbgCommandResult::Continue;

    if (!res.addr)
    {
        res.addr = res.value->reg.pointer;
        addrVal  = (uint64_t) res.addr;
    }
    else if (res.type->isPointer())
        addrVal = *(uint64_t*) res.addr;
    else
        addrVal = (uint64_t) res.addr;

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

    const uint8_t* addrB = (const uint8_t*) addrVal;

    while (count > 0)
    {
        auto addrLine = addrB;

        g_Log.print(Fmt("0x%016llx ", addrLine), LogColor::DarkYellow);
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
                g_Log.print(Fmt("%c", c));
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

BcDbgCommandResult ByteCodeDebugger::cmdInstruction(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (cmds.size() != 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    int regN = 4;
    if (cmds.size() == 2)
        regN = atoi(cmds[1].c_str());

    printInstructions(context, context->debugCxtBc, context->debugCxtIp, regN);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstructionDump(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    auto toLogBc = context->debugCxtBc;
    auto toLogIp = context->debugCxtIp;

    if (cmds.size() > 1)
    {
        auto name = cmds[1];
        auto bc   = g_Workspace->findBc(name);
        if (bc)
        {
            toLogBc = bc;
            toLogIp = bc->out;
        }
        else
        {
            g_Log.print(Fmt("cannot find function '%s'\n", name.c_str()), LogColor::Red);
            return BcDbgCommandResult::Continue;
        }
    }

    ByteCodePrintOptions opt;
    opt.curIp = toLogIp;
    toLogBc->print(opt);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdList(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;
    if (cmds.size() > 1 && !Utf8::isNumber(cmds[1].c_str()))
        return BcDbgCommandResult::BadArguments;

    auto toLogBc = context->debugCxtBc;
    auto toLogIp = context->debugCxtIp;

    if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->sourceFile)
    {
        uint32_t offset = 3;
        if (cmds.size() == 2)
            offset = atoi(cmds[1].c_str());

        auto inl = context->debugLastBreakIp->node->ownerInline;
        if (inl)
        {
            auto loc = ByteCode::getLocation(toLogBc, toLogIp, true);
            printSourceLines(context, toLogBc, inl->func->sourceFile, loc.location, offset);
        }
        else
        {
            auto loc = ByteCode::getLocation(toLogBc, toLogIp, false);
            printSourceLines(context, toLogBc, toLogBc->node->sourceFile, loc.location, offset);
        }
    }
    else
        g_Log.print("no source code\n", LogColor::Red);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdLongList(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    auto toLogBc = context->debugCxtBc;
    auto toLogIp = context->debugCxtIp;

    if (cmds.size() > 1)
    {
        auto name = cmds[1];
        auto bc   = g_Workspace->findBc(name);
        if (bc)
        {
            toLogBc = bc;
            toLogIp = bc->out;
        }
        else
        {
            g_Log.print(Fmt("cannot find function '%s'\n", name.c_str()), LogColor::Red);
            return BcDbgCommandResult::Continue;
        }
    }

    if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->sourceFile)
    {
        auto funcNode = CastAst<AstFuncDecl>(toLogBc->node, AstNodeKind::FuncDecl);
        if (funcNode->content)
        {
            auto inl = context->debugLastBreakIp->node->ownerInline;
            if (inl)
            {
                auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, true);
                uint32_t startLine = inl->func->token.startLocation.line;
                uint32_t endLine   = inl->func->content->token.endLocation.line;
                printSourceLines(context, toLogBc, inl->func->sourceFile, loc.location, startLine, endLine);
            }
            else
            {
                auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, false);
                uint32_t startLine = toLogBc->node->token.startLocation.line;
                uint32_t endLine   = funcNode->content->token.endLocation.line;
                printSourceLines(context, toLogBc, toLogBc->node->sourceFile, loc.location, startLine, endLine);
            }
        }
        else
            g_Log.print("no source code\n", LogColor::Red);
    }
    else
        g_Log.print("no source code\n", LogColor::Red);

    return BcDbgCommandResult::Continue;
}