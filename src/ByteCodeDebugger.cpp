#include "pch.h"
#include "ByteCode.h"
#include "Module.h"
#include "SemanticJob.h"
#include "ByteCodeDebugger.h"

Vector<BcDbgCommand> ByteCodeDebugger::commands;

void ByteCodeDebugger::setup()
{
    if (commands.size())
        return;

    // clang-format off
    commands.push_back({"<return>",    "",  "",                       "repeat the last command", nullptr});
    commands.push_back({"<tab>",       "",  "",                       "contextual completion of the current word", nullptr});
    commands.push_back({});

    commands.push_back({"step",        "s",    "",                    "runs to the next line", cmdStep});
    commands.push_back({"next",        "n",    "",                    "like 'step', but does not enter functions or inlined code", cmdNext});
    commands.push_back({"finish",      "f",    "",                    "runs until the current function is done", cmdFinish});
    commands.push_back({"continue",    "c",    "",                    "runs until another breakpoint is reached", cmdContinue});
    commands.push_back({"until",       "u",     "",                   "runs to the given line or instruction in the current function (depends on 'mode bc')", cmdUntil});
    commands.push_back({"jump",        "j",    "",                    "jump to the given line or instruction in the current function (depends on 'mode bc')", cmdJump});
    commands.push_back({});                    
                                               
    commands.push_back({"execute",     "e",    "<stmt>",              "execute the code statement <stmt> in the current context", cmdExecute});
    commands.push_back({"print",       "p",    "[/format] <expr>",    "print the result of the expression <expr> in the current context (format is the same as 'x' command)", cmdPrint});
    commands.push_back({});                    
                                               
    commands.push_back({"x",           "",     "[/format] [/num] <address>",     "print memory (format = s8|s16|s32|s64|u8|u16|u32|u64|x8|x16|x32|x64|f32|f64)", cmdMemory});
    commands.push_back({});                    
                                               
    commands.push_back({"list",        "l",    "[num]",               "print the current source code line and [num] lines around", cmdList});
    commands.push_back({"ll",          "",     "[name]",              "print the current function (or function [name]) source code", cmdLongList});
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
                                                                      
    commands.push_back({"mode",        "m",    "bc",                  "swap between bytecode mode and source code mode", cmdMode});
    commands.push_back({"mode",        "m",    "inline",              "swap between inline mode and non inline mode", cmdMode});
    commands.push_back({"mode",        "m",    "bkp",                 "enable/disable call to @breakpoint()", cmdMode});
    commands.push_back({});

    commands.push_back({"i",           "",     "[num]",               "print the current bytecode instruction and [num] instructions around", cmdInstruction});
    commands.push_back({"ii",          "",     "",                    "print the current function bytecode", cmdInstructionDump});
    commands.push_back({});                                           
                                                                      
    commands.push_back({"help",        "?",    "",                    "print this list of commands", cmdHelp});
    commands.push_back({"quit",        "q",    "",                    "quit the compiler", cmdQuit});
    // clang-format on
}

bool ByteCodeDebugger::getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN)
{
    regN = atoi(arg.c_str() + 1);
    if (!context->getRegCount(context->debugCxtRc))
    {
        g_Log.print("no available register", LogColor::Red);
        return false;
    }

    if (regN >= context->getRegCount(context->debugCxtRc))
    {
        g_Log.print(Fmt("invalid register number, maximum value is '%u'\n", (uint32_t) context->getRegCount(context->debugCxtRc) - 1), LogColor::Red);
        return false;
    }

    return true;
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
    g_ByteCodeStackTrace->getSteps(steps, context);
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
    Vector<Utf8> tokens;
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
    static Vector<Utf8> debugCmdHistory;
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

    switch (context->debugStepMode)
    {
    case ByteCodeRunContext::DebugStepMode::ToNextBreakpoint:
        zapCurrentIp = true;
        break;

    case ByteCodeRunContext::DebugStepMode::NextLine:
    {
        if (context->debugBcMode)
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
            break;
        }

        auto loc = ByteCode::getLocation(context->bc, ip, context->debugLastBreakIp->node->ownerInline ? true : false);
        SWAG_ASSERT(loc.file && loc.location);
        if (context->debugStepLastFile == loc.file && context->debugStepLastLocation && context->debugStepLastLocation->line == loc.location->line)
        {
            zapCurrentIp = true;
        }
        else
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        }
        break;
    }
    case ByteCodeRunContext::DebugStepMode::NextLineStepOut:
    {
        // If inside a sub function
        if (context->curRC > context->debugStepRC)
        {
            zapCurrentIp = true;
            break;
        }

        if (context->debugBcMode)
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
            break;
        }

        // If last break was in a real function, and now we are in an inline block
        if (!context->debugLastBreakIp->node->ownerInline && ip->node->ownerInline)
        {
            zapCurrentIp = true;
            break;
        }

        // If last break was in an inline block, and we are in a sub inline block
        if (context->debugLastBreakIp->node->ownerInline && context->debugLastBreakIp->node->ownerInline->isParentOf(ip->node->ownerInline))
        {
            zapCurrentIp = true;
            break;
        }

        auto loc = ByteCode::getLocation(context->bc, ip, context->debugLastBreakIp->node->ownerInline ? true : false);
        SWAG_ASSERT(loc.file && loc.location);
        if (context->debugStepLastFile == loc.file && context->debugStepLastLocation && context->debugStepLastLocation->line == loc.location->line)
        {
            zapCurrentIp = true;
        }
        else
        {
            context->debugOn       = true;
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        }
        break;
    }
    case ByteCodeRunContext::DebugStepMode::FinishedFunction:
    {
        // Top level function, break on ret
        if (context->curRC == 0 && context->debugStepRC == -1 && ByteCode::isRet(ip))
        {
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
            break;
        }

        // We are in a sub function
        if (context->curRC > context->debugStepRC)
        {
            zapCurrentIp = true;
            break;
        }

        // If last break was in an inline block, and we are in a sub inline block
        if (context->debugLastBreakIp->node->ownerInline && context->debugLastBreakIp->node->ownerInline->isParentOf(ip->node->ownerInline))
        {
            zapCurrentIp = true;
            break;
        }

        // We are in the same inline block
        if (ip->node->ownerInline && context->debugLastBreakIp->node->ownerInline == ip->node->ownerInline)
        {
            zapCurrentIp = true;
            break;
        }

        context->debugOn       = true;
        context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
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
        else if (cmds[i].length() > 2 && cmds[i][0] == '$' && cmds[i][1] == 'r' && Utf8::isNumber(cmds[i] + 2))
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
        return false;

    line.trim();
    cmdExpr.trim();
    return true;
}

