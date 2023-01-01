#include "pch.h"
#include "TypeInfo.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printMemory(ByteCodeRunContext* context, const Utf8& arg)
{
    vector<Utf8> cmds;
    Utf8::tokenize(arg, ' ', cmds);

    ValueFormat fmt;
    int         startIdx = 0;
    if (cmds.size() && getValueFormat(cmds[0], fmt))
        startIdx++;
    fmt.print0x = false;

    // Count
    int count = 64;
    if (startIdx < cmds.size() && cmds[startIdx].length() > 1 && cmds[startIdx][0] == '@' && Utf8::isNumber(cmds[startIdx] + 1) && cmds.size() != 1)
    {
        count = atoi(cmds[startIdx] + 1);
        startIdx++;
    }

    count = max(count, 1);
    count = min(count, 4096);

    // Expression
    Utf8 expr;
    for (int i = startIdx; i < cmds.size(); i++)
    {
        expr += cmds[i];
        expr += " ";
    }

    expr.trim();
    if (expr.empty())
    {
        g_Log.printColor("invalid 'x' expression\n", LogColor::Red);
        return;
    }

    uint64_t       addrVal = 0;
    EvaluateResult res;
    if (!evalExpression(context, expr, res))
        return;
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

        g_Log.printColor(Fmt("0x%016llx ", addrLine), LogColor::DarkYellow);
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
}

void ByteCodeDebugger::printWhere(ByteCodeRunContext* context)
{
    auto ipNode = context->debugCxtIp->node;
    auto bc     = context->debugCxtBc;

    if (ipNode && ipNode->ownerFct)
    {
        ipNode->ownerFct->typeInfo->computeScopedName();
        g_Log.printColor("function: ", LogColor::Gray);
        g_Log.printColor(ipNode->ownerFct->typeInfo->scopedName, LogColor::DarkYellow);
        g_Log.eol();
    }

    g_Log.printColor("bytecode name: ", LogColor::Gray);
    g_Log.printColor(bc->name, LogColor::DarkYellow);
    g_Log.eol();

    g_Log.printColor("bytecode type: ", LogColor::Gray);
    g_Log.printColor(bc->getCallType()->getDisplayNameC(), LogColor::DarkYellow);
    g_Log.eol();

    if (bc->sourceFile && bc->node)
    {
        g_Log.printColor("bytecode location: ", LogColor::Gray);
        g_Log.printColor(Fmt("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::DarkYellow);
        g_Log.eol();
    }
    else if (bc->sourceFile)
    {
        g_Log.printColor("bytecode source file: ", LogColor::Gray);
        g_Log.printColor(bc->sourceFile->path.c_str(), LogColor::DarkYellow);
        g_Log.eol();
    }

    if (ipNode && ipNode->sourceFile)
    {
        g_Log.printColor("instruction location: ", LogColor::Gray);
        g_Log.printColor(Fmt("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::DarkYellow);
        g_Log.eol();
    }

    g_Log.printColor("stack level: ", LogColor::Gray);
    g_Log.printColor(Fmt("%u", context->debugStackFrameOffset), LogColor::DarkYellow);
    g_Log.eol();
}

void ByteCodeDebugger::printSourceLines(SourceFile* file, SourceLocation* curLocation, int startLine, int endLine)
{
    vector<Utf8> lines;
    bool         eof = false;
    for (int l = startLine; l <= endLine && !eof; l++)
        lines.push_back(file->getLine(l, &eof));

    uint32_t lineIdx = 0;
    for (const auto& l : lines)
    {
        if (curLocation->line == startLine + lineIdx)
            g_Log.printColor("-> ", LogColor::Cyan);
        else
            g_Log.printColor("   ", LogColor::Cyan);

        g_Log.printColor(Fmt("%-5u ", startLine + lineIdx), LogColor::Gray);

        g_Log.printColor(l.c_str(), LogColor::Gray);
        g_Log.eol();

        lineIdx++;
    }
}

void ByteCodeDebugger::printSourceLines(SourceFile* file, SourceLocation* curLocation, uint32_t offset)
{
    int startLine, endLine;
    if (offset > curLocation->line)
        startLine = 0;
    else
        startLine = curLocation->line - offset;
    endLine = curLocation->line + offset;
    printSourceLines(file, curLocation, startLine, endLine);
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

    for (int i = 0; i < (cpt + count - 1); i++)
    {
        bc->printInstruction(ip, context->debugCxtIp);
        if (ip->op == ByteCodeOp::End)
            break;
        ip++;
    }
}

void ByteCodeDebugger::printContextInstruction(ByteCodeRunContext* context, bool force)
{
    SWAG_ASSERT(context->debugCxtBc);
    SWAG_ASSERT(context->debugCxtIp);

    SourceFile*     file;
    SourceLocation* location;
    ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &file, &location);

    // Print file
    if (file)
    {
        if (force || file != context->debugStepLastFile)
        {
            g_Log.printColor(Fmt("=> file: %s\n", file->name.c_str()), LogColor::DarkYellow);
        }
    }

    // Print function name
    AstNode* newFunc   = nullptr;
    bool     isInlined = false;
    if (context->debugCxtIp->node && context->debugCxtIp->node->ownerInline)
    {
        isInlined = true;
        newFunc   = context->debugCxtIp->node->ownerInline->func;
    }
    else if (context->debugCxtIp->node)
    {
        newFunc = context->debugCxtIp->node->ownerFct;
    }

    if (newFunc)
    {
        if (force || newFunc != context->debugStepLastFunc)
        {
            if (isInlined)
                g_Log.printColor(Fmt("=> inlined function: %s\n", newFunc->getScopedName().c_str()), LogColor::DarkYellow);
            else
                g_Log.printColor(Fmt("=> function: %s\n", newFunc->getScopedName().c_str()), LogColor::DarkYellow);
        }
    }

    // Print source line
    if (location && file)
    {
        if ((force && !context->debugBcMode) ||
            (context->debugStepLastFile != file ||
             (context->debugStepLastLocation && context->debugStepLastLocation->line != location->line)))
        {
            if (context->debugBcMode)
            {
                Utf8 l = file->getLine(location->line);
                l.trim();
                if (!l.empty())
                {
                    g_Log.printColor("=> code: ", LogColor::DarkYellow);
                    g_Log.printColor(l, LogColor::DarkYellow);
                    g_Log.eol();
                }
            }
            else
                printSourceLines(file, location, 3);
        }
    }

    // Print instruction
    if (context->debugBcMode)
    {
        printInstructions(context, context->debugCxtBc, context->debugCxtIp, 4);
    }

    context->debugLastBc           = context->debugCxtBc;
    context->debugStepLastFile     = file;
    context->debugStepLastLocation = location;
    context->debugStepLastFunc     = newFunc;
}
