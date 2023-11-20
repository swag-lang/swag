#include "pch.h"
#include "ByteCode.h"
#include "Module.h"
#include "SemanticJob.h"
#include "ByteCodeDebugger.h"

ByteCodeDebugger g_ByteCodeDebugger;

void ByteCodeDebugger::setup()
{
    if (commands.size())
        return;

    // clang-format off
    commands.push_back({"<return>",    "",  "",                       "repeat the last command", nullptr});
    commands.push_back({"<tab>",       "",  "",                       "contextual completion of the current word", nullptr});
    commands.push_back({});

    commands.push_back({"step",        "s",    "",                    "continue to the next source line", cmdStep});
    commands.push_back({"next",        "n",    "",                    "like 'step', but does not enter functions or inlined code", cmdNext});
    commands.push_back({"until",       "u",    "<line>",              "runs until the given <line> in the current function has been reached", cmdUntil});
    commands.push_back({"jump",        "j",    "<line>",              "jump to the given <line> in the current function", cmdJump});
    commands.push_back({});  
    commands.push_back({"stepi",       "si",   "",                    "execute one bytecode instruction", cmdStepi});
    commands.push_back({"nexti",       "ni",   "",                    "like 'stepi', but does not enter functions or inlined code", cmdNexti});
    commands.push_back({"untili",      "ui",   "<instruction>",       "runs until the given bytecode <instruction> has been reached", cmdUntili});
    commands.push_back({"jumpi",       "ji",   "<instruction>",       "jump to the given bytecode instruction", cmdJumpi});
    commands.push_back({});  
    commands.push_back({"finish",      "f",    "",                    "continue running until the current function is done", cmdFinish});
    commands.push_back({"continue",    "c",    "",                    "continue running until another breakpoint is reached", cmdContinue});
    commands.push_back({});                    
                                               
    commands.push_back({"execute",     "e",    "<stmt>",              "execute the code statement <stmt> in the current context", cmdExecute});
    commands.push_back({"print",       "p",    "[/format] <expr>",    "print the result of the expression <expr> in the current context (format is the same as 'x' command)", cmdPrint});
    commands.push_back({});                    
                                               
    commands.push_back({"x",           "",     "[/format] [/num] <address>",     "print memory (format = s8|s16|s32|s64|u8|u16|u32|u64|x8|x16|x32|x64|f32|f64)", cmdMemory});
    commands.push_back({});                    
                                               
    commands.push_back({"list",        "l",    "[num]",               "print the current source code line and [num] lines around", cmdList});
    commands.push_back({"ll",          "",     "[name]",              "print the current function (or function [name]) source code", cmdLongList});
    commands.push_back({});  
    commands.push_back({"i",           "",     "[num]",               "print the current bytecode instruction and [num] instructions around", cmdInstruction});
    commands.push_back({"ii",          "",     "[name]",              "print the current function (or function [name]) bytecode", cmdInstructionDump});
    commands.push_back({});                                           
                                               
    commands.push_back({"info",        "o",    "(l)ocals",            "print all current local variables", cmdInfo});
    commands.push_back({"info",        "o",    "(a)rgs",              "print all current function arguments", cmdInfo});
    commands.push_back({"info",        "o",    "(br)eakpoints",       "print all breakpoints", cmdInfo});
    commands.push_back({"info",        "o",    "(r)egs [/format]",    "print all registers (format is the same as 'x' command)", cmdInfo});
    commands.push_back({"info",        "o",    "module",              "print all modules", cmdInfo});
    commands.push_back({"info",        "o",    "func [filter]",       "print all functions which contains [filter] in their names", cmdInfo});
    commands.push_back({});
    commands.push_back({"where",       "w",    "",                    "print contextual informations", cmdWhere});
    commands.push_back({});            
                                       
    commands.push_back({"break",       "b",    "",                    "print all breakpoints", cmdBreak});
    commands.push_back({"break",       "b",    "(f)unc <name>",       "add breakpoint when entering function with exact <name>", cmdBreak});
    commands.push_back({"break",       "b",    "(f)unc *<name>",      "add breakpoint when entering function containing <name>", cmdBreak});
    commands.push_back({"break",       "b",    "line <line>",         "add breakpoint in the current source file at <line>", cmdBreak});
    commands.push_back({"break",       "b",    "file <file> <line>",  "add breakpoint in <file> at <line>", cmdBreak});
    commands.push_back({"break",       "b",    "(cl)ear",             "remove all breakpoints", cmdBreak});
    commands.push_back({"break",       "b",    "(cl)ear   <num>",     "remove breakpoint <num>", cmdBreak});
    commands.push_back({"break",       "b",    "(en)able  <num>",     "enable breakpoint <num>", cmdBreak});
    commands.push_back({"break",       "b",    "(di)sable <num>",     "disable breakpoint <num>", cmdBreak});
    commands.push_back({"tbreak",      "tb",   "",                    "same as 'break' except that the breakpoint will be automatically removed on hit", cmdBreak});
    commands.push_back({});                                           
                                                                      
    commands.push_back({"bt",          "",     "",                    "backtrace, print callstack", cmdBackTrace});
    commands.push_back({"up",          "",     "[num]",               "move stack frame <num> level up", cmdFrameUp});
    commands.push_back({"down",        "",     "[num]",               "move stack frame <num> level down", cmdFrameDown});
    commands.push_back({"frame",       "",     "<num>",               "set stack frame to level <num>", cmdFrame});
    commands.push_back({});                                           
                                                                      
    commands.push_back({"mode",        "m",    "inline",              "swap between inline mode and non inline mode", cmdMode});
    commands.push_back({"mode",        "m",    "bkp",                 "enable/disable call to @breakpoint()", cmdMode});
    commands.push_back({});
                                                                     
    commands.push_back({"help",        "?",    "",                    "print this list of commands", cmdHelp});
    commands.push_back({"help",        "?",    "<command>",           "print help about a specific command", cmdHelp});
    commands.push_back({"quit",        "q",    "",                    "quit the compiler", cmdQuit});
    // clang-format on
}

