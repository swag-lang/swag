#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Report/Log.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/SemanticJob.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

ByteCodeDebugger g_ByteCodeDebugger;

static constexpr int LINE_W = 71;

void ByteCodeDebugger::setup()
{
    if (!commands.empty())
        return;

    commands.push_back({"<RET>", "", "", "repeat the last command", nullptr});
    commands.push_back({"<TAB>", "", "", "contextual completion of the current word", nullptr});
    commands.push_back({});

    commands.push_back({"bc", "", "", "swap between bytecode and source code mode", cmdBc});
    commands.push_back({});

    commands.push_back({"step", "s", "[count]", "execute [count] source line(s) or bytecode instructions(s)", cmdStep});
    commands.push_back({"next", "n", "[count]", "like 'step', but does not enter functions or inlined code", cmdNext});
    commands.push_back({"until", "u", "<line|instruction>", "runs until the given <line> or <instruction> (in the current function) has been reached", cmdUntil});
    commands.push_back({"jump", "j", "<line|instruction>", "jump to the given <line> or <instruction> (in the current function)", cmdJump});
    commands.push_back({});
    commands.push_back({"finish", "f", "", "continue running until the current function is done", cmdFinish});
    commands.push_back({"continue", "c", "", "continue running until another breakpoint is reached", cmdContinue});
    commands.push_back({});

    commands.push_back({"execute", "e", "<stmt>", "execute the code statement <stmt> in the current context", cmdExecute});
    commands.push_back({"print", "p", "<expr> [--format]", "print the result of the expression <expr> in the current context", cmdPrint});
    commands.push_back({});
    commands.push_back({"display", "d", "<expr> [--format]", "same as 'print', but will be done at each step", cmdDisplay});
    commands.push_back({"display", "", "", "print all expressions to display", cmdDisplay});
    commands.push_back({"display", "d", "(cl)ear", "remove all expressions to display", cmdDisplay});
    commands.push_back({"display", "d", "(cl)ear <num>", "remove expression <num>", cmdDisplay});
    commands.push_back({});

    commands.push_back({"x", "", "<address> [--num] [--format]", "print memory", cmdMemory});
    commands.push_back({});

    commands.push_back({"list", "l", "[num]", "print the current source code line and [num] lines around", cmdList});
    commands.push_back({"ll", "", "[name]", "print the current function (or function [name]) source code", cmdLongList});
    commands.push_back({});
    commands.push_back({"i", "", "[num]", "print the current bytecode instruction and [num] instructions around", cmdInstruction});
    commands.push_back({"ii", "", "[name]", "print the current function (or function [name]) bytecode", cmdInstructionDump});
    commands.push_back({});

    commands.push_back({"info", "o", "(reg)isters [--format]", "print all registers", cmdInfo});
    commands.push_back({"info", "o", "(loc)als [filter]", "print all current local variables", cmdInfo});
    commands.push_back({"info", "o", "(arg)uments [filter]", "print all current function arguments", cmdInfo});
    commands.push_back({"info", "o", "(func)tions [filter]", "print all functions", cmdInfo});
    commands.push_back({"info", "o", "(var)iables [filter]", "print all global variables", cmdInfo});
    commands.push_back({"info", "o", "(exp)ressions [filter]", "print all dynamic expressions", cmdInfo});
    commands.push_back({"info", "o", "modules", "print all modules", cmdInfo});
    commands.push_back({});
    commands.push_back({"where", "w", "", "print contextual information", cmdWhere});
    commands.push_back({});

    commands.push_back({"break", "b", "", "print all breakpoints", cmdBreak});
    commands.push_back({"break", "b", "(f)unc <name>", "add breakpoint when entering function with exact <name>", cmdBreak});
    commands.push_back({"break", "b", "(f)unc *<name>", "add breakpoint when entering function containing <name>", cmdBreak});
    commands.push_back({"break", "b", "line <line>", "add breakpoint in the current source file at <line>", cmdBreak});
    commands.push_back({"break", "b", "file <file> <line>", "add breakpoint in <file> at <line>", cmdBreak});
    commands.push_back({"break", "b", "(cl)ear", "remove all breakpoints", cmdBreak});
    commands.push_back({"break", "b", "(cl)ear <num>", "remove breakpoint <num>", cmdBreak});
    commands.push_back({"break", "b", "(en)able <num>", "enable breakpoint <num>", cmdBreak});
    commands.push_back({"break", "b", "(di)sable <num>", "disable breakpoint <num>", cmdBreak});
    commands.push_back({"tbreak", "tb", "", "same as 'break' except that the breakpoint will be automatically removed on hit", cmdBreak});
    commands.push_back({});

    commands.push_back({"bt", "", "", "backtrace, print callstack", cmdBackTrace});
    commands.push_back({"up", "", "[num]", "move stack frame [num] level up", cmdFrameUp});
    commands.push_back({"down", "", "[num]", "move stack frame [num] level down", cmdFrameDown});
    commands.push_back({"frame", "", "<num>", "set stack frame to level <num>", cmdFrame});
    commands.push_back({});

    commands.push_back({"set", "", "", "print all actual debug parameters", cmdSet});
    commands.push_back({"set", "", "bkp <on|off>", "enable/disable @breakpoint()", cmdSet});
    commands.push_back({"set", "", "print struct <on|off>", "print the content of structs when printing values", cmdSet});
    commands.push_back({"set", "", "print array <on|off>", "print the content of arrays when printing values", cmdSet});
    commands.push_back({"set", "", "print bccode <on|off>", "print also source code when printing bytecode", cmdSet});
    commands.push_back({});

    commands.push_back({"help", "?", "", "print this list of commands", cmdHelp});
    commands.push_back({"help", "?", "<command>", "print help about a specific command", cmdHelp});
    commands.push_back({"quit", "q", "", "quit the compiler", cmdQuit});
}

