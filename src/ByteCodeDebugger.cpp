#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "ByteCodeStack.h"
#include "AstNode.h"
#include "Ast.h"
#include "AstOutput.h"
#include "Concat.h"
#include "Os.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Report.h"
#include "ByteCodeDebugger.h"

static bool isNumber(const char* pz)
{
    while (*pz)
    {
        if (!isdigit(*pz))
            return false;
        pz++;
    }

    return true;
}

static bool getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN)
{
    regN = atoi(arg.c_str() + 1);
    if (!context->getRegCount(context->debugCxtRc))
    {
        g_Log.printColor("no available register", LogColor::Red);
        return false;
    }

    if (regN >= context->getRegCount(context->debugCxtRc))
    {
        g_Log.printColor(Fmt("invalid register number, maximum value is '%u'\n", (uint32_t) context->getRegCount(context->debugCxtRc) - 1), LogColor::Red);
        return false;
    }

    return true;
}

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
    if (startIdx < cmds.size() && cmds[startIdx].length() > 1 && cmds[startIdx][0] == '@' && isNumber(cmds[startIdx] + 1) && cmds.size() != 1)
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

static void printWhere(ByteCodeRunContext* context)
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

static void printSourceLines(SourceFile* file, SourceLocation* curLocation, int startLine, int endLine)
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

static void printSourceLines(SourceFile* file, SourceLocation* curLocation, uint32_t offset = 3)
{
    int startLine, endLine;
    if (offset > curLocation->line)
        startLine = 0;
    else
        startLine = curLocation->line - offset;
    endLine = curLocation->line + offset;
    printSourceLines(file, curLocation, startLine, endLine);
}

static void printInstructions(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1)
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

void ByteCodeDebugger::computeDebugContext(ByteCodeRunContext* context)
{
    context->debugCxtBc    = context->bc;
    context->debugCxtIp    = context->ip;
    context->debugCxtRc    = context->curRC;
    context->debugCxtBp    = context->bp;
    context->debugCxtSp    = context->sp;
    context->debugCxtSpAlt = context->spAlt;
    context->debugCxtStack = context->stack;
    if (context->debugStackFrameOffset == 0)
        return;

    VectorNative<ByteCodeStackStep> steps;
    g_ByteCodeStackTrace->getSteps(steps);
    if (steps.empty())
        return;

    uint32_t maxLevel              = g_ByteCodeStackTrace->maxLevel(context);
    context->debugStackFrameOffset = min(context->debugStackFrameOffset, maxLevel);
    uint32_t ns                    = 0;

    for (int i = (int) maxLevel; i >= 0; i--)
    {
        if (i >= steps.size())
            continue;

        auto& step = steps[i];
        if (ns == context->debugStackFrameOffset)
        {
            context->debugCxtBc    = step.bc;
            context->debugCxtIp    = step.ip;
            context->debugCxtBp    = step.bp;
            context->debugCxtSp    = step.sp;
            context->debugCxtSpAlt = step.spAlt;
            context->debugCxtStack = step.stack;
            break;
        }

        ns++;
        if (!step.ip)
            continue;
        if (context->debugCxtRc)
            context->debugCxtRc--;
    }
}

Utf8 ByteCodeDebugger::completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete)
{
    vector<Utf8> tokens;
    Utf8::tokenize(line, ' ', tokens);
    if (tokens.empty())
        return "";

    toComplete = tokens.back();

    SemanticContext                   semContext;
    SemanticJob                       semJob;
    VectorNative<AlternativeScope>    scopeHierarchy;
    VectorNative<AlternativeScopeVar> scopeHierarchyVars;
    semContext.sourceFile = context->debugCxtIp->node->sourceFile;
    semContext.node       = context->debugCxtIp->node;
    semContext.job        = &semJob;
    if (SemanticJob::collectScopeHierarchy(&semContext, scopeHierarchy, scopeHierarchyVars, context->debugCxtIp->node, COLLECT_ALL))
    {
        for (const auto& p : scopeHierarchy)
        {
            const auto& mm       = p.scope->symTable.mapNames;
            uint32_t    cptValid = 0;
            for (uint32_t i = 0; i < mm.allocated; i++)
            {
                if (!mm.buffer[i].symbolName || !mm.buffer[i].hash)
                    continue;
                const auto& sn = mm.buffer[i].symbolName->name;
                if (sn.find(toComplete) == 0)
                    return mm.buffer[i].symbolName->name;

                cptValid++;
                if (cptValid >= mm.count)
                    break;
            }
        }
    }

    return "";
}

