#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "ByteCodeStack.h"
#include "AstNode.h"

static void printRegister(ByteCodeRunContext* context, uint32_t curRC, uint32_t reg, bool read)
{
    auto registersRC = context->registersRC[curRC]->buffer;
    Utf8 str;
    str += read ? format("(%u)", reg) : format("[%u]", reg);
    str += format(" = %016llx\n", registersRC[reg].u64);
    g_Log.setColor(LogColor::Gray);
    g_Log.print(str);
}

static void printContext(ByteCodeRunContext* context)
{
    auto ipNode = context->debugCxtIp->node;
    auto bc     = context->debugCxtBc;

    g_Log.unlock();
    g_Log.eol();

    g_Log.messageHeaderDot("bytecode name", bc->name, LogColor::Gray, LogColor::Gray, " ");

    if (bc->sourceFile)
    {
        if (bc->node)
            g_Log.messageHeaderDot("bytecode location", format("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");
        else
            g_Log.messageHeaderDot("bytecode source file", bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");
    }

    if (ipNode)
    {
        if (ipNode->sourceFile)
            g_Log.messageHeaderDot("instruction location", format("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");

        if (ipNode->ownerFct)
        {
            ipNode->ownerFct->typeInfo->computeScopedName();
            g_Log.messageHeaderDot("function", ipNode->ownerFct->typeInfo->scopedName, LogColor::Gray, LogColor::Gray, " ");
        }
    }

    g_Log.messageHeaderDot("stack level", format("%u", context->debugStackFrameOffset), LogColor::Gray, LogColor::Gray, " ");

    g_Log.eol();
    g_Log.lock();
}

static void printFullRegister(Register& regP)
{
    auto col = LogColor::Magenta;

    // clang-format off
    g_Log.printColor("s8  ", col); g_Log.printColor(format("%d ", regP.s8));
    g_Log.printColor("u8 ", col); g_Log.printColor(format("%u ", regP.u8));
    g_Log.printColor("x8 ", col); g_Log.printColor(format("%02x ", regP.u8));
    g_Log.eol();

    g_Log.printColor("s16 ", col); g_Log.printColor(format("%d ", regP.s16));
    g_Log.printColor("u16 ", col); g_Log.printColor(format("%u ", regP.u16));
    g_Log.printColor("x16 ", col); g_Log.printColor(format("%04x ", regP.u16));
    g_Log.eol();

    g_Log.printColor("s32 ", col); g_Log.printColor(format("%d ", regP.s32));
    g_Log.printColor("u32 ", col); g_Log.printColor(format("%u ", regP.u32));
    g_Log.printColor("x32 ", col); g_Log.printColor(format("%08x ", regP.u32));
    g_Log.eol();

    g_Log.printColor("s64 ", col); g_Log.printColor(format("%lld ", regP.s64));
    g_Log.printColor("u64 ", col); g_Log.printColor(format("%llu ", regP.u64));
    g_Log.printColor("x64 ", col); g_Log.printColor(format("%016llx ", regP.u64));
    g_Log.eol();
    
    g_Log.printColor("f32 ", col); g_Log.printColor(format("%lf", regP.f32));
    g_Log.eol();
    g_Log.printColor("f64 ", col); g_Log.printColor(format("%lf", regP.f64));
    g_Log.eol();
    // clang-format on
}

static void printInstruction(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1)
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
        bc->printInstruction(ip, count == 1 ? nullptr : context->debugCxtIp);
        if (ip->op == ByteCodeOp::End)
            break;
        ip++;
    }
}

static void computeCxt(ByteCodeRunContext* context)
{
    context->debugCxtBc = context->bc;
    context->debugCxtIp = context->ip;
    context->debugCxtRc = context->curRC;
    if (context->debugStackFrameOffset == 0 || g_byteCodeStack.steps.empty())
        return;

    context->debugCxtRc--;
    uint32_t maxLevel              = g_byteCodeStack.maxLevel(context);
    context->debugStackFrameOffset = min(context->debugStackFrameOffset, maxLevel);
    uint32_t ns                    = 0;
    for (int i = (int) g_byteCodeStack.steps.size() - 1; i >= 0; i--)
    {
        auto& step = g_byteCodeStack.steps[i];
        if (ns == context->debugStackFrameOffset)
        {
            context->debugCxtBc = step.bc;
            context->debugCxtIp = step.ip;
            break;
        }

        ns++;
        if (!step.ip)
            continue;
        context->debugCxtRc--;
    }
}

bool ByteCodeRun::debugger(ByteCodeRunContext* context)
{
    auto ip = context->ip;

    g_Log.lock();
    g_byteCodeStack.currentContext = context;

    if (context->debugEntry)
    {
        g_Log.setColor(LogColor::Gray);
        g_Log.eol();
        g_Log.print("#############################################\n");
        g_Log.print("entering bytecode debugger, type '?' for help\n");
        g_Log.print("#############################################\n");
        context->debugEntry            = false;
        context->debugStepMode         = ByteCodeRunContext::DebugStepMode::None;
        context->debugStepLastLocation = nullptr;
        context->debugStepLastFile     = nullptr;
        context->debugStackFrameOffset = 0;
    }

    // Step to the next line
    bool zapCurrentIp = false;
    switch (context->debugStepMode)
    {
    case ByteCodeRunContext::DebugStepMode::NextLine:
    {
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &file, &location);
        if (!location || (context->debugStepLastFile == file && context->debugStepLastLocation && context->debugStepLastLocation->line == location->line))
            zapCurrentIp = true;
        else
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
    }
    break;
    case ByteCodeRunContext::DebugStepMode::NextLineStepOut:
    {
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &file, &location);
        if (!location || (context->debugStepLastFile == file && context->debugStepLastLocation && context->debugStepLastLocation->line == location->line))
            zapCurrentIp = true;
        else if (context->curRC > context->debugStepRC)
            zapCurrentIp = true;
        else
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
    }
    break;
    case ByteCodeRunContext::DebugStepMode::FinishedFunction:
    {
        if (context->curRC > context->debugStepRC)
            zapCurrentIp = true;
        else
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
        if (context->curRC == 0 && context->debugStepRC == -1 && ip->op == ByteCodeOp::Ret)
            context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
    }
    break;
    }

    if (!zapCurrentIp)
    {
        computeCxt(context);
        printInstruction(context, context->bc, ip);

        while (true)
        {
            g_Log.setColor(LogColor::Cyan);
            g_Log.print("> ");

            // Get command from user
            char az[1024];
            fgets(az, 1024, stdin);
            Utf8 line = az;
            if (line.length() > 0 && line.back() == '\n')
                line.count--;
            line.trim();

            // Split in command + parameters
            Utf8         cmd;
            vector<Utf8> cmds;
            if (!line.empty())
            {
                tokenize(line, ' ', cmds);
                for (auto& c : cmds)
                    c.trim();
                cmd = cmds[0];
            }

            // Help
            if (cmd == "?")
            {
                g_Log.setColor(LogColor::Gray);
                g_Log.eol();
                g_Log.print("<return>           runs the current instruction\n");
                g_Log.print("s                  runs to the next line\n");
                g_Log.print("n                  like s, but does not step into functions\n");
                g_Log.print("c                  runs until another break is reached\n");
                g_Log.print("f                  runs until the current function is done\n");
                g_Log.eol();

                g_Log.print("stk, stack         print callstack\n");
                g_Log.print("up [num]           move stack frame <num> level up\n");
                g_Log.print("down [num]         move stack frame <num> level down\n");
                g_Log.print("frame <num>        move stack frame to level <num>\n");
                g_Log.eol();

                g_Log.print("i                  print the current instruction\n");
                g_Log.print("i <num>            print the current instruction and <num> instructions around\n");
                g_Log.print("cxt, context       print contextual informations\n");
                g_Log.print("r                  print all registers\n");
                g_Log.print("r <num>            print register <num>\n");
                g_Log.print("list               print current source code line\n");
                g_Log.print("bc, printbc        print current function bytecode\n");
                g_Log.eol();

                g_Log.print("?                  print this list of commands\n");
                g_Log.print("q, quit            quit the compiler\n");
                g_Log.eol();
                continue;
            }

            // Print current instruction
            if (cmd == "list" && cmds.size() == 1)
            {
                context->debugCxtBc->printSourceCode(context->debugCxtIp);
                continue;
            }

            // Stack frame up
            if (cmd == "frame" && cmds.size() == 2)
            {
                uint32_t off                   = atoi(cmds[1].c_str());
                uint32_t maxLevel              = g_byteCodeStack.maxLevel(context);
                off                            = min(off, maxLevel);
                context->debugStackFrameOffset = maxLevel - off;
                computeCxt(context);
                printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                continue;
            }

            // Stack frame up
            if (cmd == "up")
            {
                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                uint32_t maxLevel = g_byteCodeStack.maxLevel(context);
                if (context->debugStackFrameOffset == maxLevel)
                    g_Log.printColor("initial frame selected; you cannot go up.\n");
                else
                {
                    context->debugStackFrameOffset += off;
                    computeCxt(context);
                    printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                }

                continue;
            }

            // Stack frame down
            if (cmd == "down")
            {
                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                if (context->debugStackFrameOffset == 0)
                    g_Log.printColor("bottom(innermost) frame selected; you cannot go down.\n");
                else
                {
                    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
                    computeCxt(context);
                    printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                }

                continue;
            }

            // Print current instruction
            if (cmd == "q" || cmd == "quit")
            {
                g_Log.unlock();
                return false;
            }

            // Print current instruction
            if (cmd == "i" && cmds.size() == 1)
            {
                printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                continue;
            }

            if (cmd == "i" && cmds.size() == 2)
            {
                int regN = atoi(cmds[1].c_str());
                printInstruction(context, context->debugCxtBc, context->debugCxtIp, regN + 1);
                continue;
            }

            // Print context
            if (cmd == "cxt" || cmd == "context")
            {
                printContext(context);
                continue;
            }

            // Print all registers
            if (cmd == "r" && cmds.size() == 1)
            {
                g_Log.setColor(LogColor::White);
                for (int i = 0; i < context->registersRC[context->debugCxtRc]->size(); i++)
                    printRegister(context, context->debugCxtRc, i, true);
                continue;
            }

            // Print one register
            if (cmd == "r" && cmds.size() == 2)
            {
                g_Log.setColor(LogColor::Gray);
                int regN = atoi(cmds[1].c_str());
                if (regN >= context->registersRC[context->debugCxtRc]->size())
                    g_Log.print(format("invalid register number, maximum value is '%u'\n", (uint32_t) context->registersRC[context->debugCxtRc]->size()));
                else
                {
                    auto& regP = context->registersRC[context->debugCxtRc]->buffer[regN];
                    printFullRegister(regP);
                }

                continue;
            }

            // Stack
            if (cmd == "stk" || cmd == "stack")
            {
                g_byteCodeStack.currentContext = context;
                g_byteCodeStack.log();
                g_byteCodeStack.currentContext = nullptr;
                continue;
            }

            // Bytecode dump
            if (cmd == "bc" || cmd == "printbc")
            {
                g_Log.unlock();
                context->debugCxtBc->print(context->debugCxtIp);
                g_Log.lock();
                continue;
            }

            // Next instruction
            if (cmd.empty())
                break;

            // Step to next line
            if (cmd == "s")
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                break;
            }

            // Step to next line, step out
            if (cmd == "n")
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                context->debugStepRC = context->curRC;
                break;
            }

            // Exit current function
            if (cmd == "f")
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC           = context->curRC - 1;
                break;
            }

            // Debug off
            if (cmd == "c")
            {
                context->debugStackFrameOffset = 0;
                context->debugOn               = false;
                break;
            }

            g_Log.setColor(LogColor::Red);
            g_Log.print("invalid command, type '?' for help\n");
        }
    }

    g_Log.setDefaultColor();
    g_Log.unlock();

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