ByteCode* ByteCodeDebugger::findCmdBc(const Utf8& name)
{
    auto bc = findBc(name.c_str());
    if (bc.size() == 1)
        return bc[0];

    if (bc.size() > 1)
    {
        printCmdError("multiple functions");
        for (const auto one : bc)
            one->printName();
        return nullptr;
    }

    printCmdError(form("cannot find function [[%s]]", name.c_str()));
    return nullptr;
}

VectorNative<ByteCode*> ByteCodeDebugger::findBc(const char* bcName)
{
    VectorNative<Module*> modules;
    for (auto val : g_Workspace->mapModulesNames | std::views::values)
        modules.push_back(val);
    modules.push_back(g_Workspace->bootstrapModule);
    modules.push_back(g_Workspace->runtimeModule);

    VectorNative<ByteCode*> tryMatch;
    for (const auto m : modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (bc->getPrintName() == bcName)
                return {bc};

            if (bc->getPrintName().find(bcName) != -1)
                tryMatch.push_back(bc);
        }
    }

    if (tryMatch.size() == 1)
        return {tryMatch[0]};

    return tryMatch;
}

bool ByteCodeDebugger::testNameFilter(const Utf8& name, const Utf8& filter)
{
    if (filter.empty())
        return true;
    if (name == filter)
        return true;
    if (name.find(filter) != -1)
        return true;
    return false;
}

bool ByteCodeDebugger::getRegIdx(ByteCodeRunContext* context, const Utf8& arg, uint32_t& regN) const
{
    regN = static_cast<uint32_t>(arg.toInt(1));
    if (!context->getRegCount(cxtRc))
    {
        printCmdError("no available register");
        return false;
    }

    if (regN >= context->getRegCount(cxtRc))
    {
        printCmdError(form("invalid register number, maximum value is [[%u]]", context->getRegCount(cxtRc) - 1));
        return false;
    }

    return true;
}

void ByteCodeDebugger::computeDebugContext(ByteCodeRunContext* context)
{
    cxtBc    = context->bc;
    cxtIp    = context->ip;
    cxtRc    = context->curRC;
    cxtBp    = context->bp;
    cxtSp    = context->sp;
    cxtSpAlt = context->spAlt;
    cxtStack = context->stack;
    if (context->debugStackFrameOffset == 0)
        return;

    VectorNative<ByteCodeStackStep> steps;
    g_ByteCodeStackTrace->getSteps(steps, context);
    if (steps.empty())
        return;

    const uint32_t maxLevel        = g_ByteCodeStackTrace->maxLevel(context);
    context->debugStackFrameOffset = min(context->debugStackFrameOffset, maxLevel);
    uint32_t ns                    = 0;

    for (uint32_t i = maxLevel; i > UINT32_MAX; i--)
    {
        if (i >= steps.size())
            continue;

        const auto& step = steps[i];
        if (ns == context->debugStackFrameOffset)
        {
            cxtBc    = step.bc;
            cxtIp    = step.ip;
            cxtBp    = step.bp;
            cxtSp    = step.sp;
            cxtSpAlt = step.spAlt;
            cxtStack = step.stack;
            break;
        }

        ns++;
        if (!step.ip)
            continue;
        if (cxtRc)
            cxtRc--;
    }
}

