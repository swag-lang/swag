#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Workspace.h"

Utf8 ByteCodeDebugger::getPrintValue(const Utf8& name, TypeInfo* typeinfo)
{
    Utf8 str;
    if (typeinfo)
    {
        str += Log::colorToVTS(LogColor::Default);
        str += "(";
        str += Log::colorToVTS(LogColor::Type);
        str += typeinfo->getDisplayNameC();
        str += Log::colorToVTS(LogColor::Default);
        str += ") ";
    }

    str += Log::colorToVTS(LogColor::Name);
    str += name;
    str += Log::colorToVTS(LogColor::Default);

    return str;
}

Utf8 ByteCodeDebugger::getPrintSymbols(ByteCodeRunContext* context, const VectorNative<AstNode*>& nodes, const VectorNative<uint8_t*>& addrs)
{
    Utf8 result;
    for (uint32_t i = 0; i < nodes.size(); i++)
    {
        const auto n    = nodes[i];
        const auto over = n->resolvedSymbolOverload();
        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
            continue;

        Utf8 value = getPrintValue(over->symbol->getFullName(), over->typeInfo);

        if (over->typeInfo->isNative() || over->typeInfo->isPointer())
        {
            value += " = ";

            EvaluateResult res;
            res.type = over->typeInfo;
            res.addr = addrs[i];
            appendTypedValue(context, value, res, 0, 0);
            value.trimRightEol();
        }

        result += value;

        if (over->node && over->node->token.sourceFile)
        {
            result += Log::colorToVTS(LogColor::Location);
            result += " ";
            result += over->node->token.sourceFile->path;
        }

        result += "\n";
    }

    return result;
}

void ByteCodeDebugger::printLong(const Utf8& all)
{
    Vector<Utf8> lines;
    Utf8::tokenize(all, '\n', lines, true);

    Vector<std::pair<Utf8, Utf8>> pairs;
    for (const auto& line : lines)
        pairs.push_back({line, ""});

    printLong(pairs, "");
}

void ByteCodeDebugger::printLong(const Utf8& all, const Utf8& filter)
{
    Vector<Utf8> lines;
    Utf8::tokenize(all, '\n', lines, true);

    Vector<std::pair<Utf8, Utf8>> pairs;
    for (const auto& line : lines)
        pairs.push_back({line, line});

    printLong(pairs, filter);    
}

void ByteCodeDebugger::printLong(const Vector<Utf8>& all)
{
    Vector<std::pair<Utf8, Utf8>> pairs;
    for (const auto& line : all)
        pairs.push_back({line, ""});

    printLong(pairs, "");
}

void ByteCodeDebugger::printLong(const Vector<std::pair<Utf8, Utf8>>& all, const Utf8& filter, LogColor color)
{
    if (all.empty())
    {
        printCmdError("nothing to display");
        return;
    }

    g_Log.setColor(color);

    int  cpt          = 0;
    bool canStop      = true;
    bool hasSomething = false;

    for (const auto& i : all)
    {
        if (!testNameFilter(i.second, filter))
            continue;

        if (cpt == 60 && canStop)
        {
            cpt = 0;

            g_Log.setColor(LogColor::Gray);
            static constexpr auto MSG = "-- Type <RET> for more, 'q' to quit, 'c' to continue without paging --";
            g_Log.write(MSG);
            g_Log.setColor(color);

            bool ctrl, shift;
            int  c = 0;
            while (true)
            {
                const auto key = OS::promptChar(c, ctrl, shift);
                if (key == OS::Key::Return)
                    break;
                if (key == OS::Key::Ascii && c == 'q')
                {
                    g_Log.writeEol();
                    return;
                }
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

        hasSomething = true;
        g_Log.print(i.first);
        g_Log.writeEol();
    }

    if (!hasSomething && !filter.empty())
    {
        printCmdError(form("...no match with filter [[%s]] (on %d tested elements)", filter.c_str(), all.size()));
    }
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
    if (loc.file)
    {
        if (force || loc.file != stepLastFile)
        {
            printTitleNameType("=> file", loc.file->name, "");
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
        }
    }
    else if (force || lastBc != cxtBc)
    {
        printTitleNameType("=> generated", cxtBc->name, cxtBc->typeInfoFunc ? cxtBc->typeInfoFunc->getDisplayNameC() : "");
    }

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

    // Print current display expressions
    printDisplay(context);
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
        const bool currentLine = curLocation && curLocation->line == startLine + lineIdx;

        // Line breakpoint
        const DebugBreakpoint* hasBkp = nullptr;
        for (auto& bkp : breakpoints)
        {
            switch (bkp.type)
            {
                case DebugBkpType::FuncName:
                {
                    if (!bc)
                        bc = bkp.bc;
                    if (!bc)
                        break;
                    const auto loc = ByteCode::getLocation(bc, bc->out);
                    if (bc->getPrintName().find(bkp.name) != -1 && loc.location && startLine + lineIdx + 1 == loc.location->line)
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

            if (currentLine)
            {
                oneLine += Log::colorToVTS(LogColor::CurInstruction);
                oneLine += "> ";
            }
            else
            {
                oneLine += Log::colorToVTS(LogColor::Index);
                oneLine += "  ";
            }
        }
        else if (currentLine)
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

void ByteCodeDebugger::printSourceLines(const ByteCodeRunContext* context, SourceFile* file, int line, int offset) const
{
    file->getLine(0);
    const int startLine = max(line - offset, 0);
    const int endLine   = min(line + offset, (int) file->allLines.size() - 1);
    printSourceLines(context, nullptr, file, nullptr, startLine, endLine);
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
