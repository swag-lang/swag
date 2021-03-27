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
    auto ipNode = context->ip->node;

    g_Log.unlock();
    g_Log.messageHeaderDot("bytecode name", context->bc->name, LogColor::Gray, LogColor::Gray, " ");
    if (context->bc->sourceFile && context->bc->node)
        g_Log.messageHeaderDot("bytecode location", format("%s:%u:%u", context->bc->sourceFile->path.c_str(), context->bc->node->token.startLocation.line + 1, context->bc->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");
    else if (context->bc->sourceFile)
        g_Log.messageHeaderDot("bytecode source file", context->bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");

    if (ipNode && ipNode->sourceFile)
        g_Log.messageHeaderDot("instruction location", format("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, context->ip->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");

    if (ipNode && ipNode->ownerFct)
    {
        ipNode->ownerFct->typeInfo->computeScopedName();
        g_Log.messageHeaderDot("function", ipNode->ownerFct->typeInfo->scopedName, LogColor::Gray, LogColor::Gray, " ");
    }

    g_Log.lock();
}

static void printInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    context->bc->printSourceCode(ip);
    context->bc->printInstruction(ip);

    // Print input registers of next instruction
    /*auto flags = g_ByteCodeOpFlags[(int) ip->op];
    if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
        printRegister(context, context->curRC, ip->a.u32, true);
    if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
        printRegister(context, context->curRC, ip->b.u32, true);
    if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
        printRegister(context, context->curRC, ip->c.u32, true);
    if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
        printRegister(context, context->curRC, ip->d.u32, true);*/
}

void ByteCodeRun::debugger(ByteCodeRunContext* context)
{
    auto ip = context->ip;

    g_Log.lock();

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
        // Print output registers of previous instruction
        /*auto prevIp = context->debugLastIp;
        if (prevIp)
        {
            auto flags = g_ByteCodeOpFlags[(int) prevIp->op];
            if (flags & OPFLAG_WRITE_A && !(prevIp->flags & BCI_IMM_A))
                printRegister(context, context->debugLastCurRC, prevIp->a.u32, false);
            if (flags & OPFLAG_WRITE_B && !(prevIp->flags & BCI_IMM_B))
                printRegister(context, context->debugLastCurRC, prevIp->b.u32, false);
            if (flags & OPFLAG_WRITE_C && !(prevIp->flags & BCI_IMM_C))
                printRegister(context, context->debugLastCurRC, prevIp->c.u32, false);
            if (flags & OPFLAG_WRITE_D && !(prevIp->flags & BCI_IMM_D))
                printRegister(context, context->debugLastCurRC, prevIp->d.u32, false);
        }*/

        g_Log.eol();
        printInstruction(context, ip);

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
                g_Log.print("<return>           runs the current instruction\n");
                g_Log.print("s                  runs to the next line\n");
                g_Log.print("n                  like s, but does not step into functions\n");
                g_Log.print("c                  runs until a debug break is reached\n");
                g_Log.print("f                  runs until the current function is done\n");
                g_Log.print("ins                print the current instruction\n");
                g_Log.print("cxt, context       print contextual informations\n");
                g_Log.print("r                  print all registers\n");
                g_Log.print("r<num>             print register <num>\n");
                g_Log.print("stk, stack         print callstack\n");
                g_Log.print("bc, printbc        print current function bytecode\n");
                g_Log.print("?                  print this list of commands\n");
                g_Log.print("quit               quit the compiler\n");
                continue;
            }

            // Print current instruction
            if (cmd == "quit")
            {
                exit(0);
            }

            // Print current instruction
            if (cmd == "ins")
            {
                printInstruction(context, context->ip);
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
                for (int i = 0; i < context->registersRC[context->curRC]->size(); i++)
                    printRegister(context, context->curRC, i, true);
                continue;
            }

            // Print one register
            if (cmd[0] == 'r' && cmd.length() > 1 && isdigit(cmd[1]))
            {
                g_Log.setColor(LogColor::Gray);
                int regN = atoi(cmd.c_str() + 1);
                if (regN >= context->registersRC[context->curRC]->size())
                    g_Log.print(format("invalid register number, maximum value is '%u'\n", (uint32_t) context->registersRC[context->curRC]->size()));
                else
                {
                    auto& regP = context->registersRC[context->curRC]->buffer[regN];
                    g_Log.print(format("s8  [%lld] s16 [%lld] s32 [%lld] s64 [%lld]\n", regP.s8, regP.s16, regP.s32, regP.s64));
                    g_Log.print(format("u8  [%llu] u16 [%llu] u32 [%llu] u64 [%llu]\n", regP.u8, regP.u16, regP.u32, regP.u64));
                    g_Log.print(format("x8  [%02llx] x16 [%04llx] x32 [08%llx] x64 [%016llx]\n", regP.u8, regP.u16, regP.u32, regP.u64));
                    g_Log.print(format("f32 [%lf]\n", regP.f32));
                    g_Log.print(format("f64 [%lf]\n", regP.f64));
                    g_Log.print(format("ptr [%llx]\n", regP.pointer));
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
                context->bc->print();
                g_Log.lock();
                continue;
            }

            // Next instruction
            if (cmd.empty())
                break;

            // Step to next line
            if (cmd == "s")
            {
                context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLine;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                break;
            }

            // Step to next line, step out
            if (cmd == "n")
            {
                context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                ByteCode::getLocation(context->bc, ip, &context->debugStepLastFile, &context->debugStepLastLocation);
                context->debugStepRC = context->curRC;
                break;
            }

            // Exit current function
            if (cmd == "f")
            {
                context->debugStepMode = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC   = context->curRC - 1;
                break;
            }

            // Debug off
            if (cmd == "c")
            {
                context->debugOn = false;
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
}