Utf8 ByteCodeDebugger::completion(ByteCodeRunContext*, const Utf8& line, Utf8& toComplete) const
{
    Vector<Utf8> tokens;
    Utf8::tokenize(line, ' ', tokens);
    if (tokens.empty())
        return "";

    toComplete = tokens.back();

    SemanticContext                 semContext;
    SemanticJob                     semJob;
    VectorNative<CollectedScope>    scopeHierarchy;
    VectorNative<CollectedScopeVar> scopeHierarchyVars;
    semContext.sourceFile = cxtIp->node->token.sourceFile;
    semContext.node       = cxtIp->node;
    semContext.baseJob    = &semJob;

    if (Semantic::collectScopeHierarchy(&semContext, scopeHierarchy, scopeHierarchyVars, cxtIp->node, COLLECT_ALL))
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

Utf8 ByteCodeDebugger::getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift) const
{
    static Vector<Utf8> debugCmdHistory;
    static uint32_t     debugCmdHistoryIndex = 0;

    Utf8     line;
    uint32_t cursorX = 0;

    while (true)
    {
        int        c   = 0;
        const auto key = OS::promptChar(c, ctrl, shift);

        if (key == OS::Key::Return)
            break;

#define MOVE_LEFT()                      \
    do                                   \
    {                                    \
        (void) fputs("\x1B[1D", stdout); \
        cursorX--;                       \
    } while (0)
#define MOVE_RIGHT()                     \
    do                                   \
    {                                    \
        (void) fputs("\x1B[1C", stdout); \
        cursorX++;                       \
    } while (0)

        switch (key)
        {
            //////////////////////////////////
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

            //////////////////////////////////
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

            //////////////////////////////////
            case OS::Key::Home:
                if (cursorX)
                    (void) fputs(form("\x1B[%dD", cursorX), stdout); // Move the cursor at 0
                cursorX = 0;
                continue;

            //////////////////////////////////
            case OS::Key::End:
                if (cursorX != line.count)
                    (void) fputs(form("\x1B[%dC", line.count - cursorX), stdout); // Move the cursor to the end of line
                cursorX = line.count;
                continue;

            //////////////////////////////////
            case OS::Key::Delete:
                if (cursorX == line.count)
                    continue;
                (void) fputs("\x1B[1P", stdout); // Delete the character
                line.remove(cursorX, 1);
                continue;

            //////////////////////////////////
            case OS::Key::Back:
                if (!cursorX)
                    continue;
                MOVE_LEFT();
                (void) fputs("\x1B[1P", stdout); // Delete the character
                line.remove(cursorX, 1);
                continue;

            //////////////////////////////////
            case OS::Key::Up:
                if (debugCmdHistoryIndex == 0)
                    continue;
                if (cursorX) // Move the cursor at 0
                    (void) fputs(form("\x1B[%dD", cursorX), stdout);
                (void) fputs(form("\x1B[%dX", line.count), stdout); // Erase the current command
                line = debugCmdHistory[--debugCmdHistoryIndex];
                (void) fputs(line, stdout); // Insert command from history
                cursorX = line.count;
                break;

            //////////////////////////////////
            case OS::Key::Down:
                if (debugCmdHistoryIndex == debugCmdHistory.size())
                    continue;
                if (cursorX) // Move the cursor at 0
                    (void) fputs(form("\x1B[%dD", cursorX), stdout);
                (void) fputs(form("\x1B[%dX", line.count), stdout); // Erase the current command
                debugCmdHistoryIndex++;
                if (debugCmdHistoryIndex != debugCmdHistory.size())
                {
                    line = debugCmdHistory[debugCmdHistoryIndex];
                    (void) fputs(line, stdout); // Insert command from history
                    cursorX = line.count;
                }
                else
                {
                    line.clear();
                    cursorX = 0;
                }
                break;

            //////////////////////////////////
            case OS::Key::Escape:
                if (cursorX) // Move the cursor at 0
                    (void) fputs(form("\x1B[%dD", cursorX), stdout);
                (void) fputs(form("\x1B[%dX", line.count), stdout); // Erase the current command
                line.clear();
                cursorX = 0;
                break;

            //////////////////////////////////
            case OS::Key::PasteFromClipboard:
            {
                Utf8 str = OS::getClipboardString();
                (void) fputs(form("\x1B[%d@", str.length()), stdout); // Insert n blanks and shift right
                for (const auto cc : str)
                {
                    (void) fputc(cc, stdout);
                    line.insert(cursorX, cc);
                    cursorX++;
                }

                break;
            }

            //////////////////////////////////
            case OS::Key::Ascii:
                (void) fputs("\x1B[1@", stdout); // Insert n blanks and shift right
                (void) fputc(c, stdout);
                line.insert(cursorX, static_cast<char>(c));
                cursorX++;
                break;

            //////////////////////////////////
            case OS::Key::Tab:
            {
                if (!cxtIp || !cxtIp->node || !cxtIp->node->token.sourceFile)
                    continue;
                if (cursorX != line.count)
                    continue;

                Utf8 toComplete;
                auto n = completion(context, line, toComplete);
                if (!n.empty() && n != toComplete)
                {
                    n.remove(0, toComplete.length());
                    (void) fputs(n, stdout);
                    line += n;
                    cursorX += n.length();
                    continue;
                }

                break;
            }

            default:
                break;
        }
    }

    line.trim();
    if (!line.empty())
    {
        while (debugCmdHistory.size() != debugCmdHistoryIndex)
            debugCmdHistory.pop_back();
        debugCmdHistory.push_back(line);
        debugCmdHistoryIndex = debugCmdHistory.size();
    }

    g_Log.writeEol();
    return line;
}

