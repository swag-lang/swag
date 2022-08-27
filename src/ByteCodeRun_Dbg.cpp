#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "ByteCodeStack.h"
#include "AstNode.h"
#include "Ast.h"

static void printRegister(ByteCodeRunContext* context, uint32_t curRC, uint32_t reg, bool read)
{
    auto registersRC = context->getRegBuffer(curRC);
    Utf8 str;
    str += read ? Fmt("(%u)", reg) : Fmt("[%u]", reg);
    str += Fmt(" = %016llx\n", registersRC[reg].u64);
    g_Log.setColor(LogColor::Gray);
    g_Log.print(str);
}

static uint64_t getAddrValue(const void* addr, uint32_t bitCount)
{
    __try
    {
        switch (bitCount)
        {
        case 8:
        default:
            return *(uint8_t*) addr;
        case 16:
            return *(uint16_t*) addr;
        case 32:
            return *(uint32_t*) addr;
        case 64:
            return *(uint64_t*) addr;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }
}

static void printMemory(ByteCodeRunContext* context, uint32_t curRC, const char* addr, int count, uint32_t bitCount = 8)
{
    count = max(count, 1);
    count = min(count, 4096);

    uint64_t addrVal = 0;

    if (addr[0] == 'r')
    {
        addr += 1;
        auto reg         = atoi(addr);
        auto registersRC = context->getRegBuffer(curRC);
        addrVal          = registersRC[reg].u64;
    }
    else
    {
        while (*addr)
        {
            addrVal <<= 4;
            if (*addr >= 'a' && *addr <= 'f')
                addrVal += 10 + (*addr - 'a');
            else if (*addr >= 'A' && *addr <= 'F')
                addrVal += 10 + (*addr - 'A');
            else if (*addr >= '0' && *addr <= '9')
                addrVal += (*addr - '0');
            addr++;
        }
    }

    int perLine = 8;
    switch (bitCount)
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

    g_Log.setColor(LogColor::Gray);

    while (count > 0)
    {
        auto addrLine = addrB;

        for (int i = 0; i < min(count, perLine); i++)
        {
            switch (bitCount)
            {
            case 8:
            default:
                g_Log.print(Fmt("%02llx ", getAddrValue(addrB, 8)));
                addrB += 1;
                break;

            case 16:
                g_Log.print(Fmt("%04llx ", getAddrValue(addrB, 16)));
                addrB += 2;
                break;

            case 32:
                g_Log.print(Fmt("%08llx ", getAddrValue(addrB, 32)));
                break;

            case 64:
                g_Log.print(Fmt("%016llx ", getAddrValue(addrB, 64)));
                addrB += 8;
                break;
            }
        }

        addrB = addrLine;
        if (bitCount == 8)
        {
            for (int i = 0; i < perLine - min(count, perLine); i++)
                g_Log.print("   ");

            g_Log.print(" ");
            for (int i = 0; i < min(count, perLine); i++)
            {
                auto c = getAddrValue(addrB, 8);
                if (c < 32 || c > 127)
                    c = '.';
                g_Log.print(Fmt("%c", c));
                addrB += 1;
            }
        }

        g_Log.eol();

        addrB = addrLine;
        addrB += count * (bitCount / 8);
        count -= min(count, 8);
        if (!count)
            break;
    }
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
            g_Log.messageHeaderDot("bytecode location", Fmt("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");
        else
            g_Log.messageHeaderDot("bytecode source file", bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");
    }

    if (ipNode)
    {
        if (ipNode->sourceFile)
            g_Log.messageHeaderDot("instruction location", Fmt("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");

        if (ipNode->ownerFct)
        {
            ipNode->ownerFct->typeInfo->computeScopedName();
            g_Log.messageHeaderDot("function", ipNode->ownerFct->typeInfo->scopedName, LogColor::Gray, LogColor::Gray, " ");
        }
    }

    g_Log.messageHeaderDot("stack level", Fmt("%u", context->debugStackFrameOffset), LogColor::Gray, LogColor::Gray, " ");

    g_Log.eol();
    g_Log.lock();
}

static void printFullRegister(Register& regP)
{
    auto col = LogColor::Magenta;

    // clang-format off
    g_Log.printColor("s8  ", col); g_Log.printColor(Fmt("%d ", regP.s8));
    g_Log.printColor("u8 ", col); g_Log.printColor(Fmt("%u ", regP.u8));
    g_Log.printColor("x8 ", col); g_Log.printColor(Fmt("%02x ", regP.u8));
    g_Log.eol();

    g_Log.printColor("s16 ", col); g_Log.printColor(Fmt("%d ", regP.s16));
    g_Log.printColor("u16 ", col); g_Log.printColor(Fmt("%u ", regP.u16));
    g_Log.printColor("x16 ", col); g_Log.printColor(Fmt("%04x ", regP.u16));
    g_Log.eol();

    g_Log.printColor("s32 ", col); g_Log.printColor(Fmt("%d ", regP.s32));
    g_Log.printColor("u32 ", col); g_Log.printColor(Fmt("%u ", regP.u32));
    g_Log.printColor("x32 ", col); g_Log.printColor(Fmt("%08x ", regP.u32));
    g_Log.eol();

    g_Log.printColor("s64 ", col); g_Log.printColor(Fmt("%lld ", regP.s64));
    g_Log.printColor("u64 ", col); g_Log.printColor(Fmt("%llu ", regP.u64));
    g_Log.printColor("x64 ", col); g_Log.printColor(Fmt("%016llx ", regP.u64));
    g_Log.eol();
    
    g_Log.printColor("f32 ", col); g_Log.printColor(Fmt("%lf", regP.f32));
    g_Log.eol();
    g_Log.printColor("f64 ", col); g_Log.printColor(Fmt("%lf", regP.f64));
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
    context->debugCxtBp = context->bp;
    if (context->debugStackFrameOffset == 0)
        return;

    VectorNative<ByteCodeStackStep> steps;
    g_ByteCodeStackTrace.getSteps(steps);
    if (steps.empty())
        return;

    uint32_t maxLevel              = g_ByteCodeStackTrace.maxLevel(context);
    context->debugStackFrameOffset = min(context->debugStackFrameOffset, maxLevel);
    uint32_t ns                    = 0;

    for (int i = (int) maxLevel; i >= 0; i--)
    {
        if (i >= steps.size())
            continue;

        auto& step = steps[i];
        if (ns == context->debugStackFrameOffset)
        {
            context->debugCxtBc = step.bc;
            context->debugCxtIp = step.ip;
            context->debugCxtBp = step.bp;
            break;
        }

        ns++;
        if (!step.ip)
            continue;
        if (context->debugCxtRc)
            context->debugCxtRc--;
    }
}

static void appendValue(Utf8& str, TypeInfo* typeInfo, void* addr)
{
    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        str += Fmt("0x%016llx", *(void**) addr);
        return;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            str += Fmt("%s", *(bool*) addr ? "true" : "false");
            return;

        case NativeTypeKind::S8:
            str += Fmt("%d", *(int8_t*) addr);
            return;
        case NativeTypeKind::S16:
            str += Fmt("%d", *(int16_t*) addr);
            return;
        case NativeTypeKind::S32:
            str += Fmt("%d", *(int32_t*) addr);
            return;
        case NativeTypeKind::Int:
        case NativeTypeKind::S64:
            str += Fmt("%lld", *(int64_t*) addr);
            return;

        case NativeTypeKind::U8:
            str += Fmt("%u", *(uint8_t*) addr);
            return;
        case NativeTypeKind::U16:
            str += Fmt("%u", *(uint16_t*) addr);
            return;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            str += Fmt("%u", *(uint32_t*) addr);
            return;
        case NativeTypeKind::UInt:
        case NativeTypeKind::U64:
            str += Fmt("%llu", *(uint64_t*) addr);
            return;

        case NativeTypeKind::F32:
            str += Fmt("%f", *(float*) addr);
            return;
        case NativeTypeKind::F64:
            str += Fmt("%lf", *(double*) addr);
            return;
        }
    }

    str += "?";
}

bool ByteCodeRun::debugger(ByteCodeRunContext* context)
{
    auto ip = context->ip;

    g_Log.lock();
    g_ByteCodeStackTrace.currentContext = context;

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
                Utf8::tokenize(line, ' ', cmds);
                for (auto& c : cmds)
                    c.trim();
                cmd = cmds[0];
            }

            // Help
            if (cmd == "?")
            {
                g_Log.setColor(LogColor::Gray);
                g_Log.eol();
                g_Log.print("<return>               runs the current instruction\n");
                g_Log.print("s                      runs to the next line\n");
                g_Log.print("n                      like s, but does not step into functions\n");
                g_Log.print("c                      runs until another break is reached\n");
                g_Log.print("f                      runs until the current function is done\n");
                g_Log.eol();

                g_Log.print("stk, stack             print callstack\n");
                g_Log.print("up [num]               move stack frame <num> level up\n");
                g_Log.print("down [num]             move stack frame <num> level down\n");
                g_Log.print("frame <num>            move stack frame to level <num>\n");
                g_Log.eol();

                g_Log.print("i                      print the current instruction\n");
                g_Log.print("i <num>                print the current instruction and <num> instructions around\n");
                g_Log.print("cxt, context           print contextual informations\n");
                g_Log.print("r                      print all registers\n");
                g_Log.print("r <num>                print register <num>\n");
                g_Log.print("list                   print current source code line\n");
                g_Log.print("bc, printbc            print current function bytecode\n");
                g_Log.eol();

                g_Log.print("x8  <addr|reg> [num]   print memory at address <addr> or register value <reg>, in 8 bits\n");
                g_Log.print("x16 <addr|reg> [num]   print memory at address or register value, in 16 bits\n");
                g_Log.print("x32 <addr|reg> [num]   print memory at address or register value, in 32 bits\n");
                g_Log.print("x64 <addr|reg> [num]   print memory at address or register value, in 64 bits\n");
                g_Log.eol();

                g_Log.print("info locals            print local variables\n");
                g_Log.print("info args              print function arguments\n");
                g_Log.eol();

                g_Log.print("?                      print this list of commands\n");
                g_Log.print("q, quit                quit the compiler\n");
                g_Log.eol();
                continue;
            }

            // Info locals
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

                        Utf8 str;
                        str = Fmt("%s (%s) = ", over->symbol->name.c_str(), over->typeInfo->getDisplayNameC());
                        appendValue(str, over->typeInfo, context->debugCxtBp + over->computedValue.storageOffset);
                        g_Log.printColor(str);
                        g_Log.eol();
                    }
                }

                continue;
            }

            // Info args
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
                        Utf8 str;
                        str = Fmt("%s (%s) = ", over->symbol->name.c_str(), over->typeInfo->getDisplayNameC());
                        appendValue(str, over->typeInfo, context->debugCxtBp + over->computedValue.storageOffset);
                        g_Log.printColor(str);
                        g_Log.eol();
                    }
                }

                continue;
            }

            // Print current instruction
            if (cmd == "list" && cmds.size() == 1)
            {
                context->debugCxtBc->printSourceCode(context->debugCxtIp);
                continue;
            }

            if (cmd == "x8" && cmds.size() >= 2)
            {
                int count = cmds.size() >= 3 ? atoi(cmds[2].c_str()) : 64;
                printMemory(context, context->debugCxtRc, cmds[1].c_str(), count, 8);
                continue;
            }

            if (cmd == "x16" && cmds.size() >= 2)
            {
                int count = cmds.size() >= 3 ? atoi(cmds[2].c_str()) : 64;
                printMemory(context, context->debugCxtRc, cmds[1].c_str(), count, 16);
                continue;
            }

            if (cmd == "x32" && cmds.size() >= 2)
            {
                int count = cmds.size() >= 3 ? atoi(cmds[2].c_str()) : 64;
                printMemory(context, context->debugCxtRc, cmds[1].c_str(), count, 32);
                continue;
            }

            if (cmd == "x64" && cmds.size() >= 2)
            {
                int count = cmds.size() >= 3 ? atoi(cmds[2].c_str()) : 64;
                printMemory(context, context->debugCxtRc, cmds[1].c_str(), count, 64);
                continue;
            }

            // Stack frame up
            if (cmd == "frame" && cmds.size() == 2)
            {
                uint32_t off                   = atoi(cmds[1].c_str());
                uint32_t maxLevel              = g_ByteCodeStackTrace.maxLevel(context);
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
                uint32_t maxLevel = g_ByteCodeStackTrace.maxLevel(context);
                if (context->debugStackFrameOffset == maxLevel)
                    g_Log.printColor("initial frame selected; you cannot go up\n");
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
                    g_Log.printColor("bottom(innermost) frame selected; you cannot go down\n");
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
                for (int i = 0; i < context->getRegCount(context->debugCxtRc); i++)
                    printRegister(context, context->debugCxtRc, i, true);
                continue;
            }

            // Print one register
            if (cmd == "r" && cmds.size() == 2)
            {
                g_Log.setColor(LogColor::Gray);
                int regN = atoi(cmds[1].c_str());
                if (regN >= context->getRegCount(context->debugCxtRc))
                    g_Log.print(Fmt("invalid register number, maximum value is '%u'\n", (uint32_t) context->getRegCount(context->debugCxtRc)));
                else
                {
                    auto& regP = context->getRegBuffer(context->debugCxtRc)[regN];
                    printFullRegister(regP);
                }

                continue;
            }

            // Stack
            if (cmd == "stk" || cmd == "stack")
            {
                g_ByteCodeStackTrace.currentContext = context;
                g_ByteCodeStackTrace.log();
                g_ByteCodeStackTrace.currentContext = nullptr;
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