bool ByteCodeDebugger::getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN)
{
    regN = atoi(arg.c_str() + 1);
    if (!context->getRegCount(debugCxtRc))
    {
        g_ByteCodeDebugger.printCmdError("no available register");
        return false;
    }

    if (regN >= context->getRegCount(debugCxtRc))
    {
        g_ByteCodeDebugger.printCmdError(Fmt("invalid register number, maximum value is '%u'", (uint32_t) context->getRegCount(debugCxtRc) - 1));
        return false;
    }

    return true;
}

void ByteCodeDebugger::computeDebugContext(ByteCodeRunContext* context)
{
    debugCxtBc    = context->bc;
    debugCxtIp    = context->ip;
    debugCxtRc    = context->curRC;
    debugCxtBp    = context->bp;
    debugCxtSp    = context->sp;
    debugCxtSpAlt = context->spAlt;
    debugCxtStack = context->stack;
    if (context->debugStackFrameOffset == 0)
        return;

    VectorNative<ByteCodeStackStep> steps;
    g_ByteCodeStackTrace->getSteps(steps, context);
    if (steps.empty())
        return;

    uint32_t maxLevel              = g_ByteCodeStackTrace->maxLevel(context);
    context->debugStackFrameOffset = min(context->debugStackFrameOffset, maxLevel);
    uint32_t ns                    = 0;

    for (int i = (int) maxLevel; i >= 0; i--)
    {
        if (i >= (int) steps.size())
            continue;

        auto& step = steps[i];
        if (ns == context->debugStackFrameOffset)
        {
            debugCxtBc    = step.bc;
            debugCxtIp    = step.ip;
            debugCxtBp    = step.bp;
            debugCxtSp    = step.sp;
            debugCxtSpAlt = step.spAlt;
            debugCxtStack = step.stack;
            break;
        }

        ns++;
        if (!step.ip)
            continue;
        if (debugCxtRc)
            debugCxtRc--;
    }
}

Utf8 ByteCodeDebugger::completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete)
{
    Vector<Utf8> tokens;
    Utf8::tokenize(line, ' ', tokens);
    if (tokens.empty())
        return "";

    toComplete = tokens.back();

    SemanticContext                   semContext;
    SemanticJob                       semJob;
    VectorNative<AlternativeScope>    scopeHierarchy;
    VectorNative<AlternativeScopeVar> scopeHierarchyVars;
    semContext.sourceFile = debugCxtIp->node->sourceFile;
    semContext.node       = debugCxtIp->node;
    semContext.job        = &semJob;

    if (SemanticJob::collectScopeHierarchy(&semContext, scopeHierarchy, scopeHierarchyVars, debugCxtIp->node, COLLECT_ALL))
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
    static Vector<Utf8> debugCmdHistory;
    static uint32_t     debugCmdHistoryIndex = 0;

    Utf8     line;
    uint32_t cursorX = 0;

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
                fputs(Fmt("\x1B[%dD", cursorX), stdout); // Move the cursor at 0
            cursorX = 0;
            continue;

        //////////////////////////////////
        case OS::Key::End:
            if (cursorX != line.count)
                fputs(Fmt("\x1B[%dC", line.count - cursorX), stdout); // Move the cursor to the end of line
            cursorX = line.count;
            continue;

        //////////////////////////////////
        case OS::Key::Delete:
            if (cursorX == line.count)
                continue;
            fputs("\x1B[1P", stdout); // Delete the character
            line.remove(cursorX, 1);
            continue;

        //////////////////////////////////
        case OS::Key::Back:
            if (!cursorX)
                continue;
            MOVE_LEFT();
            fputs("\x1B[1P", stdout); // Delete the character
            line.remove(cursorX, 1);
            continue;

        //////////////////////////////////
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

        //////////////////////////////////
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

        //////////////////////////////////
        case OS::Key::Escape:
            if (cursorX) // Move the cursor at 0
                fputs(Fmt("\x1B[%dD", cursorX), stdout);
            fputs(Fmt("\x1B[%dX", line.count), stdout); // Erase the current command
            line.clear();
            cursorX = 0;
            break;

        //////////////////////////////////
        case OS::Key::PasteFromClipboard:
        {
            Utf8 str = OS::getClipboardString();
            fputs(Fmt("\x1B[%d@", str.length()), stdout); // Insert n blanks and shift right
            for (auto cc : str)
            {
                fputc(cc, stdout);
                line.insert(cursorX, (char) cc);
                cursorX++;
            }

            break;
        }

        //////////////////////////////////
        case OS::Key::Ascii:
            fputs("\x1B[1@", stdout); // Insert n blanks and shift right
            fputc(c, stdout);
            line.insert(cursorX, (char) c);
            cursorX++;
            break;

        //////////////////////////////////
        case OS::Key::Tab:
        {
            if (!debugCxtIp || !debugCxtIp->node || !debugCxtIp->node->sourceFile)
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
        debugCmdHistoryIndex = (uint32_t) debugCmdHistory.size();
    }

    g_Log.eol();
    return line;
}

