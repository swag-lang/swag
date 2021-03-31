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
    if (bc->sourceFile && bc->node)
        g_Log.messageHeaderDot("bytecode location", format("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");
    else if (bc->sourceFile)
        g_Log.messageHeaderDot("bytecode source file", bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");

    if (ipNode && ipNode->sourceFile)
        g_Log.messageHeaderDot("instruction location", format("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");

    if (ipNode && ipNode->ownerFct)
    {
        ipNode->ownerFct->typeInfo->computeScopedName();
        g_Log.messageHeaderDot("function", ipNode->ownerFct->typeInfo->scopedName, LogColor::Gray, LogColor::Gray, " ");
    }

    g_Log.eol();
    g_Log.lock();
}

static void printFullRegister(Register& regP)
{
    // clang-format off
    g_Log.printColor("s8 ", LogColor::Cyan); g_Log.printColor(format("%lld ", regP.s8));
    g_Log.printColor("s16 ", LogColor::Cyan); g_Log.printColor(format("%lld ", regP.s16));
    g_Log.printColor("s32 ", LogColor::Cyan); g_Log.printColor(format("%lld ", regP.s32));
    g_Log.printColor("s64 ", LogColor::Cyan); g_Log.printColor(format("%lld ", regP.s64));
    g_Log.eol();

    g_Log.printColor("u8 ", LogColor::Cyan); g_Log.printColor(format("%llu ", regP.u8));
    g_Log.printColor("u16 ", LogColor::Cyan); g_Log.printColor(format("%llu ", regP.u16));
    g_Log.printColor("u32 ", LogColor::Cyan); g_Log.printColor(format("%llu ", regP.u32));
    g_Log.printColor("u64 ", LogColor::Cyan); g_Log.printColor(format("%llu ", regP.u64));
    g_Log.eol();

    g_Log.printColor("x8 ", LogColor::Cyan); g_Log.printColor(format("%02llx ", regP.u8));
    g_Log.printColor("x16 ", LogColor::Cyan); g_Log.printColor(format("%04llx ", regP.u16));
    g_Log.printColor("x32 ", LogColor::Cyan); g_Log.printColor(format("%08llx ", regP.u32));
    g_Log.printColor("x64 ", LogColor::Cyan); g_Log.printColor(format("%016llx ", regP.u64));
    g_Log.eol();
    
    g_Log.printColor("f32 ", LogColor::Cyan); g_Log.printColor(format("%lf", regP.f32));
    g_Log.eol();
    g_Log.printColor("f64 ", LogColor::Cyan); g_Log.printColor(format("%lf", regP.f64));
    g_Log.eol();
    // clang-format on
}

static void printInstruction(ByteCodeRunContext* context, ByteCode* bc, ByteCodeInstruction* ip, int num = 1)
{
    int cpt = 1;
    while (--num)
    {
        if (ip == bc->out)
            break;
        cpt++;
        ip--;
    }

    uint32_t    lastLine = UINT32_MAX;
    SourceFile* lastFile = nullptr;
    for (int i = 0; i < cpt; i++)
    {
        bc->printSourceCode(ip, &lastLine, &lastFile);
        bc->printInstruction(ip, context->debugCxtIp);
        ip++;
    }
}

static void computeCxt(ByteCodeRunContext* context)
{
    context->debugCxtBc = context->bc;
    context->debugCxtIp = context->ip;
    context->debugCxtRc = context->curRC;
    if (context->debugStackContext == 0)
        return;

    auto ns = context->debugStackContext - 1;
    for (int i = (int) g_byteCodeStack.steps.size() - 1; i >= 0; i--)
    {
        auto& step = g_byteCodeStack.steps[i];
        if (ns == 0)
        {
            context->debugCxtBc = step.bc;
            context->debugCxtIp = step.ip;
            break;
        }

        if (!step.ip)
            continue;
        ns--;
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
        context->debugStackContext     = 0;
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
        g_Log.eol();
        computeCxt(context);
        printInstruction(context, context->bc, ip);

        while (true)
        {
            g_Log.setColor(LogColor::Cyan);
            g_Log.print("> ");

            // Get command from user
            char az[1024];
            fgets(az, 1024, stdin);
            Utf8 cmd = az;
            if (cmd.length() > 0 && cmd.back() == '\n')
                cmd.count--;
            cmd.trim();

            // Help
            if (cmd == "?")
            {
                g_Log.setColor(LogColor::Gray);
                g_Log.eol();
                g_Log.print("<return>           runs the current instruction\n");
                g_Log.print("s                  runs to the next line\n");
                g_Log.print("n                  like s, but does not step into functions\n");
                g_Log.print("c                  runs until a debug break is reached\n");
                g_Log.print("f                  runs until the current function is done\n");
                g_Log.print("i                  print the current instruction\n");
                g_Log.print("i<num>             print the current instruction and <num> instructions before\n");
                g_Log.print("x<num>             set stack context <num>\n");
                g_Log.print("cxt, context       print contextual informations\n");
                g_Log.print("r                  print all registers\n");
                g_Log.print("r<num>             print register <num>\n");
                g_Log.print("stk, stack         print callstack\n");
                g_Log.print("bc, printbc        print current function bytecode\n");
                g_Log.print("?                  print this list of commands\n");
                g_Log.print("q, quit            quit the compiler\n");
                g_Log.eol();
                continue;
            }

            if (cmd[0] == 'x' && cmd.length() > 1 && isdigit(cmd[1]))
            {
                int regN                   = atoi(cmd.c_str() + 1);
                context->debugStackContext = regN;
                computeCxt(context);
                printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                continue;
            }

            // Print current instruction
            if (cmd == "q" || cmd == "quit")
            {
                g_Log.unlock();
                return false;
            }

            // Print current instruction
            if (cmd == "i")
            {
                printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                continue;
            }

            if (cmd[0] == 'i' && cmd.length() > 1 && isdigit(cmd[1]))
            {
                int regN = atoi(cmd.c_str() + 1);
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
            if (cmd == "r")
            {
                g_Log.setColor(LogColor::White);
                for (int i = 0; i < context->registersRC[context->debugCxtRc]->size(); i++)
                    printRegister(context, context->debugCxtRc, i, true);
                continue;
            }

            // Print one register
            if (cmd[0] == 'r' && cmd.length() > 1 && isdigit(cmd[1]))
            {
                g_Log.setColor(LogColor::Gray);
                int regN = atoi(cmd.c_str() + 1);
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
                g_byteCodeStack.log();
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
                context->debugStackContext = 0;
                context->debugStepMode     = ByteCodeRunContext::DebugStepMode::NextLine;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                break;
            }

            // Step to next line, step out
            if (cmd == "n")
            {
                context->debugStackContext = 0;
                context->debugStepMode     = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                context->debugStepRC = context->curRC;
                break;
            }

            // Exit current function
            if (cmd == "f")
            {
                context->debugStackContext = 0;
                context->debugStepMode     = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC       = context->curRC - 1;
                break;
            }

            // Debug off
            if (cmd == "c")
            {
                context->debugStackContext = 0;
                context->debugOn           = false;
                break;
            }

            g_Log.setColor(LogColor::Red);
            g_Log.print("undefined command, type '?' for help\n");
        }
    }

    g_Log.setDefaultColor();
    g_Log.unlock();

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
