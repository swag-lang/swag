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

ByteCodeDebugger g_ByteCodeDebugger;

static constexpr int LINE_W = 71;

void ByteCodeDebugger::setup()
{
    if (!commands.empty())
        return;

    commands.push_back({.category = "stepping and continuing", .name = "step", .shortname = "s", .subcommand = "", .args = "[count]", .help = "Execute [count] source line(s) or bytecode instructions(s)", .cb = cmdStep});
    commands.push_back({.category = "stepping and continuing", .name = "next", .shortname = "n", .subcommand = "", .args = "[count]", .help = "Like 'step', but does not enter functions or inlined code", .cb = cmdNext});
    commands.push_back({.category = "stepping and continuing", .name = "until", .shortname = "u", .subcommand = "", .args = "<line|instruction>", .help = "Runs until the given <line> or <instruction> (in the current function) has been reached", .cb = cmdUntil});
    commands.push_back({.category = "stepping and continuing", .name = "jump", .shortname = "j", .subcommand = "", .args = "<line|instruction>", .help = "Jump to the given <line> or <instruction> (in the current function)", .cb = cmdJump});
    commands.push_back({.category = "stepping and continuing", .name = "finish", .shortname = "f", .subcommand = "", .args = "", .help = "Continue running until the current function is done", .cb = cmdFinish});
    commands.push_back({.category = "stepping and continuing", .name = "continue", .shortname = "c", .subcommand = "", .args = "", .help = "Continue running until another breakpoint is reached", .cb = cmdContinue});

    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "<subcommand>", .args = "", .help = "Add or remove breakpoints", .cb = nullptr});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "", .args = "", .help = "Print all breakpoints", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "func|f", .args = "<name>", .help = "Add a breakpoint when entering function containing <name>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "line", .args = "<line>", .help = "Add a breakpoint in the current source file at <line>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "instruction|inst", .args = "<index>", .help = "Add an instruction breakpoint in the current function at <index>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "file", .args = "<file> <line>", .help = "Add a breakpoint in <file> at <line>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "clear|cl", .args = "", .help = "Remove all breakpoints", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "clear|cl", .args = "<num>", .help = "Remove breakpoint <num>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "enable|en", .args = "<num>", .help = "Enable breakpoint <num>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "disable|dis", .args = "<num>", .help = "Disable breakpoint <num>", .cb = cmdBreak});
    commands.push_back({.category = "breakpoints", .name = "break", .shortname = "b", .subcommand = "memory|mem", .args = "<address> [--size]", .help = "Add a memory breakpoint", .cb = cmdBreakMemory});
    commands.push_back({.category = "breakpoints", .name = "tbreak", .shortname = "tb", .subcommand = "", .args = "", .help = "Same as 'break' except that the breakpoint will be automatically removed on hit", .cb = cmdBreak});

    commands.push_back({.category = "source code", .name = "list", .shortname = "l", .subcommand = "", .args = "[num]", .help = "Print the current source code line and [num] lines around", .cb = cmdList});
    commands.push_back({.category = "source code", .name = "longlist", .shortname = "ll", .subcommand = "", .args = "", .help = "Print the source code of the current function", .cb = cmdLongList});
    commands.push_back({.category = "source code", .name = "instruction", .shortname = "i", .subcommand = "", .args = "[num]", .help = "Print the current bytecode instruction and [num] instructions around", .cb = cmdInstruction});
    commands.push_back({.category = "source code", .name = "longinst", .shortname = "li", .subcommand = "", .args = "", .help = "Print the bytecode of the current function", .cb = cmdInstructionDump});
    commands.push_back({.category = "source code", .name = "source", .shortname = "sc", .subcommand = "", .args = "<symbol>", .help = "Print the source code of <symbol>, if applicable", .cb = cmdSource});

    commands.push_back({.category = "stack", .name = "backtrace", .shortname = "bt", .subcommand = "", .args = "[num]", .help = "Print [num] frames (0 for all)", .cb = cmdBackTrace});
    commands.push_back({.category = "stack", .name = "up", .shortname = "", .subcommand = "", .args = "[num]", .help = "Move stack frame [num] level up", .cb = cmdFrameUp});
    commands.push_back({.category = "stack", .name = "down", .shortname = "", .subcommand = "", .args = "[num]", .help = "Move stack frame [num] level down", .cb = cmdFrameDown});
    commands.push_back({.category = "stack", .name = "frame", .shortname = "", .subcommand = "", .args = "<num>", .help = "Set stack frame to level <num>", .cb = cmdFrame});

    commands.push_back({.category = "examining state", .name = "print", .shortname = "p", .subcommand = "", .args = "<expr> [--format]", .help = "Print the result of the expression <expr> in the current context", .cb = cmdPrint});
    commands.push_back({.category = "examining state", .name = "examine", .shortname = "x", .subcommand = "", .args = "<address> [--num] [--format]", .help = "Print memory at the given <address>", .cb = cmdMemory});
    commands.push_back({.category = "examining state", .name = "display", .shortname = "d", .subcommand = "<subcommand>", .args = "", .help = "Automatic display of some expressions", .cb = nullptr});
    commands.push_back({.category = "examining state", .name = "display", .shortname = "d", .subcommand = "", .args = "", .help = "Print all expressions to display", .cb = cmdDisplay});
    commands.push_back({.category = "examining state", .name = "display", .shortname = "d", .subcommand = "", .args = "<expr> [--format]", .help = "Same as 'print', but will be done at each step", .cb = cmdDisplay});
    commands.push_back({.category = "examining state", .name = "display", .shortname = "d", .subcommand = "clear|cl", .args = "", .help = "Remove all expressions to display", .cb = cmdDisplay});
    commands.push_back({.category = "examining state", .name = "display", .shortname = "d", .subcommand = "clear|cl", .args = "<num>", .help = "Remove expression <num>", .cb = cmdDisplay});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "<subcommand>", .args = "", .help = "Display various information", .cb = nullptr});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "registers", .args = "[--format]", .help = "Print all registers", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "segments", .args = "", .help = "Print all segments addresses", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "expressions", .args = "", .help = "Print all dynamic expressions", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "values", .args = "", .help = "Print all automatic expression values", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "modules", .args = "", .help = "Print all modules", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "files", .args = "[filter]", .help = "Print all files", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "locals", .args = "[filter]", .help = "Print all current local variables", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "arguments", .args = "[filter]", .help = "Print all current function arguments", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "functions", .args = "[filter]", .help = "Print all functions", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "globals", .args = "[filter]", .help = "Print all global variables", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "show", .shortname = "o", .subcommand = "types", .args = "[filter]", .help = "Print all exported types", .cb = cmdShow});
    commands.push_back({.category = "examining state", .name = "where", .shortname = "wh", .subcommand = "", .args = "", .help = "Print some information about the current execution context", .cb = cmdWhere});

    commands.push_back({.category = "modifying state", .name = "execute", .shortname = "e", .subcommand = "", .args = "<stmt>", .help = "Execute the code statement <stmt> in the current context", .cb = cmdExecute});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "<subcommand>", .args = "", .help = "Change bcdbg behaviour", .cb = nullptr});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "", .args = "", .help = "Print all the current 'set' values", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "bkp", .args = "[on|off]", .help = "Enable or disable @breakpoint()", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "print", .args = "struct [on|off]", .help = "Display the content of structs when printing values", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "print", .args = "array [on|off]", .help = "Display the content of arrays when printing values", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "print", .args = "eval [on|off]", .help = "Display the bytecode content of a debugger expression to evaluate", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "print", .args = "compsym [on|off]", .help = "Display compiler symbols when listing symbols", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "bytecode", .args = "source [on|off]", .help = "Display source code when printing bytecode", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "backtrace", .args = "source [on|off]", .help = "Display source code when printing stack frames", .cb = cmdSet});
    commands.push_back({.category = "modifying state", .name = "set", .shortname = "", .subcommand = "register", .args = "<$reg> <expr>", .help = "Modify the given register <$reg> with the result of <expr>", .cb = cmdSet});

    commands.push_back({.category = "bcdbg", .name = "help", .shortname = "?", .subcommand = "", .args = "[command] [subcommand]", .help = "Display help", .cb = nullptr});
    commands.push_back({.category = "bcdbg", .name = "help", .shortname = "?", .subcommand = "", .args = "", .help = "Print this list of commands", .cb = cmdHelp});
    commands.push_back({.category = "bcdbg", .name = "help", .shortname = "?", .subcommand = "<command>", .args = "", .help = "Print help about a specific command", .cb = cmdHelp});
    commands.push_back({.category = "bcdbg", .name = "help", .shortname = "?", .subcommand = "<command> <subcommand>", .args = "", .help = "Print help about a specific command and sub command", .cb = cmdHelp});
    commands.push_back({.category = "bcdbg", .name = "cls", .shortname = "", .subcommand = "", .args = "", .help = "Clear the console", .cb = cmdClear});
    commands.push_back({.category = "bcdbg", .name = "quit", .shortname = "q", .subcommand = "", .args = "", .help = "Quit the compiler", .cb = cmdQuit});
}