bool ByteCodeDebugger::mustBreak(ByteCodeRunContext* context)
{
    const auto ip           = context->ip;
    bool       zapCurrentIp = false;

    switch (stepMode)
    {
        case DebugStepMode::ToNextBreakpoint:
            zapCurrentIp = true;
            break;

        case DebugStepMode::NextInstructionStepIn:
            if (stepCount == 0)
            {
                context->debugOn = true;
                stepMode         = DebugStepMode::None;
            }
            else
            {
                stepCount--;
                zapCurrentIp = true;
            }
            break;

        case DebugStepMode::NextInstructionStepOut:
            // If inside a sub function
            if (context->curRC > stepRc)
            {
                zapCurrentIp = true;
                break;
            }

            if (stepCount == 0)
            {
                context->debugOn = true;
                stepMode         = DebugStepMode::None;
            }
            else
            {
                stepCount--;
                zapCurrentIp = true;
            }
            break;

        case DebugStepMode::NextLineStepIn:
        {
            bcMode         = false;
            const auto loc = ByteCode::getLocation(context->bc, ip, true);
            if (!loc.file || !loc.location)
            {
                zapCurrentIp = true;
                break;
            }

            if (stepLastFile == loc.file && stepLastLocation && stepLastLocation->line == loc.location->line)
            {
                zapCurrentIp = true;
            }
            else if (stepCount == 0)
            {
                context->debugOn = true;
                stepMode         = DebugStepMode::None;
            }
            else
            {
                stepCount--;
                zapCurrentIp           = true;
                stepLastLocation->line = loc.location->line;
            }
            break;
        }
        case DebugStepMode::NextLineStepOut:
        {
            bcMode = false;

            // If inside a sub function
            if (context->curRC > stepRc)
            {
                zapCurrentIp = true;
                break;
            }

            if (!ip->node)
            {
                zapCurrentIp = true;
                break;
            }

            if (!lastBreakIp->node->hasOwnerInline() && ip->node->hasOwnerInline())
            {
                // Can only step into a mixin if we come from a not inlined block
                if (!ip->node->hasAstFlag(AST_IN_MIXIN))
                {
                    zapCurrentIp = true;
                    break;
                }

                const auto loc = ByteCode::getLocation(context->bc, ip, true);
                if (loc.file != stepLastFile)
                {
                    zapCurrentIp = true;
                    break;
                }
            }

            if (lastBreakIp->node->safeOwnerInline())
            {
                // If i am still in an inline, but not in a mixin block, and was in a mixin block, zap
                if (ip->node->safeOwnerInline() &&
                    lastBreakIp->node->hasAstFlag(AST_IN_MIXIN) &&
                    !ip->node->hasAstFlag(AST_IN_MIXIN))
                {
                    zapCurrentIp = true;
                    break;
                }

                const auto loc = ByteCode::getLocation(context->bc, ip, true);
                if (loc.file != stepLastFile)
                {
                    zapCurrentIp = true;
                    break;
                }
            }

            const auto loc = ByteCode::getLocation(context->bc, ip, true);
            if (!loc.file || !loc.location)
            {
                zapCurrentIp = true;
                break;
            }

            if (stepLastFile == loc.file && stepLastLocation && stepLastLocation->line == loc.location->line)
            {
                zapCurrentIp = true;
            }
            else if (stepCount == 0)
            {
                context->debugOn = true;
                stepMode         = DebugStepMode::None;
            }
            else
            {
                stepCount--;
                stepLastLocation->line = loc.location->line;
                zapCurrentIp           = true;
            }
            break;
        }
        case DebugStepMode::FinishedFunction:
        {
            // Top level function, break on ret
            if (context->curRC == 0 && stepRc == UINT32_MAX && ByteCode::isRet(ip))
            {
                stepMode = DebugStepMode::None;
                break;
            }

            // We are in a sub function
            if (context->curRC > stepRc)
            {
                zapCurrentIp = true;
                break;
            }

            // If last break was in an inline block, and we are in a sub inline block
            if (lastBreakIp->node->hasOwnerInline() && lastBreakIp->node->ownerInline()->isParentOf(ip->node->safeOwnerInline()))
            {
                zapCurrentIp = true;
                break;
            }

            // We are in the same inline block
            if (lastBreakIp->node->safeOwnerInline() == ip->node->safeOwnerInline())
            {
                zapCurrentIp = true;
                break;
            }

            context->debugOn = true;
            stepMode         = DebugStepMode::None;
            break;
        }
    }

    return !zapCurrentIp;
}