bool ByteCodeDebugger::mustBreak(ByteCodeRunContext* context)
{
    auto ip           = context->ip;
    bool zapCurrentIp = false;

    switch (debugStepMode)
    {
    case DebugStepMode::ToNextBreakpoint:
        zapCurrentIp = true;
        break;

    case DebugStepMode::NextInstructionStepIn:
        context->debugOn = true;
        debugStepMode    = DebugStepMode::None;
        debugBcMode      = true;
        break;

    case DebugStepMode::NextInstructionStepOut:
        // If inside a sub function
        if (context->curRC > debugStepRC)
        {
            zapCurrentIp = true;
            break;
        }

        context->debugOn = true;
        debugStepMode    = DebugStepMode::None;
        debugBcMode      = true;
        break;

    case DebugStepMode::NextLineStepIn:
    {
        debugBcMode = false;
        auto loc    = ByteCode::getLocation(context->bc, ip, debugLastBreakIp->node->ownerInline ? true : false);
        SWAG_ASSERT(loc.file && loc.location);
        if (debugStepLastFile == loc.file && debugStepLastLocation && debugStepLastLocation->line == loc.location->line)
        {
            zapCurrentIp = true;
        }
        else
        {
            context->debugOn = true;
            debugStepMode    = DebugStepMode::None;
        }
        break;
    }
    case DebugStepMode::NextLineStepOut:
    {
        debugBcMode = false;

        // If inside a sub function
        if (context->curRC > debugStepRC)
        {
            zapCurrentIp = true;
            break;
        }

        if (!ip->node)
        {
            zapCurrentIp = true;
            break;
        }

        // If last break was in a real function, and now we are in an inline block
        if (!debugLastBreakIp->node->ownerInline && ip->node->ownerInline)
        {
            zapCurrentIp = true;
            break;
        }

        // If last break was in an inline block, and we are in a sub inline block
        if (debugLastBreakIp->node->ownerInline && debugLastBreakIp->node->ownerInline->isParentOf(ip->node->ownerInline))
        {
            zapCurrentIp = true;
            break;
        }

        auto loc = ByteCode::getLocation(context->bc, ip, debugLastBreakIp->node->ownerInline ? true : false);
        SWAG_ASSERT(loc.file && loc.location);
        if (debugStepLastFile == loc.file && debugStepLastLocation && debugStepLastLocation->line == loc.location->line)
        {
            zapCurrentIp = true;
        }
        else
        {
            context->debugOn = true;
            debugStepMode    = DebugStepMode::None;
        }
        break;
    }
    case DebugStepMode::FinishedFunction:
    {
        // Top level function, break on ret
        if (context->curRC == 0 && debugStepRC == -1 && ByteCode::isRet(ip))
        {
            debugStepMode = DebugStepMode::None;
            break;
        }

        // We are in a sub function
        if (context->curRC > debugStepRC)
        {
            zapCurrentIp = true;
            break;
        }

        // If last break was in an inline block, and we are in a sub inline block
        if (debugLastBreakIp->node->ownerInline && debugLastBreakIp->node->ownerInline->isParentOf(ip->node->ownerInline))
        {
            zapCurrentIp = true;
            break;
        }

        // We are in the same inline block
        if (ip->node->ownerInline && debugLastBreakIp->node->ownerInline == ip->node->ownerInline)
        {
            zapCurrentIp = true;
            break;
        }

        context->debugOn = true;
        debugStepMode    = DebugStepMode::None;
        break;
    }
    default:
        break;
    }

    return !zapCurrentIp;
}

