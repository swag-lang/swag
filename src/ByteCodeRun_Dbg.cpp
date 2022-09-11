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

#pragma optimize("", off)

struct EvaluateResult
{
    TypeInfo*      type  = nullptr;
    void*          addr  = nullptr;
    ComputedValue* value = nullptr;
    void*          storage[2];
};

static bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res)
{
    auto funcDecl = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
    for (auto l : context->debugCxtBc->localVars)
    {
        auto over = l->resolvedSymbolOverload;
        if (over && over->symbol->name == expr)
        {
            res.type = over->typeInfo;
            res.addr = context->debugCxtBp + over->computedValue.storageOffset;
            return true;
        }
    }

    if (funcDecl->parameters)
    {
        for (auto l : funcDecl->parameters->childs)
        {
            auto over = l->resolvedSymbolOverload;
            if (over && over->symbol->name == expr)
            {
                res.type = over->typeInfo;
                res.addr = context->debugCxtBp + over->computedValue.storageOffset;
                return true;
            }
        }
    }

    auto sourceFile = context->debugCxtBc->sourceFile;
    sourceFile->silentError.clear();

    // Syntax
    SyntaxJob syntaxJob;
    AstNode   parent;
    syntaxJob.module  = sourceFile->module;
    parent.ownerScope = context->debugCxtIp->node ? context->debugCxtIp->node->ownerScope : nullptr;
    parent.ownerFct   = CastAst<AstFuncDecl>(context->bc->node, AstNodeKind::FuncDecl);
    parent.sourceFile = sourceFile;
    if (!syntaxJob.constructEmbedded(expr, &parent, nullptr, CompilerAstKind::Expression, false, true))
    {
        if (sourceFile->silentError.empty())
            g_Log.printColor("expression syntax error\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    // Semantic
    auto        child = parent.childs.front();
    SemanticJob semanticJob;
    semanticJob.sourceFile = sourceFile;
    sourceFile->silent++;
    semanticJob.nodes.push_back(child);
    auto result = semanticJob.execute();
    if (result != JobResult::ReleaseJob || !sourceFile->silentError.empty())
    {
        sourceFile->silent--;
        if (sourceFile->silentError.empty())
            g_Log.printColor("cannot solve expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    // This has been evaluated to a constant
    if (child->flags & AST_VALUE_COMPUTED)
    {
        sourceFile->silent--;
        res.type  = child->typeInfo;
        res.value = child->computedValue;
        return true;
    }

    // Gen bytecode for expression
    ByteCodeGenJob genJob;
    genJob.sourceFile = sourceFile;
    genJob.nodes.push_back(child);
    child->allocateExtension();
    child->extension->bc             = g_Allocator.alloc<ByteCode>();
    child->extension->bc->node       = child;
    child->extension->bc->sourceFile = sourceFile;
    result                           = genJob.execute();
    if (result != JobResult::ReleaseJob || !sourceFile->silentError.empty())
    {
        sourceFile->silent--;
        if (child->sourceFile->silentError.empty())
            g_Log.printColor("cannot generate expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", child->sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    // Execute node
    JobContext         callerContext;
    ByteCodeRunContext runContext;
    ExecuteNodeParams  execParams;
    execParams.runContext        = &runContext;
    execParams.inheritRunContext = &g_RunContext;
    execParams.forDebugger       = true;
    if (!sourceFile->module->executeNode(sourceFile, child, &callerContext, &execParams))
    {
        sourceFile->silent--;
        if (child->sourceFile->silentError.empty())
            g_Log.printColor("cannot run expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", child->sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    sourceFile->silent--;
    res.type = TypeManager::concreteReferenceType(child->typeInfo);
    if (res.type->flags & TYPEINFO_RETURN_BY_COPY)
        res.addr = execParams.debuggerResult[0];
    else
    {
        res.storage[0] = execParams.debuggerResult[0];
        res.storage[1] = execParams.debuggerResult[1];
        res.addr       = &res.storage[0];
    }

    return true;
}

static bool getRegIdx(ByteCodeRunContext* context, const Utf8& arg, int& regN)
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

template<typename T>
static T getAddrValue(const void* addr)
{
    SWAG_TRY
    {
        return *(T*) addr;
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }
}

struct ValueFormat
{
    int  bitCount = 8;
    bool isSigned = false;
    bool isFloat  = false;
    bool isHexa   = true;
};

static bool getValueFormat(const Utf8& cmd, ValueFormat& fmt)
{
    // Format
    if (cmd == "s8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "s16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "s32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "s64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "u8")
    {
        fmt.bitCount = 8;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "u16")
    {
        fmt.bitCount = 16;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "u32")
    {
        fmt.bitCount = 32;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "u64")
    {
        fmt.bitCount = 64;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "x8")
    {
        fmt.bitCount = 8;
        return true;
    }

    if (cmd == "x16")
    {
        fmt.bitCount = 16;
        return true;
    }

    if (cmd == "x32")
    {
        fmt.bitCount = 32;
        return true;
    }

    if (cmd == "x64")
    {
        fmt.bitCount = 64;
        return true;
    }

    if (cmd == "f32")
    {
        fmt.bitCount = 32;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "f64")
    {
        fmt.bitCount = 64;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    return false;
}

static void printValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    switch (fmt.bitCount)
    {
    case 8:
    default:
        if (fmt.isSigned)
            result = Fmt("%4d ", getAddrValue<int8_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%3u ", getAddrValue<uint8_t>(addr));
        else
            result = Fmt("%02llx ", getAddrValue<uint8_t>(addr));
        break;

    case 16:
        if (fmt.isSigned)
            result = Fmt("%6d ", getAddrValue<int16_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%5u ", getAddrValue<uint16_t>(addr));
        else
            result = Fmt("%04llx ", getAddrValue<uint16_t>(addr));
        break;

    case 32:
        if (fmt.isFloat)
            result = Fmt("%16.5g ", getAddrValue<float>(addr));
        else if (fmt.isSigned)
            result = Fmt("%11d ", getAddrValue<int32_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%10u ", getAddrValue<uint32_t>(addr));
        else
            result = Fmt("%08llx ", getAddrValue<uint32_t>(addr));
        break;

    case 64:
        if (fmt.isFloat)
            result = Fmt("%16.5g ", getAddrValue<double>(addr));
        else if (fmt.isSigned)
            result = Fmt("%21lld ", getAddrValue<int64_t>(addr));
        else if (!fmt.isHexa)
            result = Fmt("%20llu ", getAddrValue<uint64_t>(addr));
        else
            result = Fmt("%016llx ", getAddrValue<uint64_t>(addr));
        break;
    }
}

static void printValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    SWAG_TRY
    {
        printValueProtected(context, result, fmt, addr);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        result = "<error>";
    }
}

static void printMemory(ByteCodeRunContext* context, const Utf8& arg)
{
    vector<Utf8> cmds;
    Utf8::tokenize(arg.c_str(), ' ', cmds);

    ValueFormat fmt;
    int         startIdx = 0;
    if (cmds.size() && getValueFormat(cmds[0], fmt))
        startIdx++;

    // Count
    int count = 64;
    if (startIdx < cmds.size() && isdigit(cmds[startIdx][0]))
    {
        count = atoi(cmds[startIdx]);
        startIdx++;
    }

    count = max(count, 1);
    count = min(count, 4096);

    // Expression
    Utf8 expr;
    for (int i = startIdx; i < cmds.size(); i++)
    {
        expr += cmds[i];
        expr += " ";
    }

    expr.trim();
    if (expr.empty())
    {
        g_Log.printColor("empty expression\n", LogColor::Red);
        return;
    }

    uint64_t addrVal = 0;
    auto     addr    = expr.c_str();

    if (expr[0] == '$')
    {
        expr.remove(0, 1);
        if (expr[0] == 'r')
        {
            int regN;
            if (!getRegIdx(context, expr, regN))
                return;
            auto& regP = context->getRegBuffer(context->debugCxtRc)[regN];
            addrVal    = regP.u64;
        }
        else if (expr == "sp")
        {
            addrVal = (uint64_t) context->sp;
        }
    }
    else if (isdigit(expr[0]))
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
    else
    {
        EvaluateResult res;
        if (!evalExpression(context, expr, res))
            return;
        if (!res.addr)
            res.addr = &res.value->reg;
        if (res.type->kind == TypeInfoKind::Pointer)
            addrVal = *(uint64_t*) res.addr;
        else
            addrVal = (uint64_t) res.addr;
    }

    int perLine = 8;
    switch (fmt.bitCount)
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
            Utf8 str;
            printValue(context, str, fmt, addrB);
            g_Log.print(str);
            addrB += fmt.bitCount / 8;
        }

        addrB = addrLine;
        if (fmt.bitCount == 8)
        {
            for (int i = 0; i < perLine - min(count, perLine); i++)
            {
                if (fmt.isHexa)
                    g_Log.print("   ");
                else if (!fmt.isSigned)
                    g_Log.print("    ");
                else
                    g_Log.print("     ");
            }

            g_Log.print(" ");
            for (int i = 0; i < min(count, perLine); i++)
            {
                auto c = getAddrValue<uint8_t>(addrB);
                if (c < 32 || c > 127)
                    c = '.';
                g_Log.print(Fmt("%c", c));
                addrB += 1;
            }
        }

        g_Log.eol();

        addrB = addrLine;
        addrB += count * (fmt.bitCount / 8);
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

    g_Log.messageHeaderDot("bytecode name", bc->name.c_str(), LogColor::Gray, LogColor::Gray, " ");
    g_Log.messageHeaderDot("bytecode type", bc->getCallType()->getDisplayNameC(), LogColor::Gray, LogColor::Gray, " ");

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

static void appendValueProtected(Utf8& str, const EvaluateResult& res, int indent = 0)
{
    auto typeInfo = res.type;
    auto addr     = res.addr;

    if (!addr && res.value)
        addr = res.value->reg.pointer;

    if (typeInfo->isPointerToTypeInfo())
    {
        auto ptr = ((ConcreteTypeInfo**) addr)[0];
        Utf8 str1;
        str1.setView((const char*) ptr->name.buffer, (int) ptr->name.count);
        str += str1;
        return;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (res.value)
            addr = res.value->reg.pointer;
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        str += Fmt("0x%016llx\n", addr);
        for (auto p : typeStruct->fields)
        {
            for (int i = 0; i < indent; i++)
                str += "   ";
            str += " | ";
            str += p->namedParam.c_str();
            str += ": ";
            str += p->typeInfo->getDisplayName().c_str();
            str += " = ";
            EvaluateResult res1;
            res1.type = p->typeInfo;
            res1.addr = ((uint8_t*) addr) + p->offset;
            appendValueProtected(str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        str += Fmt("0x%016llx\n", addr);
        for (uint32_t idx = 0; idx < typeArray->count; idx++)
        {
            for (int i = 0; i < indent; i++)
                str += "   ";
            str += Fmt(" [%d] ", idx);
            EvaluateResult res1;
            res1.type = typeArray->pointedType;
            res1.addr = ((uint8_t*) addr) + (idx * typeArray->pointedType->sizeOf);
            appendValueProtected(str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto count     = ((uint64_t*) addr)[1];
        auto ptr       = ((uint8_t**) addr)[0];
        str += Fmt("(0x%016llx ", ptr);
        str += Fmt("%llu)\n", count);
        for (uint64_t idx = 0; idx < count; idx++)
        {
            for (int i = 0; i < indent; i++)
                str += "   ";
            str += Fmt(" [%d] ", idx);
            EvaluateResult res1;
            res1.type = typeSlice->pointedType;
            res1.addr = ptr + (idx * typeSlice->pointedType->sizeOf);
            appendValueProtected(str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        str += Fmt("0x%016llx", *(void**) addr);
        return;
    }

    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        str += Fmt("(0x%016llx ", ((void**) addr)[0]);
        str += Fmt("0x%016llx)", ((void**) addr)[1]);
        return;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::String:
        {
            void*    ptr;
            uint64_t len;

            if (res.value)
            {
                ptr = res.value->text.buffer;
                len = res.value->text.length();
            }
            else
            {
                ptr = ((void**) addr)[0];
                len = ((uint64_t*) addr)[1];
            }

            if (!ptr || !len)
                str += "<empty>";
            else
            {
                Utf8 str1;
                str1.resize((int) len);
                memcpy(str1.buffer, ptr, len);
                str += "\"";
                str += str1;
                str += "\"";
            }

            return;
        }

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

static void appendValue(Utf8& str, const EvaluateResult& res, int indent = 0)
{
    SWAG_TRY
    {
        appendValueProtected(str, res, indent);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        str += "<error>";
    }
}

static void printHelp()
{
    g_Log.setColor(LogColor::Gray);
    g_Log.eol();
    g_Log.print("<return>                   runs to the next line or instruction (depends on 'bcmode')\n");
    g_Log.eol();
    g_Log.print("s(tep)                     runs to the next line\n");
    g_Log.print("n(ext)                     like s, but does not step into functions\n");
    g_Log.print("f(inish)                   runs until the current function is done\n");
    g_Log.print("c(ont(inue))               runs until another breakpoint is reached\n");
    g_Log.print("u(ntil) <num>              runs to the given line or instruction (depends on 'bcmode')\n");
    g_Log.print("j(ump) <num>               jump to the given line or instruction (depends on 'bcmode')\n");
    g_Log.eol();

    g_Log.print("l(ist) [num]               print the current source code line and <num> lines around\n");
    g_Log.print("ll                         print the current function source code\n");

    g_Log.eol();
    g_Log.print("cxt                        print contextual informations\n");
    g_Log.print("bcmode                     swap between bytecode mode and source mode\n");
    g_Log.eol();

    g_Log.print("p <name>                   print the value of <name>\n");
    g_Log.print("loc(als)                   print all current local variables\n");
    g_Log.print("a(rgs)                     print all current function arguments\n");
    g_Log.eol();

    g_Log.print("b(reak)                    print all breakpoints\n");
    g_Log.print("b(reak) fct <name>         add breakpoint when entering function <name>\n");
    g_Log.print("b(reak) <line>             add breakpoint in the current source file at line <line>\n");
    g_Log.print("b(reak) clear              remove all breakpoints\n");
    g_Log.print("b(reak) clear <num>        remove breakpoint <num>\n");
    g_Log.print("b(reak) enable <num>       enable breakpoint <num>\n");
    g_Log.print("b(reak) disable <num>      disable breakpoint <num>\n");
    g_Log.eol();

    g_Log.print("stack                      print callstack\n");
    g_Log.print("up [num]                   move stack frame <num> level up\n");
    g_Log.print("down [num]                 move stack frame <num> level down\n");
    g_Log.print("frame <num>                move stack frame to level <num>\n");
    g_Log.eol();

    g_Log.print("i [num]                    print the current bytecode instruction and <num> instructions around\n");
    g_Log.print("r(egs) [format]            print all registers (format is the same as 'x' command)\n");
    g_Log.print("r<num> [format]            print register <num> (format is the same as 'x' command)\n");
    g_Log.print("bc                         print the current function bytecode\n");
    g_Log.eol();

    g_Log.print("x [format] [num] <expr>    print memory (format = s8|s16|s32|s64|u8|u16|u32|u64|x8|x16|x32|x64|f32|f64)\n");
    g_Log.print("x [format] [num] $r<num>   print memory at current register value\n");
    g_Log.print("x [format] [num] $sp       print memory at current stack pointer\n");
    g_Log.eol();

    g_Log.print("?                          print this list of commands\n");
    g_Log.print("q(uit)                     quit the compiler\n");
    g_Log.eol();
}

static void printBreakpoints(ByteCodeRunContext* context)
{
    if (context->debugBreakpoints.empty())
    {
        g_Log.printColor("no breakpoint\n", LogColor::Red);
        return;
    }

    g_Log.setColor(LogColor::White);
    for (int i = 0; i < context->debugBreakpoints.size(); i++)
    {
        const auto& bkp = context->debugBreakpoints[i];
        g_Log.print(Fmt("#%d: ", i + 1));
        switch (bkp.type)
        {
        case ByteCodeRunContext::DebugBkpType::FuncName:
            g_Log.print(Fmt("entering function '%s'", bkp.name.c_str()));
            break;
        case ByteCodeRunContext::DebugBkpType::FileLine:
            g_Log.print(Fmt("file %s, line '%d'", bkp.name.c_str(), bkp.line));
            break;
        case ByteCodeRunContext::DebugBkpType::InstructionIndex:
            g_Log.print(Fmt("instruction '%d'", bkp.line));
            break;
        }

        if (bkp.disabled)
            g_Log.print(" (DISABLED)");
        g_Log.eol();
    }
}

static void checkBreakpoints(ByteCodeRunContext* context)
{
    for (auto it = context->debugBreakpoints.begin(); it != context->debugBreakpoints.end(); it++)
    {
        auto& bkp = *it;
        if (bkp.disabled)
            continue;

        if (bkp.type == ByteCodeRunContext::DebugBkpType::FuncName)
        {
            if (context->bc->node && (context->bc->node->token.text == bkp.name || context->bc->name == bkp.name))
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.printColor(Fmt("#### breakpoint hit entering function '%s' ####\n", context->bc->name.c_str()), LogColor::Magenta);
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn       = true;
                    bkp.autoDisabled       = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    break;
                }
            }
            else if (context->bc->node && context->bc->node->token.text != bkp.name && context->bc->name != bkp.name)
            {
                bkp.autoDisabled = false;
            }
        }
        else if (bkp.type == ByteCodeRunContext::DebugBkpType::FileLine)
        {
            SourceFile*     file;
            SourceLocation* location;
            ByteCode::getLocation(context->bc, context->ip, &file, &location);
            if (file->name == bkp.name && location->line == bkp.line)
            {
                if (!bkp.autoDisabled)
                {
                    g_Log.printColor(Fmt("#### breakpoint hit at line '%d' ####\n", bkp.line), LogColor::Magenta);
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                    context->debugOn       = true;
                    if (bkp.autoRemove)
                        context->debugBreakpoints.erase(it);
                    else
                        bkp.autoDisabled = true;
                    break;
                }
            }
            else
            {
                bkp.autoDisabled = false;
            }
        }
        else if (bkp.type == ByteCodeRunContext::DebugBkpType::InstructionIndex)
        {
            uint32_t offset = (uint32_t) (context->ip - context->bc->out);
            if (offset == bkp.line)
            {
                g_Log.printColor(Fmt("#### breakpoint hit at instruction '%d' ####\n", bkp.line), LogColor::Magenta);
                context->debugStepMode = ByteCodeRunContext::DebugStepMode::None;
                context->debugOn       = true;
                if (bkp.autoRemove)
                    context->debugBreakpoints.erase(it);
                else
                    bkp.autoDisabled = true;
                break;
            }
        }
    }
}

static bool addBreakpoint(ByteCodeRunContext* context, const ByteCodeRunContext::DebugBreakpoint& bkp)
{
    for (const auto& b : context->debugBreakpoints)
    {
        if (b.type == bkp.type && b.name == bkp.name && b.line == bkp.line)
        {
            g_Log.printColor("breakpoint already exists\n", LogColor::Red);
            return false;
        }
    }

    context->debugBreakpoints.push_back(bkp);
    return true;
}

static void setContextInstruction(ByteCodeRunContext* context)
{
    // Print function name
    if (context->debugLastBc != context->bc)
        g_Log.printColor(Fmt("--> function %s %s\n", context->bc->name.c_str(), context->bc->getCallType()->getDisplayNameC()), LogColor::Yellow);

    // Print source line
    SourceFile*     file;
    SourceLocation* location;
    ByteCode::getLocation(context->bc, context->ip, &file, &location);
    if (location && (context->debugStepLastFile != file || context->debugStepLastLocation && context->debugStepLastLocation->line != location->line))
    {
        if (context->bc->node && context->bc->node->sourceFile)
        {
            g_Log.printColor("--> ", LogColor::Yellow);
            auto str1 = context->bc->node->sourceFile->getLine(location->line);
            str1.trim();
            g_Log.printColor(str1, LogColor::Yellow);
            g_Log.eol();
        }
    }

    // Print instruction
    if (context->debugBcMode)
        printInstruction(context, context->bc, context->ip);

    context->debugLastBc           = context->bc;
    context->debugStepLastFile     = file;
    context->debugStepLastLocation = location;
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
        g_Log.printColor("#############################################\n", LogColor::Magenta);
        g_Log.printColor("entering bytecode debugger, type '?' for help\n", LogColor::Magenta);
        g_Log.printColor("#############################################\n", LogColor::Magenta);
        g_Log.eol();
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
        ByteCode::getLocation(context->bc, ip, &file, &location);
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
        setContextInstruction(context);

        computeCxt(context);

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
            Utf8         cmdExpr;
            vector<Utf8> cmds;
            if (!line.empty())
            {
                Utf8::tokenize(line, ' ', cmds);
                for (auto& c : cmds)
                    c.trim();
                cmd = cmds[0];
            }

            for (int i = 1; i < cmds.size(); i++)
                cmdExpr += cmds[i] + " ";
            cmdExpr.trim();

            // Help
            if (cmd == "?")
            {
                printHelp();
                continue;
            }

            // Info name
            if (cmd == "p" && cmds.size() >= 2)
            {
                EvaluateResult res;
                if (evalExpression(context, cmdExpr, res))
                {
                    Utf8 str = Fmt("%s: ", res.type->getDisplayNameC());
                    appendValue(str, res);
                    g_Log.printColor(str);
                    if (str.back() != '\n')
                        g_Log.eol();
                }

                continue;
            }

            // Info locals
            if ((cmd == "loc" || cmd == "locals") && cmds.size() == 1)
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
                        Utf8           str = Fmt("%s: %s = ", over->symbol->name.c_str(), over->typeInfo->getDisplayNameC());
                        EvaluateResult res;
                        res.type = over->typeInfo;
                        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
                        appendValue(str, res);
                        g_Log.printColor(str);
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Info args
            if ((cmd == "a" || cmd == "args") && cmds.size() == 1)
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
                        Utf8           str = Fmt("%s: %s = ", over->symbol->name.c_str(), over->typeInfo->getDisplayNameC());
                        EvaluateResult res;
                        res.type = over->typeInfo;
                        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
                        appendValue(str, res);
                        g_Log.printColor(str);
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Print memory
            if (cmd == "x" && cmds.size() >= 2)
            {
                printMemory(context, cmdExpr);
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
                    g_Log.printColor("initial frame selected; you cannot go up\n", LogColor::Red);
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
                    g_Log.printColor("bottom(innermost) frame selected; you cannot go down\n", LogColor::Red);
                else
                {
                    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
                    computeCxt(context);
                    printInstruction(context, context->debugCxtBc, context->debugCxtIp);
                }

                continue;
            }

            // Exit
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
            if (cmd == "cxt")
            {
                printContext(context);
                continue;
            }

            // Print all registers
            if ((cmd == "r" || cmd == "regs") && (cmds.size() == 1 || cmds.size() == 2))
            {
                ValueFormat fmt;
                fmt.isHexa   = true;
                fmt.bitCount = 64;
                if (cmds.size() > 1)
                {
                    if (!getValueFormat(cmds[1], fmt))
                    {
                        g_Log.printColor("invalid format\n", LogColor::Red);
                        continue;
                    }
                }

                g_Log.setColor(LogColor::White);
                for (int i = 0; i < context->getRegCount(context->debugCxtRc); i++)
                {
                    auto& regP = context->getRegBuffer(context->debugCxtRc)[i];
                    Utf8  str;
                    printValue(context, str, fmt, &regP);
                    str.trim();
                    g_Log.print(Fmt("r%d = ", i));
                    g_Log.print(str);
                    g_Log.eol();
                }

                g_Log.print(Fmt("sp = %016llx\n", context->sp));
                continue;
            }

            // Print one register
            if (cmd[0] == 'r' && cmd.length() > 1 && isdigit(cmd[1]) && (cmds.size() == 1 || cmds.size() == 2))
            {
                g_Log.setColor(LogColor::Gray);
                int regN;
                if (!getRegIdx(context, cmd, regN))
                    continue;
                auto& regP = context->getRegBuffer(context->debugCxtRc)[regN];

                ValueFormat fmt;
                fmt.isHexa   = true;
                fmt.bitCount = 64;
                if (cmds.size() > 1)
                {
                    if (!getValueFormat(cmds[1], fmt))
                    {
                        g_Log.printColor("invalid format\n", LogColor::Red);
                        continue;
                    }
                }

                g_Log.setColor(LogColor::White);
                Utf8 str;
                printValue(context, str, fmt, &regP);
                str.trim();
                g_Log.print(str);
                g_Log.eol();
                continue;
            }

            // Print stack
            if (cmd == "stack")
            {
                g_ByteCodeStackTrace.currentContext = context;
                g_ByteCodeStackTrace.log();
                g_ByteCodeStackTrace.currentContext = nullptr;
                continue;
            }

            // Bytecode dump
            if (cmd == "bc")
            {
                g_Log.unlock();
                context->debugCxtBc->print(context->debugCxtIp);
                g_Log.lock();
                continue;
            }

            // Bc mode
            if (cmd == "bcmode")
            {
                context->debugBcMode = !context->debugBcMode;
                if (context->debugBcMode)
                    g_Log.printColor("bytecode mode\n", LogColor::White);
                else
                    g_Log.printColor("source code mode\n", LogColor::White);
                continue;
            }

            // Function code
            if (((cmd == "l" || cmd == "list") && cmds.size() <= 2) ||
                (cmd == "ll" && cmds.size() == 1))
            {
                if (context->debugCxtBc->node && context->debugCxtBc->node->kind == AstNodeKind::FuncDecl && context->debugCxtBc->node->sourceFile)
                {
                    SourceFile*     curFile;
                    SourceLocation* curLocation;
                    ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &curFile, &curLocation);

                    auto funcNode = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);

                    uint32_t startLine = context->debugCxtBc->node->token.startLocation.line;
                    uint32_t endLine   = funcNode->content->token.endLocation.line;
                    if (cmd == "l" && cmds.size() == 1)
                    {
                        startLine = curLocation->line;
                        endLine   = startLine;
                    }
                    else if (cmd == "l" && cmds.size() == 2)
                    {
                        uint32_t offset = atoi(cmds[1]);
                        if (offset > curLocation->line)
                            startLine = 0;
                        else
                            startLine = curLocation->line - offset;
                        endLine = curLocation->line + offset;
                    }

                    vector<Utf8> lines;
                    bool         eof = false;
                    for (uint32_t l = startLine; l <= endLine && !eof; l++)
                        lines.push_back(context->debugCxtBc->node->sourceFile->getLine(l, &eof));

                    g_Log.setColor(LogColor::Yellow);
                    uint32_t lineIdx = 0;
                    for (const auto& l : lines)
                    {
                        g_Log.print(Fmt("%08u ", startLine + lineIdx));
                        if (curLocation->line == startLine + lineIdx)
                            g_Log.print(" --> ");
                        else
                            g_Log.print("     ");
                        g_Log.print(l.c_str());
                        g_Log.eol();
                        lineIdx++;
                    }
                }
                else
                    g_Log.printColor("no source code\n", LogColor::Red);
                continue;
            }

            // Breakpoints
            if (cmd == "b" || cmd == "break")
            {
                if (cmds.size() == 1)
                {
                    printBreakpoints(context);
                    continue;
                }

                if (cmds.size() == 3 && cmds[1] == "fct")
                {
                    ByteCodeRunContext::DebugBreakpoint bkp;
                    bkp.type = ByteCodeRunContext::DebugBkpType::FuncName;
                    bkp.name = cmds[2];
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("set breakpoint #%d entering function '%s'\n", context->debugBreakpoints.size(), bkp.name.c_str()), LogColor::White);
                    continue;
                }

                if (cmds.size() == 2 && isdigit(cmds[1][0]))
                {
                    SourceFile*     curFile;
                    SourceLocation* curLocation;
                    ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &curFile, &curLocation);

                    int                                 lineNo = atoi(cmds[1]);
                    ByteCodeRunContext::DebugBreakpoint bkp;
                    bkp.type = ByteCodeRunContext::DebugBkpType::FileLine;
                    bkp.name = curFile->name;
                    bkp.line = lineNo;
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("set breakpoint #%d at line '%d'\n", context->debugBreakpoints.size(), lineNo), LogColor::White);
                    continue;
                }

                if (cmds.size() == 2 && cmds[1] == "clear")
                {
                    if (context->debugBreakpoints.empty())
                        g_Log.printColor("no breakpoint\n", LogColor::Red);
                    else
                    {
                        g_Log.printColor(Fmt("%d breakpoint(s) have been removed\n", context->debugBreakpoints.size()), LogColor::White);
                        context->debugBreakpoints.clear();
                    }

                    continue;
                }

                if (cmds.size() == 3 && cmds[1] == "clear" && isdigit(cmds[2][0]))
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints.erase(context->debugBreakpoints.begin() + numB - 1);
                        g_Log.printColor(Fmt("breakpoint #%d has been removed\n", numB), LogColor::White);
                    }

                    continue;
                }

                if (cmds.size() == 3 && cmds[1] == "disable" && isdigit(cmds[2][0]))
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints[numB - 1].disabled = true;
                        g_Log.printColor(Fmt("breakpoint #%d has been disabled\n", numB), LogColor::White);
                    }

                    continue;
                }

                if (cmds.size() == 3 && cmds[1] == "enable" && isdigit(cmds[2][0]))
                {
                    int numB = atoi(cmds[2]);
                    if (!numB || numB - 1 >= context->debugBreakpoints.size())
                        g_Log.printColor("invalid breakpoint number\n", LogColor::Red);
                    else
                    {
                        context->debugBreakpoints[numB - 1].disabled = false;
                        g_Log.printColor(Fmt("breakpoint #%d has been enabled\n", numB), LogColor::White);
                    }

                    continue;
                }
            }

            // Next instruction
            if (cmd.empty())
            {
                if (context->debugBcMode)
                    break;
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
                break;
            }

            // Step to next line
            if ((cmd == "s" || cmd == "step") && cmds.size() == 1)
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
                break;
            }

            // Step to next line, step out
            if ((cmd == "n" || cmd == "next") && cmds.size() == 1)
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                context->debugStepRC           = context->curRC;
                break;
            }

            // Run to the end of the current function
            if ((cmd == "f" || cmd == "finish") && cmds.size() == 1)
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC           = context->curRC - 1;
                break;
            }

            // Continue
            if ((cmd == "c" || cmd == "cont" || cmd == "continue") && cmds.size() == 1)
            {
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
                context->debugOn               = false;
                break;
            }

            // Jump to line/instruction
            if ((cmd == "j" || cmd == "jump") && cmds.size() == 2 && isdigit(cmds[1][0]))
            {
                context->debugStackFrameOffset = 0;

                uint32_t to = (uint32_t) atoi(cmds[1]);
                if (to >= context->bc->numInstructions - 1)
                {
                    g_Log.printColor("invalid jump destination\n", LogColor::Red);
                    continue;
                }

                if (context->debugBcMode)
                {
                    context->ip         = context->bc->out + to;
                    context->debugCxtIp = context->ip;
                }
                else
                {
                    auto curIp = context->ip;
                    while (true)
                    {
                        if (curIp >= context->bc->out + context->bc->numInstructions - 1)
                        {
                            g_Log.printColor("invalid jump destination\n", LogColor::Red);
                            break;
                        }

                        SourceFile*     file;
                        SourceLocation* location;
                        ByteCode::getLocation(context->bc, curIp, &file, &location);
                        if (location && location->line == to)
                        {
                            context->ip         = curIp;
                            context->debugCxtIp = context->ip;
                            break;
                        }

                        curIp++;
                    }
                }

                setContextInstruction(context);
                continue;
            }

            // Run until a line/instruction is reached
            if ((cmd == "u" || cmd == "until") && cmds.size() == 2 && isdigit(cmds[1][0]))
            {
                ByteCodeRunContext::DebugBreakpoint bkp;
                if (context->debugBcMode)
                    bkp.type = ByteCodeRunContext::DebugBkpType::InstructionIndex;
                else
                    bkp.type = ByteCodeRunContext::DebugBkpType::FileLine;
                bkp.name       = context->debugStepLastFile->name;
                bkp.line       = atoi(cmds[1]);
                bkp.autoRemove = true;
                addBreakpoint(context, bkp);
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
                break;
            }

            g_Log.printColor("invalid command or argument, type '?' for help\n", LogColor::Red);
        }
    }

    g_Log.unlock();

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
