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

bool ByteCodeDebugger::getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN)
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
                continue;

            line.trim();
            cmdExpr.trim();

            // Help
            /////////////////////////////////////////
            if (cmd == "?")
            {
                CHECK_CMD_RESULT(cmdHelp(context, cmds, cmdExpr));
                continue;
            }

            // Print expression
            /////////////////////////////////////////
            if (cmd == "p" || cmd == "print")
            {
                CHECK_CMD_RESULT(cmdPrint(context, cmds, cmdExpr));
                continue;
            }

            // Execute expression
            /////////////////////////////////////////
            if (cmd == "e" || cmd == "exec" || cmd == "execute")
            {
                CHECK_CMD_RESULT(cmdExecute(context, cmds, cmdExpr));
                continue;
            }

            // Info functions
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "func")
            {
                CHECK_CMD_RESULT(cmdInfoFuncs(context, cmds, cmdExpr));
                continue;
            }

            // Info modules
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "modules")
            {
                CHECK_CMD_RESULT(cmdInfoModules(context, cmds, cmdExpr));
                continue;
            }

            // Info locals
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "locals")
            {
                CHECK_CMD_RESULT(cmdInfoLocals(context, cmds, cmdExpr));
                continue;
            }

            // Info registers
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "regs")
            {
                CHECK_CMD_RESULT(cmdInfoRegs(context, cmds, cmdExpr));
                continue;
            }

            // Info args
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "args")
            {
                CHECK_CMD_RESULT(cmdInfoArgs(context, cmds, cmdExpr));
                continue;
            }

            // Print memory
            /////////////////////////////////////////
            if (cmd == "x")
            {
                CHECK_CMD_RESULT(cmdMemory(context, cmds, cmdExpr));
                continue;
            }

            // Print stack
            /////////////////////////////////////////
            if (cmd == "bt")
            {
                CHECK_CMD_RESULT(cmdBackTrace(context, cmds, cmdExpr));
                continue;
            }

            // Set stack frame
            /////////////////////////////////////////
            if (cmd == "frame")
            {
                CHECK_CMD_RESULT(cmdFrame(context, cmds, cmdExpr));
                continue;
            }

            // Stack frame up
            /////////////////////////////////////////
            if (cmd == "u" || cmd == "up")
            {
                CHECK_CMD_RESULT(cmdFrameUp(context, cmds, cmdExpr));
                continue;
            }

            // Stack frame down
            /////////////////////////////////////////
            if (cmd == "d" || cmd == "down")
            {
                CHECK_CMD_RESULT(cmdFrameDown(context, cmds, cmdExpr));
                continue;
            }

            // Exit
            /////////////////////////////////////////
            if (cmd == "q" || cmd == "quit")
            {
                CHECK_CMD_RESULT(cmdQuit(context, cmds, cmdExpr));
                continue;
            }

            // Print context
            /////////////////////////////////////////
            if (cmd == "w" || cmd == "where")
            {
                CHECK_CMD_RESULT(cmdWhere(context, cmds, cmdExpr));
                continue;
            }

            // Print current instruction
            /////////////////////////////////////////
            if (cmd == "i")
            {
                CHECK_CMD_RESULT(cmdInstruction(context, cmds, cmdExpr));
                continue;
            }

            // Bytecode dump
            /////////////////////////////////////////
            if (cmd == "ii")
            {
                CHECK_CMD_RESULT(cmdInstructionDump(context, cmds, cmdExpr));
                continue;
            }

            // Bc mode
            /////////////////////////////////////////
            if (cmd == "bcmode")
            {
                CHECK_CMD_RESULT(cmdBcMode(context, cmds, cmdExpr));
                continue;
            }

            // Function line code
            /////////////////////////////////////////
            if (cmd == "l" || cmd == "list")
            {
                CHECK_CMD_RESULT(cmdList(context, cmds, cmdExpr));
                continue;
            }

            // Function full code
            /////////////////////////////////////////
            if (cmd == "ll")
            {
                CHECK_CMD_RESULT(cmdLongList(context, cmds, cmdExpr));
                continue;
            }

            // Breakpoints
            /////////////////////////////////////////
            if (cmd == "b" || cmd == "break" || cmd == "tb" || cmd == "tbreak")
            {
                CHECK_CMD_RESULT(cmdBreakpoint(context, cmd, cmds, cmdExpr));
                continue;
            }

            // Next instruction
            /////////////////////////////////////////
            if (cmd.empty())
            {
                CHECK_CMD_RESULT(cmdEmpty(context, shift, cmds, cmdExpr));
                continue;
            }

            // Step to next line
            /////////////////////////////////////////
            if (cmd == "s" || cmd == "step")
            {
                CHECK_CMD_RESULT(cmdStep(context, cmds, cmdExpr));
                continue;
            }

            // Step to next line, step out
            /////////////////////////////////////////
            if (cmd == "n" || cmd == "next")
            {
                CHECK_CMD_RESULT(cmdNext(context, cmds, cmdExpr));
                continue;
            }

            // Run to the end of the current function
            /////////////////////////////////////////
            if (cmd == "f" || cmd == "finish")
            {
                CHECK_CMD_RESULT(cmdFinish(context, cmds, cmdExpr));
                continue;
            }

            // Continue
            /////////////////////////////////////////
            if (cmd == "c" || cmd == "cont" || cmd == "continue")
            {
                CHECK_CMD_RESULT(cmdContinue(context, cmds, cmdExpr));
                continue;
            }

            // Jump to line/instruction
            /////////////////////////////////////////
            if (cmd == "j" || cmd == "jump")
            {
                CHECK_CMD_RESULT(cmdJump(context, cmds, cmdExpr));
                continue;
            }

            // Run until a line/instruction is reached
            /////////////////////////////////////////
            if (cmd == "un" || cmd == "until")
            {
                CHECK_CMD_RESULT(cmdUntil(context, cmds, cmdExpr));
                continue;
            }

            // Default to 'print' / 'execute'
            /////////////////////////////////////////
            evalDefault(context, line);
        }
    }

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