bool ByteCodeDebugger::processCommandLine(ByteCodeRunContext* context, Vector<Utf8>& cmds, Utf8& line, Utf8& cmdExpr)
{
    line.clear();
    bool err = false;
    for (size_t i = 0; i < cmds.size() && !err; i++)
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
        else if (cmds[i].length() > 2 && cmds[i][0] == '$' && cmds[i][1] == 'r' && Utf8::isNumber(cmds[i] + 2))
        {
            int regN;
            if (!getRegIdx(context, cmds[i] + 1, regN))
            {
                err = true;
                continue;
            }
            auto& regP = context->getRegBuffer(debugCxtRc)[regN];
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
        return false;

    line.trim();
    cmdExpr.trim();
    return true;
}

bool ByteCodeDebugger::step(ByteCodeRunContext* context)
{
    static mutex dbgMutex;
    ScopedLock   sc(dbgMutex);

    auto ip = context->ip;

    if (context->debugEntry)
    {
        static bool firstOne = true;

        g_Log.setColor(LogColor::Gray);
        setup();

        if (firstOne)
        {
            firstOne = false;
            g_Log.setColor(LogColor::Gray);

            g_Log.eol();
            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine2);
            g_Log.eol();

            g_Log.print("entering bytecode debugger, type '?' for help\n");

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine);
            g_Log.eol();

            g_Log.print(Fmt("build configuration            = '%s'\n", g_CommandLine.buildCfg.c_str()));

            Module* module = nullptr;
            if (context->bc->sourceFile)
                module = context->bc->sourceFile->module;
            if (!module && context->bc->node && context->bc->node->sourceFile)
                module = context->bc->node->sourceFile->module;
            if (module)
            {
                g_Log.print(Fmt("BuildCfg.byteCodeDebugInline   = %s\n", module->buildCfg.byteCodeDebugInline ? "true" : "false"));
                g_Log.print(Fmt("BuildCfg.byteCodeInline        = %s\n", module->buildCfg.byteCodeInline ? "true" : "false"));
                g_Log.print(Fmt("BuildCfg.byteCodeOptimizeLevel = %d\n", module->buildCfg.byteCodeOptimizeLevel));
            }

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(LogSymbol::HorizontalLine2);
            g_Log.eol();
            g_Log.setColor(LogColor::Gray);
        }

        printMsgBkp("@breakpoint() hit");

        context->debugEntry            = false;
        context->debugStackFrameOffset = 0;
        debugStepMode                  = DebugStepMode::None;
        debugStepLastLocation          = nullptr;
        debugStepLastFile              = nullptr;
        debugForcePrintContext         = true;
    }

    // Check breakpoints/step mode
    checkBreakpoints(context);
    if (!mustBreak(context))
    {
        debugLastCurRC = context->curRC;
        debugLastIp    = ip;
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
        g_Log.print("(bcdbg) > ");

        // Get command from user
        bool ctrl  = false;
        bool shift = false;
        auto line  = getCommandLine(context, ctrl, shift);

        if (line.empty())
        {
            line = debugLastLine;
            if (line.empty())
                continue;
        }

        debugLastLine    = line;
        debugLastBreakIp = ip;

        // Split in command + parameters
        Utf8         cmd;
        Utf8         cmdExpr;
        Vector<Utf8> cmds;
        if (!line.empty())
        {
            Utf8::tokenize(line, ' ', cmds);
            for (auto& c : cmds)
                c.trim();
            cmd = cmds[0];
        }

        // Replace some stuff
        if (!processCommandLine(context, cmds, line, cmdExpr))
            continue;

        // Command
        /////////////////////////////////////////
        BcDbgCommandResult result = BcDbgCommandResult::Invalid;

        if (!cmd.empty())
        {
            for (auto& c : commands)
            {
                if (c.cb == nullptr)
                    continue;

                if (cmd == c.name || cmd == c.shortname)
                {
                    result = c.cb(context, cmds, cmdExpr);
                    break;
                }
            }
        }

        if (result == BcDbgCommandResult::Break)
            break;
        else if (result == BcDbgCommandResult::Return)
            return false;
        else if (result == BcDbgCommandResult::BadArguments)
            g_ByteCodeDebugger.printCmdError(Fmt("bad '%s' arguments", cmd.c_str()));
        else if (result == BcDbgCommandResult::Invalid)
            g_ByteCodeDebugger.printCmdError(Fmt("unknown debugger command '%s'", cmd.c_str()));
        continue;
    }

    debugLastCurRC = context->curRC;
    debugLastIp    = ip;
    return true;
}