bool ByteCodeDebugger::step(ByteCodeRunContext* context)
{
    static mutex dbgMutex;
    ScopedLock   sc(dbgMutex);

    auto ip     = context->ip;
    auto module = context->bc->sourceFile->module;

    if (context->debugEntry)
    {
        static bool firstOne = true;

        g_Log.setColor(LogColor::Gray);
        setup();

        if (firstOne)
        {
            firstOne = false;
            g_Log.setColor(LogColor::Magenta);

            g_Log.eol();
            for (int i = 0; i < LINE_W; i++)
                g_Log.print(Utf8("\xe2\x95\x90"));
            g_Log.eol();

            g_Log.print("entering bytecode debugger, type '?' for help\n");

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(Utf8("\xe2\x94\x80"));
            g_Log.eol();

            g_Log.print(Fmt("build configuration            = '%s'\n", g_CommandLine.buildCfg.c_str()));
            g_Log.print(Fmt("BuildCfg.byteCodeDebugInline   = %s\n", module->buildCfg.byteCodeDebugInline ? "true" : "false"));
            g_Log.print(Fmt("BuildCfg.byteCodeInline        = %s\n", module->buildCfg.byteCodeInline ? "true" : "false"));
            g_Log.print(Fmt("BuildCfg.byteCodeOptimizeLevel = %d\n", module->buildCfg.byteCodeOptimizeLevel));

            for (int i = 0; i < LINE_W; i++)
                g_Log.print(Utf8("\xe2\x95\x90"));
            g_Log.eol();

            g_Log.eol();

            // Bar
            /////////////////////////////////////////
            g_Log.setColor(LogColor::Green);
            for (int i = 0; i < LINE_W; i++)
                g_Log.print(Utf8("\xe2\x94\x80"));
            g_Log.eol();
            g_Log.setColor(LogColor::Gray);
        }

        g_Log.print("#### @breakpoint() hit ####\n", LogColor::Magenta);

        context->debugEntry             = false;
        context->debugStepMode          = ByteCodeRunContext::DebugStepMode::None;
        context->debugStepLastLocation  = nullptr;
        context->debugStepLastFile      = nullptr;
        context->debugStackFrameOffset  = 0;
        context->debugForcePrintContext = true;
    }

    // Check breakpoints/step mode
    checkBreakpoints(context);
    if (!mustBreak(context))
    {
        context->debugLastCurRC = context->curRC;
        context->debugLastIp    = ip;
        return true;
    }

    computeDebugContext(context);
    printDebugContext(context);

    while (true)
    {
        // Prompt
        /////////////////////////////////////////

        g_Log.setColor(LogColor::Green);
        g_Log.print("> ");

        // Get command from user
        bool ctrl  = false;
        bool shift = false;
        auto line  = getCommandLine(context, ctrl, shift);

        if (line.empty())
        {
            line = context->debugLastLine;
            if (line.empty())
                continue;
        }

        context->debugLastLine    = line;
        context->debugLastBreakIp = ip;

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

        // Bar
        /////////////////////////////////////////
        g_Log.setColor(LogColor::Green);
        for (int i = 0; i < LINE_W; i++)
            g_Log.print(Utf8("\xe2\x94\x80"));
        g_Log.eol();
        g_Log.setColor(LogColor::Gray);

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
            g_Log.print(Fmt("bad '%s' arguments\n", cmd.c_str()), LogColor::Red);
        else if (result == BcDbgCommandResult::Invalid)
            g_Log.print(Fmt("unknown debugger command '%s'\n", cmd.c_str()), LogColor::Red);
        continue;
    }

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