bool ByteCodeDebugger::step(ByteCodeRunContext* context)
{
    static std::mutex dbgMutex;
    ScopedLock        sc(dbgMutex);

    const auto ip = context->ip;

    if (context->debugEntry)
    {
        static bool firstOne = true;

        g_Log.setColor(LogColor::Gray);
        setup();

        if (firstOne)
        {
            firstOne = false;
            g_Log.setColor(LogColor::Gray);

            g_Log.writeEol();
            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine2);
            g_Log.writeEol();

            g_Log.write("entering bytecode debugger, type '?' for help\n");

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine);
            g_Log.writeEol();

            g_Log.print(form("build configuration            = [[%s]]\n", g_CommandLine.buildCfg.c_str()));

            const Module* module = nullptr;
            if (context->bc->sourceFile)
                module = context->bc->sourceFile->module;
            if (!module && context->bc->node && context->bc->node->token.sourceFile)
                module = context->bc->node->token.sourceFile->module;
            if (module)
            {
                g_Log.print(form("BuildCfg.byteCodeInline        = %s\n", module->buildCfg.byteCodeInline ? "true" : "false"));
                g_Log.print(form("BuildCfg.byteCodeOptimizeLevel = %d\n", module->buildCfg.byteCodeOptimizeLevel));
            }

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine2);
            g_Log.writeEol();
            g_Log.setColor(LogColor::Gray);
        }

        printMsgBkp("@breakpoint() hit");

        context->debugEntry            = false;
        context->debugStackFrameOffset = 0;
        stepMode                       = DebugStepMode::None;
        stepLastLocation               = nullptr;
        stepLastFile                   = nullptr;
        forcePrintContext              = true;
    }

    // Check breakpoints/step mode
    checkBreakpoints(context);
    if (!mustBreak(context))
    {
        lastCurRc = context->curRC;
        lastIp    = ip;
        return true;
    }

    computeDebugContext(context);
    printDebugContext(context);

    while (true)
    {
        // Bar
        /////////////////////////////////////////
        printSeparator();

        // Prompt
        /////////////////////////////////////////

        g_Log.setColor(LogColor::Green);
        if(bcMode)
            g_Log.write("(dbg-bc) > ");
        else
            g_Log.write("(dbg-source) > ");

        // Get command from user
        bool ctrl  = false;
        bool shift = false;
        auto line  = getCommandLine(context, ctrl, shift);

        if (line.empty())
        {
            line = lastLine;
            if (line.empty())
                continue;
        }

        lastLine    = line;
        lastBreakIp = ip;

        // Split in command + parameters
        BcDbgCommandArg arg;
        tokenizeCommand(line, arg);
        g_Log.writeEol();

        // Command
        /////////////////////////////////////////
        auto result = BcDbgCommandResult::Invalid;

        if (!arg.cmd.empty())
        {
            for (auto& c : commands)
            {
                if (c.cb == nullptr)
                    continue;

                if (arg.cmd == c.name || arg.cmd == c.shortname)
                {
                    result = c.cb(context, arg);
                    break;
                }
            }
        }

        if (result == BcDbgCommandResult::Break)
            break;
        if (result == BcDbgCommandResult::Return)
            return false;
        if (result == BcDbgCommandResult::BadArguments)
            printCmdError(form("bad [[%s]] arguments", arg.cmd.c_str()));
        else if (result == BcDbgCommandResult::Invalid)
            printCmdError(form("unknown debugger command [[%s]]", arg.cmd.c_str()));
        continue;
    }

    lastCurRc = context->curRC;
    lastIp    = ip;
    return true;
}