bool ByteCodeDebugger::testNameFilter(const Utf8& name, const Utf8& filter, const Utf8& alternate)
{
    if (filter.empty())
        return true;
    if (name == filter)
        return true;

    bool atStart = false;
    bool atEnd   = false;

    Utf8 test = filter;
    if (test[0] == '^')
    {
        atStart = true;
        test    = Utf8{filter.buffer + 1, filter.length() - 1};
    }

    if (test.back() == '$')
    {
        atEnd = true;
        test  = Utf8{filter.buffer, filter.length() - 1};
    }

    const int pos = name.find(test);
    if (pos != -1)
    {
        if (atStart && pos != 0)
            return false;
        if (atEnd && pos != static_cast<int>(name.length()) - static_cast<int>(filter.length()) + 1)
            return false;
        return true;
    }

    if (!alternate.empty() && alternate.find(test) != -1)
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

    for (uint32_t i = maxLevel; i != UINT32_MAX; i--)
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

    if (tokens[0] == "?" || tokens[0] == "help")
        tokens.erase(tokens.begin());
    if (tokens.empty())
        return "";

    toComplete = tokens.back();

    // Command
    if (tokens.size() == 1)
    {
        for (const auto& c : commands)
        {
            Utf8 cmd{c.name};
            if (cmd.find(toComplete) == 0)
                return c.name;
        }

        return "";
    }

    // Sub command
    if (tokens.size() == 2)
    {
        for (const auto& c : commands)
        {
            if (tokens[0] == c.name || tokens[0] == c.shortname)
            {
                if (c.subcommand[0] == 0)
                    continue;

                Vector<Utf8> subCmds;
                Utf8::tokenize(c.subcommand, '|', subCmds);
                for (const auto& c1 : subCmds)
                {
                    if (c1.find(toComplete) == 0)
                        return c1;
                }
            }
        }
    }

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
#define IS_WORD(__i) (isalnum(line[__i]) || line[__i] == '_' || isdigit(line[__i]))

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
                    else if (IS_WORD(cursorX - 1))
                    {
                        while (cursorX && IS_WORD(cursorX - 1))
                            MOVE_LEFT();
                    }
                    else
                    {
                        while (cursorX && !IS_WORD(cursorX - 1))
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
                    else if (IS_WORD(cursorX))
                    {
                        while (cursorX != line.count && IS_WORD(cursorX))
                            MOVE_RIGHT();
                    }
                    else
                    {
                        while (cursorX != line.count && !IS_WORD(cursorX))
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

        case DebugStepMode::NextLineStepOut:
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

            if (lastBreakIp->node->hasOwnerInline())
            {
                // If I am still in an inline, but not in a mixin block, and was in a mixin block, zap
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

            [[fallthrough]];
        case DebugStepMode::NextLineStepIn:
        {
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
            if (lastBreakIp->node->safeOwnerInline() == ip->node->safeOwnerInline() && ip->node->safeOwnerInline())
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

void ByteCodeDebugger::enterDebugger(const ByteCodeRunContext* context)
{
    g_Log.setColor(LogColor::Gray);

    g_Log.writeEol();
    for (int i = 0; i < LINE_W; i++)
        g_Log.print(LogSymbol::HorizontalLine2);
    g_Log.writeEol();

    g_Log.write("entering bytecode debugger, type '?' for help\n");

    for (int i = 0; i < LINE_W; i++)
        g_Log.print(LogSymbol::HorizontalLine);
    g_Log.writeEol();

    g_Log.print(form("build configuration            = [[%s]]\n", g_CommandLine.buildCfg.cstr()));

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
            enterDebugger(context);
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
    printOneStepContext(context);

    while (true)
    {
        // Promptq
        /////////////////////////////////////////

        g_Log.setColor(LogColor::Green);
        if (bcMode)
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

        switch (result)
        {
            case BcDbgCommandResult::BadArguments:
                printCmdError(form("bad arguments for command [[%s]]", arg.cmd.cstr()));
                break;
            case BcDbgCommandResult::NotEnoughArguments:
                printCmdError(form("not enough arguments for command [[%s]]", arg.cmd.cstr()));
                break;
            case BcDbgCommandResult::TooManyArguments:
                printCmdError(form("too many arguments for command [[%s]]", arg.cmd.cstr()));
                break;
            case BcDbgCommandResult::Invalid:
                printCmdError(form("unknown debugger command [[%s]]", arg.cmd.cstr()));
                break;
        }

        continue;
    }

    lastCurRc = context->curRC;
    lastIp    = ip;
    return true;
}

bool ByteCodeDebugger::replaceSegmentPointer(Utf8& result, const Utf8& name, SegmentKind kind, bool& err) const
{
    if (!cxtBc || !cxtBc->sourceFile || !cxtBc->sourceFile->module)
        return false;

    const auto seg     = cxtBc->sourceFile->module->getSegment(kind);
    const Utf8 segName = seg->shortName;

    if (!name.startsWith(segName))
        return false;

    auto cpy = name;
    cpy.remove(0, segName.length());
    uint32_t idx = cpy.empty() ? 0 : cpy.toInt();

    for (const auto& b : seg->buckets)
    {
        if (!idx)
        {
            result += form("0x%llx", b.buffer);
            return true;
        }

        idx--;
    }

    err = true;
    printCmdError(form("no corresponding [[%s]] segment pointer", segName.cstr()));
    return false;
}

bool ByteCodeDebugger::commandSubstitution(ByteCodeRunContext* context, Utf8& cmdExpr) const
{
    bool recom = true;
    while (recom)
    {
        recom = false;
        Utf8 result;
        result.reserve(cmdExpr.length());

        auto pz = cmdExpr.cstr();
        while (*pz)
        {
            if (*pz != '$')
            {
                result += *pz++;
                continue;
            }

            Utf8 what;
            auto pze = pz + 1;
            while (SWAG_IS_AL_NUM(*pze))
            {
                what += *pze;
                pze++;
            }

            if (SWAG_IS_BLANK(*pze) || *pze == 0)
            {
                if (what == "sp")
                {
                    result += form("0x%llx", context->sp);
                    pz += 3;
                    continue;
                }

                if (what == "bp")
                {
                    result += form("0x%llx", context->bp);
                    pz += 3;
                    continue;
                }

                if (what == "rr0")
                {
                    result += form("0x%llx", context->registersRR[0].u64);
                    pz += 4;
                    continue;
                }

                if (what == "rr1")
                {
                    result += form("0x%llx", context->registersRR[1].u64);
                    pz += 4;
                    continue;
                }

                bool err = false;
                if (replaceSegmentPointer(result, what, SegmentKind::Bss, err) ||
                    replaceSegmentPointer(result, what, SegmentKind::Data, err) ||
                    replaceSegmentPointer(result, what, SegmentKind::Compiler, err) ||
                    replaceSegmentPointer(result, what, SegmentKind::Constant, err) ||
                    replaceSegmentPointer(result, what, SegmentKind::Tls, err))
                {
                    pz += what.length() + 1;
                    continue;
                }
                if (err)
                    return false;
            }

            // Register number
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

            // Historical expression
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
                    printCmdError(form("invalid expression number [[$%s]] (out of range)", value.cstr()));
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