Utf8 ByteCodeDebugger::getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift)
{
    static vector<Utf8> debugCmdHistory;
    static uint32_t     debugCmdHistoryIndex = 0;

    Utf8 line;
    int  cursorX = 0;

    while (true)
    {
        int  c   = 0;
        auto key = OS::promptChar(c, ctrl, shift);

        if (key == OS::Key::Return)
            break;

#define MOVE_LEFT()               \
    {                             \
        fputs("\x1B[1D", stdout); \
        cursorX--;                \
    }
#define MOVE_RIGHT()              \
    {                             \
        fputs("\x1B[1C", stdout); \
        cursorX++;                \
    }

        switch (key)
        {
        case OS::Key::Left:
            if (!cursorX)
                continue;
            if (ctrl)
            {
                if (isblank(line[cursorX - 1]))
                {
                    while (cursorX && isblank(line[cursorX - 1]))
                        MOVE_LEFT();
                }
                else
                {
                    while (cursorX && (isalnum(line[cursorX - 1]) || line[cursorX - 1] == '_' || isdigit(line[cursorX - 1])))
                        MOVE_LEFT();
                }
            }
            else
                MOVE_LEFT();
            continue;

        case OS::Key::Right:
            if (cursorX == line.count)
                continue;
            if (ctrl)
            {
                if (isblank(line[cursorX]))
                {
                    while (cursorX != line.count && isblank(line[cursorX]))
                        MOVE_RIGHT();
                }
                else
                {
                    while (cursorX != line.count && (isalnum(line[cursorX]) || line[cursorX] == '_' || isdigit(line[cursorX])))
                        MOVE_RIGHT();
                }
            }
            else
                MOVE_RIGHT();
            continue;

        case OS::Key::Home:
            if (cursorX)
                fputs(Fmt("\x1B[%dD", cursorX), stdout); // Move the cursor at 0
            cursorX = 0;
            continue;
        case OS::Key::End:
            if (cursorX != line.count)
                fputs(Fmt("\x1B[%dC", line.count - cursorX), stdout); // Move the cursor to the end of line
            cursorX = line.count;
            continue;
        case OS::Key::Delete:
            if (cursorX == line.count)
                continue;
            fputs("\x1B[1P", stdout); // Delete the character
            line.remove(cursorX, 1);
            continue;
        case OS::Key::Back:
            if (!cursorX)
                continue;
            MOVE_LEFT();
            fputs("\x1B[1P", stdout); // Delete the character
            line.remove(cursorX, 1);
            continue;
        case OS::Key::Up:
            if (debugCmdHistoryIndex == 0)
                continue;
            if (cursorX) // Move the cursor at 0
                fputs(Fmt("\x1B[%dD", cursorX), stdout);
            fputs(Fmt("\x1B[%dX", line.count), stdout); // Erase the current command
            line = debugCmdHistory[--debugCmdHistoryIndex];
            fputs(line, stdout); // Insert command from history
            cursorX = line.count;
            break;
        case OS::Key::Down:
            if (debugCmdHistoryIndex == debugCmdHistory.size())
                continue;
            if (cursorX) // Move the cursor at 0
                fputs(Fmt("\x1B[%dD", cursorX), stdout);
            fputs(Fmt("\x1B[%dX", line.count), stdout); // Erase the current command
            debugCmdHistoryIndex++;
            if (debugCmdHistoryIndex != debugCmdHistory.size())
            {
                line = debugCmdHistory[debugCmdHistoryIndex];
                fputs(line, stdout); // Insert command from history
                cursorX = line.count;
            }
            else
            {
                line.clear();
                cursorX = 0;
            }
            break;

        case OS::Key::Escape:
            if (cursorX) // Move the cursor at 0
                fputs(Fmt("\x1B[%dD", cursorX), stdout);
            fputs(Fmt("\x1B[%dX", line.count), stdout); // Erase the current command
            line.clear();
            cursorX = 0;
            break;

        case OS::Key::Ascii:
            fputs("\x1B[1@", stdout); // Insert n blanks and shift right
            fputc(c, stdout);
            line.insert(cursorX, (char) c);
            cursorX++;
            break;

        case OS::Key::Tab:
            if (!context->debugCxtIp || !context->debugCxtIp->node || !context->debugCxtIp->node->sourceFile)
                continue;
            if (cursorX != line.count)
                continue;

            Utf8 toComplete;
            auto n = completion(context, line, toComplete);
            if (!n.empty() && n != toComplete)
            {
                n.remove(0, toComplete.length());
                fputs(n, stdout);
                line += n;
                cursorX += n.length();
                continue;
            }

            break;
        }
    }

    line.trim();
    if (!line.empty())
    {
        while (debugCmdHistory.size() != debugCmdHistoryIndex)
            debugCmdHistory.pop_back();
        debugCmdHistory.push_back(line);
        debugCmdHistoryIndex = (uint32_t) debugCmdHistory.size();
    }

    g_Log.eol();
    return line;
}

