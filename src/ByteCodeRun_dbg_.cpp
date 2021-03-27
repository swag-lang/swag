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
    g_Log.unlock();
    g_Log.messageHeaderDot("bytecode name", context->bc->name, LogColor::Gray, LogColor::Gray, " ");
    if (context->bc->sourceFile && context->bc->node)
        g_Log.messageHeaderDot("bytecode location", format("%s:%u:%u", context->bc->sourceFile->path.c_str(), context->bc->node->token.startLocation.line, context->bc->node->token.startLocation.column), LogColor::Gray, LogColor::Gray, " ");
    else if (context->bc->sourceFile)
        g_Log.messageHeaderDot("bytecode source file", context->bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");
    if (context->ip->node && context->ip->node->sourceFile)
        g_Log.messageHeaderDot("instruction location", format("%s:%u:%u", context->ip->node->sourceFile->path.c_str(), context->ip->node->token.startLocation.line, context->ip->node->token.startLocation.column), LogColor::Gray, LogColor::Gray, " ");
    g_Log.lock();
}

static void printInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    context->bc->printLocation(ip);
    context->bc->printInstruction(ip);

    // Print input registers of next instruction
    auto flags = g_ByteCodeOpFlags[(int) ip->op];
    if (flags & OPFLAG_READ_A && !(ip->flags & BCI_IMM_A))
        printRegister(context, context->curRC, ip->a.u32, true);
    if (flags & OPFLAG_READ_B && !(ip->flags & BCI_IMM_B))
        printRegister(context, context->curRC, ip->b.u32, true);
    if (flags & OPFLAG_READ_C && !(ip->flags & BCI_IMM_C))
        printRegister(context, context->curRC, ip->c.u32, true);
    if (flags & OPFLAG_READ_D && !(ip->flags & BCI_IMM_D))
        printRegister(context, context->curRC, ip->d.u32, true);
}

void ByteCodeRun::debugger(ByteCodeRunContext* context)
{
    auto ip     = context->ip;
    auto prevIp = context->debugPrevIp;

    g_Log.lock();

    if (context->debugEntry)
    {
        g_Log.setColor(LogColor::Gray);
        g_Log.print("##########################\n");
        g_Log.print("entering bytecode debugger\n");
        g_Log.print("##########################\n");
        context->debugEntry        = false;
        context->debugReachNL      = false;
        context->debugLastLocation = nullptr;
        context->debugLastFile     = nullptr;
    }

    // Step to the next line
    bool zapCurrentIp = false;
    if (context->debugReachNL)
    {
        SourceFile*     file;
        SourceLocation* location;
        ByteCode::getLocation(context->bc, ip, &file, &location);
        if (!location || (context->debugLastFile == file && context->debugLastLocation && context->debugLastLocation->line == location->line))
            zapCurrentIp = true;
        else
            context->debugReachNL = false;
    }

    if (!zapCurrentIp)
    {
        // Print output registers of previous instruction
        if (prevIp)
        {
            auto flags = g_ByteCodeOpFlags[(int) prevIp->op];
            if (flags & OPFLAG_WRITE_A && !(prevIp->flags & BCI_IMM_A))
                printRegister(context, context->debugPrevCurRC, prevIp->a.u32, false);
            if (flags & OPFLAG_WRITE_B && !(prevIp->flags & BCI_IMM_B))
                printRegister(context, context->debugPrevCurRC, prevIp->b.u32, false);
            if (flags & OPFLAG_WRITE_C && !(prevIp->flags & BCI_IMM_C))
                printRegister(context, context->debugPrevCurRC, prevIp->c.u32, false);
            if (flags & OPFLAG_WRITE_D && !(prevIp->flags & BCI_IMM_D))
                printRegister(context, context->debugPrevCurRC, prevIp->d.u32, false);
        }

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
                g_Log.print("n or <return>      step current instruction\n");
                g_Log.print("s                  step to the next line\n");
                g_Log.print("c                  step until another @dbgbreak is reached\n");
                g_Log.print("ins                print the current instruction\n");
                g_Log.print("context            print contextual informations\n");
                g_Log.print("rall               print all registers\n");
                g_Log.print("r<num>             print register <num>\n");
                g_Log.print("stack              print bytecode callstack\n");
                g_Log.print("printbc            print current bytecode\n");
                continue;
            }

            // Print current instruction
            if (cmd == "context")
            {
                printInstruction(context, context->ip);
                continue;
            }

            // Print context
            if (cmd == "context")
            {
                printContext(context);
                continue;
            }

            // Print all registers
            if (cmd == "rall")
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
            if (cmd == "stack")
            {
                g_byteCodeStack.log();
                continue;
            }

            // Bytecode dump
            if (cmd == "printbc")
            {
                g_Log.unlock();
                context->bc->print();
                g_Log.lock();
                continue;
            }

            // Next instruction
            if (cmd.empty() || cmd == "n")
                break;

            // Step
            if (cmd == "s")
            {
                context->debugReachNL = true;
                ByteCode::getLocation(context->bc, ip, &context->debugLastFile, &context->debugLastLocation);
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

    context->debugPrevCurRC = context->curRC;
    context->debugPrevIp    = ip;
}