bool ByteCodeDebugger::commandSubstitution(ByteCodeRunContext* context, Utf8& cmdExpr) const
{
    bool recom = true;
    while (recom)
    {
        recom = false;
        Utf8 result;
        result.reserve(cmdExpr.length());

        auto pz = cmdExpr.c_str();
        while (*pz)
        {
            if (*pz != '$')
            {
                result += *pz++;
                continue;
            }

            if (pz[1] == 's' && pz[2] == 'p' && (SWAG_IS_BLANK(pz[3]) || !pz[3]))
            {
                result += form("0x%llx", context->sp);
                pz += 3;
                continue;
            }

            if (pz[1] == 'b' && pz[2] == 'p' && (SWAG_IS_BLANK(pz[3]) || !pz[3]))
            {
                result += form("0x%llx", context->bp);
                pz += 3;
                continue;
            }

            if (pz[1] == 'r' && SWAG_IS_DIGIT(pz[2]))
            {
                uint32_t regN;
                if (!getRegIdx(context, pz + 1, regN))
                    return false;

                const auto& regP = context->getRegBuffer(cxtRc)[regN];
                result += form("0x%llx", regP.u64);
                pz += 2;
                while (SWAG_IS_DIGIT(*pz))
                    pz++;
                continue;
            }

            if (SWAG_IS_DIGIT(pz[1]))
            {
                Utf8 value;
                pz += 1;
                while (SWAG_IS_DIGIT(*pz))
                {
                    value += *pz++;
                }

                const int num = value.toInt();
                if (num >= static_cast<int>(evalExpr.size()))
                {
                    printCmdError(form("invalid expression number [[$%s]] (out of range)", value.c_str()));
                    return false;
                }

                recom = true;
                result += evalExprResult[num];
                continue;
            }

            result += *pz++;
        }

        cmdExpr = result;
    }

    return true;
}

void ByteCodeDebugger::tokenizeCommand(const Utf8& line, BcDbgCommandArg& arg)
{
    arg.cmd.clear();
    arg.cmdExpr.clear();
    arg.split.clear();

    if (!line.empty())
    {
        Utf8::tokenize(line, ' ', arg.split);
        for (auto& c : arg.split)
            c.trim();
        arg.cmd = arg.split[0];

        for (uint32_t i = 1; i < arg.split.size(); i++)
        {
            const auto& cmd = arg.split[i];
            arg.cmdExpr += cmd;
            arg.cmdExpr += " ";
        }

        arg.cmdExpr.trim();
    }
}