bool ByteCodeDebugger::step(ByteCodeRunContext* context)
{
    static mutex dbgMutex;
    ScopedLock   sc(dbgMutex);

    auto ip                              = context->ip;
    auto module                          = context->bc->sourceFile->module;
    g_ByteCodeStackTrace->currentContext = context;

    if (context->debugEntry)
    {
        static bool firstOne = true;

        g_Log.setColor(LogColor::Gray);

        if (firstOne)
        {
            firstOne = false;
            g_Log.eol();
            g_Log.printColor("#############################################\n", LogColor::Magenta);
            g_Log.printColor("entering bytecode debugger, type '?' for help\n", LogColor::Magenta);
            g_Log.printColor("---------------------------------------------\n", LogColor::Magenta);
            g_Log.printColor(Fmt("build configuration            = '%s'\n", g_CommandLine.buildCfg.c_str()), LogColor::Magenta);
            g_Log.printColor(Fmt("BuildCfg.byteCodeDebugInline   = %s\n", module->buildCfg.byteCodeDebugInline ? "true" : "false"), LogColor::Magenta);
            g_Log.printColor(Fmt("BuildCfg.byteCodeInline        = %s\n", module->buildCfg.byteCodeInline ? "true" : "false"), LogColor::Magenta);
            g_Log.printColor(Fmt("BuildCfg.byteCodeOptimizeLevel = %d\n", module->buildCfg.byteCodeOptimizeLevel), LogColor::Magenta);
            g_Log.printColor("#############################################\n", LogColor::Magenta);
            g_Log.eol();
        }
        else
        {
            g_Log.printColor("#### @breakpoint() hit ####\n", LogColor::Magenta);
        }

        context->debugEntry            = false;
        context->debugStepMode         = ByteCodeRunContext::DebugStepMode::None;
        context->debugStepLastLocation = nullptr;
        context->debugStepLastFile     = nullptr;
        context->debugStackFrameOffset = 0;
    }

    // Check breakpoints
    checkBreakpoints(context);

    // Step to the next line
    bool zapCurrentIp = false;
    switch (context->debugStepMode)
    {
    case ByteCodeRunContext::DebugStepMode::ToNextBreakpoint:
        zapCurrentIp = true;
        break;

    case ByteCodeRunContext::DebugStepMode::NextLine:
    {
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &file, &location);
        if (!location || (context->debugStepLastFile == file && context->debugStepLastLocation && context->debugStepLastLocation->line == location->line))
            zapCurrentIp = true;
        else
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        }
        break;
    }
    case ByteCodeRunContext::DebugStepMode::NextLineStepOut:
    {
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &file, &location, false, true);
        if (!location || (context->debugStepLastFile == file && context->debugStepLastLocation && context->debugStepLastLocation->line == location->line))
            zapCurrentIp = true;
        else if (context->curRC > context->debugStepRC)
            zapCurrentIp = true;
        else
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        }
        break;
    }
    case ByteCodeRunContext::DebugStepMode::FinishedFunction:
    {
        if (context->curRC == 0 && context->debugStepRC == -1 && ip->op == ByteCodeOp::Ret)
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        else if (context->curRC > context->debugStepRC)
            zapCurrentIp = true;
        else
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        }
        break;
    }
    }

    if (!zapCurrentIp)
    {
        computeDebugContext(context);
        printContextInstruction(context);

        while (true)
        {
            g_Log.setColor(LogColor::Green);
            g_Log.print("> ");

            // Get command from user
            bool ctrl  = false;
            bool shift = false;
            auto line  = getCommandLine(context, ctrl, shift);

            // Split in command + parameters
            Utf8         cmd;
            Utf8         cmdExpr;
            vector<Utf8> cmds;
            if (!line.empty())
            {
                Utf8::tokenize(line, ' ', cmds);
                for (auto& c : cmds)
                    c.trim();
                cmd = cmds[0];
            }

            // Replace some stuff
            line.clear();
            bool err = false;
            for (int i = 0; i < cmds.size() && !err; i++)
            {
                // Make some replacements
                if (cmds[i] == "$sp")
                {
                    cmds[i] = Fmt("0x%llx", (uint64_t) context->sp);
                }
                else if (cmds[i] == "$bp")
                {
                    cmds[i] = Fmt("0x%llx", (uint64_t) context->bp);
                }
                else if (cmds[i].length() > 2 && cmds[i][0] == '$' && cmds[i][1] == 'r' && isNumber(cmds[i] + 2))
                {
                    int regN;
                    if (!getRegIdx(context, cmds[i] + 1, regN))
                    {
                        err = true;
                        continue;
                    }
                    auto& regP = context->getRegBuffer(context->debugCxtRc)[regN];
                    cmds[i]    = Fmt("0x%llx", regP.u64);
                }

                line += cmds[i];
                line += " ";

                if (i > 0)
                {
                    cmdExpr += cmds[i];
                    cmdExpr += " ";
                }
            }

            if (err)
                continue;

            line.trim();
            cmdExpr.trim();

            // Help
            /////////////////////////////////////////
            if (cmd == "?")
            {
                printHelp();
                continue;
            }

            // Print expression
            /////////////////////////////////////////
            if (cmd == "p" || cmd == "print")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                ValueFormat fmt;
                fmt.isHexa     = true;
                fmt.bitCount   = 64;
                bool hasFormat = false;
                if (cmds.size() > 1)
                {
                    if (getValueFormat(cmds[1], fmt))
                    {
                        hasFormat = true;
                        cmdExpr.clear();
                        for (int i = 2; i < cmds.size(); i++)
                            cmdExpr += cmds[i] + " ";
                        cmdExpr.trim();
                        if (cmdExpr.empty())
                            goto evalDefault;
                    }
                }

                EvaluateResult res;
                if (evalExpression(context, cmdExpr, res))
                {
                    if (!res.type->isVoid())
                    {
                        auto concrete = TypeManager::concreteType(res.type, CONCRETE_ALIAS);
                        Utf8 str;
                        if (hasFormat)
                        {
                            if (!concrete->isNativeIntegerOrRune() && !concrete->isNativeFloat())
                            {
                                g_Log.printColor(Fmt("cannot apply print format to type `%s`\n", concrete->getDisplayNameC()), LogColor::Red);
                                continue;
                            }

                            if (!res.addr && res.value)
                                res.addr = &res.value->reg;
                            appendLiteralValue(context, str, fmt, res.addr);
                        }
                        else
                        {
                            str = Fmt("(%s%s%s) ", COLOR_TYPE, res.type->getDisplayNameC(), COLOR_DEFAULT);
                            appendTypedValue(context, str, res, 0, hasFormat ? &fmt : nullptr);
                        }

                        g_Log.printColor(str);
                        str.trim();
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Execute expression
            /////////////////////////////////////////
            if (cmd == "e" || cmd == "exec" || cmd == "execute")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                EvaluateResult res;
                evalDynExpression(context, cmdExpr, res, CompilerAstKind::EmbeddedInstruction);
                continue;
            }

            // Info functions
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "func")
            {
                if (cmds.size() > 3)
                    goto evalDefault;
                auto           filter = cmds.size() == 3 ? cmds[2] : Utf8("");
                vector<string> all;
                g_Log.setColor(LogColor::Gray);
                for (auto m : g_Workspace->modules)
                {
                    for (auto bc : m->byteCodeFunc)
                    {
                        if (filter.empty() || bc->name.find(filter) != -1)
                        {
                            string          str = Fmt("%s%s%s ", COLOR_NAME, bc->getCallName().c_str(), COLOR_DEFAULT).c_str();
                            SourceFile*     bcFile;
                            SourceLocation* bcLocation;
                            ByteCode::getLocation(bc, bc->out, &bcFile, &bcLocation);
                            str += Fmt(" (%s%s%s) ", COLOR_TYPE, bc->getCallType()->getDisplayNameC(), COLOR_DEFAULT);
                            if (bcFile)
                                str += Fmt("%s", bcFile->name.c_str());
                            if (bcLocation)
                                str += Fmt(":%d", bcLocation->line);
                            all.push_back(str);
                        }
                    }
                }

                sort(all.begin(), all.end(), [](const string& a, const string& b)
                     { return a < b; });
                for (auto& o : all)
                {
                    if (OS::longOpStopKeyPressed())
                    {
                        g_Log.printColor("...stopped\n", LogColor::Red);
                        break;
                    }

                    g_Log.print(o);
                    g_Log.eol();
                }

                continue;
            }

            // Info modules
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "modules")
            {
                g_Log.setColor(LogColor::Gray);
                for (auto m : g_Workspace->modules)
                {
                    g_Log.print(Fmt("%s\n", m->name.c_str()));
                }

                continue;
            }

            // Info locals
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "locals")
            {
                if (context->debugCxtBc->localVars.empty())
                    g_Log.printColor("no locals\n");
                else
                {
                    for (auto l : context->debugCxtBc->localVars)
                    {
                        auto over = l->resolvedSymbolOverload;
                        if (!over)
                            continue;
                        // Generated
                        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
                            continue;
                        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_TYPE, over->typeInfo->getDisplayNameC(), COLOR_DEFAULT, COLOR_NAME, over->symbol->name.c_str(), COLOR_DEFAULT);
                        EvaluateResult res;
                        res.type = over->typeInfo;
                        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
                        appendTypedValue(context, str, res, 0);
                        g_Log.printColor(str);
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Info registers
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "regs")
            {
                if (cmds.size() > 3)
                    goto evalDefault;

                ValueFormat fmt;
                fmt.isHexa   = true;
                fmt.bitCount = 64;
                if (cmds.size() > 2)
                {
                    if (!getValueFormat(cmds[2], fmt))
                        goto evalDefault;
                }

                g_Log.setColor(LogColor::Gray);
                for (int i = 0; i < context->getRegCount(context->debugCxtRc); i++)
                {
                    auto& regP = context->getRegBuffer(context->debugCxtRc)[i];
                    Utf8  str;
                    appendLiteralValue(context, str, fmt, &regP);
                    str.trim();
                    g_Log.print(Fmt("%s$r%d%s = ", COLOR_NAME, i, COLOR_DEFAULT));
                    g_Log.print(str);
                    g_Log.eol();
                }

                g_Log.print(Fmt("%s$sp%s = 0x%016llx\n", COLOR_NAME, COLOR_DEFAULT, context->sp));
                g_Log.print(Fmt("%s$bp%s = 0x%016llx\n", COLOR_NAME, COLOR_DEFAULT, context->bp));
                continue;
            }

            // Info args
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "args")
            {
                auto funcDecl = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
                if (!funcDecl->parameters || funcDecl->parameters->childs.empty())
                    g_Log.printColor("no arguments\n");
                else
                {
                    for (auto l : funcDecl->parameters->childs)
                    {
                        auto over = l->resolvedSymbolOverload;
                        if (!over)
                            continue;
                        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_TYPE, over->typeInfo->getDisplayNameC(), COLOR_DEFAULT, COLOR_NAME, over->symbol->name.c_str(), COLOR_DEFAULT);
                        EvaluateResult res;
                        res.type = over->typeInfo;
                        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
                        appendTypedValue(context, str, res, 0);
                        g_Log.printColor(str);
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Print memory
            /////////////////////////////////////////
            if (cmd == "x")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                printMemory(context, cmdExpr);
                continue;
            }

            // Print stack
            /////////////////////////////////////////
            if (cmd == "bt")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                g_ByteCodeStackTrace->currentContext = context;
                g_ByteCodeStackTrace->log();
                continue;
            }

            // Set stack frame
            /////////////////////////////////////////
            if (cmd == "frame")
            {
                if (cmds.size() == 1)
                    goto evalDefault;
                if (cmds.size() > 2)
                    goto evalDefault;

                if (!isNumber(cmds[1]))
                {
                    g_Log.printColor("invalid 'frame' number\n", LogColor::Red);
                    continue;
                }

                uint32_t off      = atoi(cmds[1].c_str());
                uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
                off               = min(off, maxLevel);

                auto oldIndex                  = context->debugStackFrameOffset;
                context->debugStackFrameOffset = maxLevel - off;
                computeDebugContext(context);
                if (!context->debugCxtIp)
                {
                    context->debugStackFrameOffset = oldIndex;
                    computeDebugContext(context);
                    g_Log.printColor("this frame is external; you cannot go there\n", LogColor::Red);
                    continue;
                }

                printContextInstruction(context);
                continue;
            }

            // Stack frame up
            /////////////////////////////////////////
            if (cmd == "u" || cmd == "up")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
                if (context->debugStackFrameOffset == maxLevel)
                    g_Log.printColor("initial frame selected; you cannot go up\n", LogColor::Red);
                else
                {
                    auto oldIndex = context->debugStackFrameOffset;
                    context->debugStackFrameOffset += off;
                    computeDebugContext(context);
                    if (!context->debugCxtIp)
                    {
                        context->debugStackFrameOffset = oldIndex;
                        computeDebugContext(context);
                        g_Log.printColor("the up frame is external; you cannot go there\n", LogColor::Red);
                        continue;
                    }

                    printContextInstruction(context);
                }

                continue;
            }

            // Stack frame down
            /////////////////////////////////////////
            if (cmd == "d" || cmd == "down")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                if (context->debugStackFrameOffset == 0)
                    g_Log.printColor("bottom(innermost) frame selected; you cannot go down\n", LogColor::Red);
                else
                {
                    auto oldIndex = context->debugStackFrameOffset;
                    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
                    computeDebugContext(context);
                    if (!context->debugCxtIp)
                    {
                        context->debugStackFrameOffset = oldIndex;
                        computeDebugContext(context);
                        g_Log.printColor("the down frame is external; you cannot go there\n", LogColor::Red);
                        continue;
                    }

                    printContextInstruction(context);
                }

                continue;
            }

            // Exit
            /////////////////////////////////////////
            if (cmd == "q" || cmd == "quit")
            {
                if (cmds.size() != 1)
                    goto evalDefault;
                g_Log.setDefaultColor();
                return false;
            }

            // Print current instruction
            /////////////////////////////////////////
            if (cmd == "i")
            {
                int regN = 4;

                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                if (cmds.size() == 2)
                    regN = atoi(cmds[1].c_str());

                printInstructions(context, context->debugCxtBc, context->debugCxtIp, regN);
                continue;
            }

            // Print context
            /////////////////////////////////////////
            if (cmd == "w" || cmd == "where")
            {
                if (cmds.size() != 1)
                    goto evalDefault;
                printWhere(context);
                continue;
            }

            // Bytecode dump
            /////////////////////////////////////////
            if (cmd == "ii")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                context->debugCxtBc->print(context->debugCxtIp);
                continue;
            }

            // Bc mode
            /////////////////////////////////////////
            if (cmd == "bcmode")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                context->debugBcMode = !context->debugBcMode;
                if (context->debugBcMode)
                    g_Log.printColor("=> bytecode mode\n", LogColor::Gray);
                else
                    g_Log.printColor("=> source code mode\n", LogColor::Gray);
                printContextInstruction(context, true);
                continue;
            }

            // Function code
            /////////////////////////////////////////
            if (cmd == "l" || cmd == "list" || cmd == "ll")
            {
                if (cmd == "l" || cmd == "list")
                {
                    if (cmds.size() > 2)
                        goto evalDefault;
                    if (cmds.size() > 1 && !isNumber(cmds[1]))
                        goto evalDefault;
                }

                auto toLogBc = context->debugCxtBc;
                auto toLogIp = context->debugCxtIp;

                if (cmd == "ll" && cmds.size() > 1)
                {
                    if (cmds.size() > 2)
                        goto evalDefault;
                    auto name = cmds[1];
                    auto bc   = g_Workspace->findBc(name);
                    if (bc)
                    {
                        toLogBc = bc;
                        toLogIp = bc->out;
                    }
                    else
                    {
                        g_Log.printColor(Fmt("cannot find function '%s'\n", name.c_str()), LogColor::Red);
                        continue;
                    }
                }

                if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->sourceFile)
                {
                    SourceFile*     curFile;
                    SourceLocation* curLocation;
                    ByteCode::getLocation(toLogBc, toLogIp, &curFile, &curLocation);

                    if (cmd == "l")
                    {
                        uint32_t offset = 3;
                        if (cmds.size() == 2)
                            offset = atoi(cmds[1]);
                        printSourceLines(toLogBc->node->sourceFile, curLocation, offset);
                    }
                    else
                    {
                        auto     funcNode  = CastAst<AstFuncDecl>(toLogBc->node, AstNodeKind::FuncDecl);
                        uint32_t startLine = toLogBc->node->token.startLocation.line;
                        uint32_t endLine   = funcNode->content->token.endLocation.line;
                        printSourceLines(toLogBc->node->sourceFile, curLocation, startLine, endLine);
                    }
                }
                else
                    g_Log.printColor("no source code\n", LogColor::Red);
                continue;
            }

            // Breakpoints
            /////////////////////////////////////////
            if (cmd == "b" || cmd == "break" || cmd == "tb" || cmd == "tbreak")
            {
                if (cmds.size() == 1)
                {
                    printBreakpoints(context);
                    continue;
                }

                if (cmds.size() > 3)
                    goto evalDefault;

                bool oneShot = false;
                if (cmd == "tb" || cmd == "tbreak")
                    oneShot = true;

                // At line
                vector<Utf8> fileFunc;
                Utf8::tokenize(cmds[1], ':', fileFunc);
                if (cmds.size() == 2 && ((fileFunc.size() == 1 && isNumber(cmds[1])) || (fileFunc.size() == 2 && isNumber(fileFunc[1]))))
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
                                continue;
                            }
                            bkp.name = f->name;
                        }
                    }

                    int lineNo     = fileFunc.size() == 2 ? atoi(fileFunc[1]) : atoi(cmds[1]);
                    bkp.type       = ByteCodeRunContext::DebugBkpType::FileLine;
                    bkp.line       = lineNo;
                    bkp.autoRemove = oneShot;
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("breakpoint #%d, file '%s', line '%d'\n", context->debugBreakpoints.size(), curFile->name.c_str(), lineNo), LogColor::Gray);
                    continue;
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

                    continue;
                }

                if (cmds.size() == 2)
                    goto evalDefault;

                // Break on function
                if (cmds[1] == "func")
                {
                    ByteCodeRunContext::DebugBreakpoint bkp;
                    bkp.type = ByteCodeRunContext::DebugBkpType::FuncName;
                    auto bc  = g_Workspace->findBc(cmds[2]);
                    if (!bc)
                    {
                        g_Log.printColor(Fmt("cannot find function '%s'\n", cmds[2].c_str()), LogColor::Red);
                        continue;
                    }

                    bkp.name       = cmds[2];
                    bkp.autoRemove = oneShot;
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("breakpoint #%d entering function '%s'\n", context->debugBreakpoints.size(), bkp.name.c_str()), LogColor::Gray);
                    continue;
                }

                if (!isNumber(cmds[2]))
                    goto evalDefault;

                // Clear one breakpoint
                if (cmds[1] == "clear")
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints.erase(context->debugBreakpoints.begin() + numB - 1);
                        g_Log.printColor(Fmt("breakpoint #%d has been removed\n", numB), LogColor::Gray);
                    }

                    continue;
                }

                // Disable one breakpoint
                if (cmds[1] == "disable")
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints[numB - 1].disabled = true;
                        g_Log.printColor(Fmt("breakpoint #%d has been disabled\n", numB), LogColor::Gray);
                    }

                    continue;
                }

                // Enable one breakpoint
                if (cmds[1] == "enable")
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints[numB - 1].disabled = false;
                        g_Log.printColor(Fmt("breakpoint #%d has been enabled\n", numB), LogColor::Gray);
                    }

                    continue;
                }
            }

            // Next instruction
            /////////////////////////////////////////
            if (cmd.empty())
            {
                if (context->debugBcMode)
                    break;

                context->debugStackFrameOffset = 0;
                if (shift)
                {
                    context->debugStepRC   = context->curRC;
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                }
                else
                {
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLine;
                }

                break;
            }

            // Step to next line
            /////////////////////////////////////////
            if (cmd == "s" || cmd == "step")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
                break;
            }

            // Step to next line, step out
            /////////////////////////////////////////
            if (cmd == "n" || cmd == "next")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                context->debugStepRC           = context->curRC;
                break;
            }

            // Run to the end of the current function
            /////////////////////////////////////////
            if (cmd == "f" || cmd == "finish")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC           = context->curRC - 1;
                break;
            }

            // Continue
            /////////////////////////////////////////
            if (cmd == "c" || cmd == "cont" || cmd == "continue")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                g_Log.printColor("continue...\n", LogColor::Gray);
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
                context->debugOn               = false;
                break;
            }

            // Jump to line/instruction
            /////////////////////////////////////////
            if (cmd == "j" || cmd == "jump")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() > 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                context->debugStackFrameOffset = 0;

                uint32_t to = (uint32_t) atoi(cmds[1]);
                if (context->debugBcMode)
                {
                    if (to >= context->bc->numInstructions - 1)
                    {
                        g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
                        continue;
                    }

                    context->ip         = context->bc->out + to;
                    context->debugCxtIp = context->ip;
                }
                else
                {
                    auto curIp = context->bc->out;
                    while (true)
                    {
                        if (curIp >= context->bc->out + context->bc->numInstructions - 1)
                        {
                            g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
                            break;
                        }

                        SourceFile*     file;
                        SourceLocation* location;
                        ByteCode::getLocation(context->bc, curIp, &file, &location);
                        if (location && location->line == to)
                        {
                            context->ip         = curIp;
                            context->debugCxtIp = context->ip;
                            break;
                        }

                        curIp++;
                    }
                }

                printContextInstruction(context);
                continue;
            }

            // Run until a line/instruction is reached
            /////////////////////////////////////////
            if (cmd == "un" || cmd == "until")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() > 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                ByteCodeRunContext::DebugBreakpoint bkp;
                if (context->debugBcMode)
                    bkp.type = ByteCodeRunContext::DebugBkpType::InstructionIndex;
                else
                    bkp.type = ByteCodeRunContext::DebugBkpType::FileLine;
                bkp.name       = context->debugStepLastFile->name;
                bkp.line       = atoi(cmds[1]);
                bkp.autoRemove = true;
                addBreakpoint(context, bkp);
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
                break;
            }

        evalDefault:
            // Default to 'print' / 'execute'
            /////////////////////////////////////////
            line.trim();
            if (line[0] == '$')
                line.remove(0, 1);
            if (line.empty())
                continue;
            EvaluateResult res;
            if (evalExpression(context, line, res, true))
            {
                if (!res.type->isVoid())
                {
                    Utf8 str = Fmt("(%s%s%s) ", COLOR_TYPE, res.type->getDisplayNameC(), COLOR_DEFAULT);
                    appendTypedValue(context, str, res, 0);
                    g_Log.printColor(str);
                    if (str.back() != '\n')
                        g_Log.eol();
                }

                continue;
            }

            evalDynExpression(context, line, res, CompilerAstKind::EmbeddedInstruction);
        }
    }

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
