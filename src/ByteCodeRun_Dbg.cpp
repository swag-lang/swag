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

struct EvaluateResult
{
    TypeInfo*      type  = nullptr;
    void*          addr  = nullptr;
    ComputedValue* value = nullptr;
    void*          storage[2];
};

static bool evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent = false)
{
    auto sourceFile = context->debugCxtBc->sourceFile;
    sourceFile->silentError.clear();

    // Syntax
    SyntaxJob syntaxJob;
    AstNode   parent;
    syntaxJob.module  = sourceFile->module;
    parent.ownerScope = context->debugCxtIp->node ? context->debugCxtIp->node->ownerScope : nullptr;
    parent.ownerFct   = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
    parent.sourceFile = sourceFile;
    parent.parent     = context->debugCxtIp->node;
    if (!syntaxJob.constructEmbedded(expr, &parent, nullptr, kind, false, true))
    {
        if (silent)
            return false;
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
    semanticJob.module     = sourceFile->module;
    sourceFile->silent++;
    semanticJob.nodes.push_back(child);

    g_ThreadMgr.doJobCount = true;
    while (true)
    {
        auto result = semanticJob.execute();
        if (result == JobResult::ReleaseJob)
            break;
        while (g_ThreadMgr.addJobCount.load())
            this_thread::yield();
    }
    g_ThreadMgr.doJobCount = false;

    if (!sourceFile->silentError.empty())
    {
        sourceFile->silent--;
        if (silent)
            return false;
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
    genJob.module     = sourceFile->module;
    genJob.nodes.push_back(child);
    child->allocateExtension(ExtensionKind::ByteCode);
    child->extension->bytecode->bc             = g_Allocator.alloc<ByteCode>();
    child->extension->bytecode->bc->node       = child;
    child->extension->bytecode->bc->sourceFile = sourceFile;

    g_ThreadMgr.doJobCount = true;
    while (true)
    {
        auto result = genJob.execute();
        if (result == JobResult::ReleaseJob)
            break;
        while (g_ThreadMgr.addJobCount.load())
            this_thread::yield();
    }
    g_ThreadMgr.doJobCount = false;

    if (!sourceFile->silentError.empty())
    {
        sourceFile->silent--;
        if (silent)
            return false;
        if (sourceFile->silentError.empty())
            g_Log.printColor("cannot generate expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", child->sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    // Execute node
    JobContext         callerContext;
    ByteCodeRunContext runContext;
    ByteCodeStack      stackTrace;
    ExecuteNodeParams  execParams;
    execParams.inheritSp    = context->debugCxtSp;
    execParams.inheritSpAlt = context->debugCxtSpAlt;
    execParams.inheritStack = context->debugCxtStack;
    execParams.inheritBp    = context->debugCxtBp;
    execParams.forDebugger  = true;
    runContext.debugAccept  = false;
    runContext.sharedStack  = true;

    g_RunContext         = &runContext;
    g_ByteCodeStackTrace = &stackTrace;

    auto resExec = sourceFile->module->executeNode(sourceFile, child, &callerContext, &execParams);

    child->extension->bytecode->bc->releaseOut();
    sourceFile->silent--;
    g_RunContext         = &g_RunContextVal;
    g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

    if (!resExec)
    {
        if (silent)
            return false;
        if (child->sourceFile->silentError.empty())
            g_Log.printColor("cannot run expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", child->sourceFile->silentError.c_str()), LogColor::Red);
        return false;
    }

    res.type       = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
    res.storage[0] = runContext.registersRR[0].pointer;
    res.storage[1] = runContext.registersRR[1].pointer;
    if (res.type->flags & TYPEINFO_RETURN_BY_COPY)
        res.addr = res.storage[0];
    else
        res.addr = &res.storage[0];
    return true;
}

static bool evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent = false)
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

    return evalDynExpression(context, expr, res, CompilerAstKind::Expression, silent);
}

static bool isNumber(const char* pz)
{
    while (*pz)
    {
        if (!isdigit(*pz))
            return false;
        pz++;
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
    if (cmd == "@s8")
    {
        fmt.bitCount = 8;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@s16")
    {
        fmt.bitCount = 16;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@s32")
    {
        fmt.bitCount = 32;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@s64")
    {
        fmt.bitCount = 64;
        fmt.isSigned = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@u8")
    {
        fmt.bitCount = 8;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@u16")
    {
        fmt.bitCount = 16;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@u32")
    {
        fmt.bitCount = 32;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@u64")
    {
        fmt.bitCount = 64;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@x8")
    {
        fmt.bitCount = 8;
        return true;
    }

    if (cmd == "@x16")
    {
        fmt.bitCount = 16;
        return true;
    }

    if (cmd == "@x32")
    {
        fmt.bitCount = 32;
        return true;
    }

    if (cmd == "@x64")
    {
        fmt.bitCount = 64;
        return true;
    }

    if (cmd == "@f32")
    {
        fmt.bitCount = 32;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    if (cmd == "@f64")
    {
        fmt.bitCount = 64;
        fmt.isFloat  = true;
        fmt.isHexa   = false;
        return true;
    }

    return false;
}

static void appendLiteralValueProtected(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
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

static void appendLiteralValue(ByteCodeRunContext* context, Utf8& result, const ValueFormat& fmt, const void* addr)
{
    SWAG_TRY
    {
        appendLiteralValueProtected(context, result, fmt, addr);
    }
    SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
    {
        result = "<error>";
    }
}

static void printMemory(ByteCodeRunContext* context, const Utf8& arg)
{
    vector<Utf8> cmds;
    Utf8::tokenize(arg, ' ', cmds);

    ValueFormat fmt;
    int         startIdx = 0;
    if (cmds.size() && getValueFormat(cmds[0], fmt))
        startIdx++;

    // Count
    int count = 64;
    if (startIdx < cmds.size() && cmds[startIdx].length() > 1 && cmds[startIdx][0] == '@' && isNumber(cmds[startIdx] + 1) && cmds.size() != 1)
    {
        count = atoi(cmds[startIdx] + 1);
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
        g_Log.printColor("invalid 'x' expression\n", LogColor::Red);
        return;
    }

    uint64_t       addrVal = 0;
    EvaluateResult res;
    if (!evalExpression(context, expr, res))
        return;
    if (!res.addr)
    {
        res.addr = res.value->reg.pointer;
        addrVal  = (uint64_t) res.addr;
    }
    else if (res.type->isPointer())
        addrVal = *(uint64_t*) res.addr;
    else
        addrVal = (uint64_t) res.addr;

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
            appendLiteralValue(context, str, fmt, addrB);
            g_Log.print(str);
            addrB += fmt.bitCount / 8;
        }

        addrB = addrLine;
        if (fmt.bitCount == 8)
        {
            // Align to the right
            for (int i = 0; i < perLine - min(count, perLine); i++)
            {
                if (fmt.isHexa)
                    g_Log.print("   ");
                else if (!fmt.isSigned)
                    g_Log.print("    ");
                else
                    g_Log.print("     ");
            }

            // Print as 'char'
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
        addrB += min(count, perLine) * (fmt.bitCount / 8);
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

    if (ipNode && ipNode->ownerFct)
    {
        ipNode->ownerFct->typeInfo->computeScopedName();
        g_Log.messageHeaderDot("function", ipNode->ownerFct->typeInfo->scopedName, LogColor::Gray, LogColor::Gray, " ");
    }

    g_Log.messageHeaderDot("bytecode name", bc->name.c_str(), LogColor::Gray, LogColor::Gray, " ");
    g_Log.messageHeaderDot("bytecode type", bc->getCallType()->getDisplayNameC(), LogColor::Gray, LogColor::Gray, " ");

    if (bc->sourceFile && bc->node)
        g_Log.messageHeaderDot("bytecode location", Fmt("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");
    else if (bc->sourceFile)
        g_Log.messageHeaderDot("bytecode source file", bc->sourceFile->path, LogColor::Gray, LogColor::Gray, " ");

    if (ipNode && ipNode->sourceFile)
        g_Log.messageHeaderDot("instruction location", Fmt("%s:%u:%u", ipNode->sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1), LogColor::Gray, LogColor::Gray, " ");

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

static void computeDebugContext(ByteCodeRunContext* context)
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

static void appendValue(Utf8& str, const EvaluateResult& res, int indent);
static void appendValueProtected(Utf8& str, const EvaluateResult& res, int indent = 0)
{
    auto typeInfo = res.type;
    auto addr     = res.addr;

    if (!addr && res.value)
        addr = &res.value->reg;

    if (typeInfo->isEnum())
    {
        Register reg;
        auto     ptr = ((uint8_t**) addr)[0];
        reg.pointer  = ptr;
        str += Ast::enumToString(typeInfo, res.value ? res.value->text : Utf8{}, reg, false);
        return;
    }

    if (typeInfo->isPointerToTypeInfo())
    {
        auto ptr = ((ConcreteTypeInfo**) addr)[0];
        if (!ptr)
            str += "null";
        else
        {
            Utf8 str1;
            str1.setView((const char*) ptr->name.buffer, (int) ptr->name.count);
            str += str1;
        }

        return;
    }

    if (typeInfo->isPointer() || typeInfo->isLambdaClosure())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
            str += Fmt("0x%016llx", ptr);
        return;
    }

    if (typeInfo->isStruct())
    {
        if (res.value)
            addr = res.value->reg.pointer;
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        str += "\n";
        for (auto p : typeStruct->fields)
        {
            for (int i = 0; i < indent + 1; i++)
                str += "   ";
            str += p->namedParam.c_str();
            str += ": ";
            str += p->typeInfo->getDisplayName().c_str();
            str += " = ";
            EvaluateResult res1;
            res1.type = p->typeInfo;
            res1.addr = ((uint8_t*) addr) + p->offset;
            appendValue(str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->isArray())
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
            appendValue(str, res1, indent + 1);
            if (str.back() != '\n')
                str += "\n";
        }

        return;
    }

    if (typeInfo->isSlice())
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptr       = ((uint8_t**) addr)[0];
        auto count     = ((uint64_t*) addr)[1];
        if (ptr == nullptr)
            str += "null";
        else
        {
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
                appendValue(str, res1, indent + 1);
                if (str.back() != '\n')
                    str += "\n";
            }
        }

        return;
    }

    if (typeInfo->isInterface())
    {
        auto ptr = ((uint8_t**) addr)[0];
        if (ptr == nullptr)
            str += "null";
        else
        {
            str += Fmt("(0x%016llx ", ptr);
            str += Fmt("0x%016llx)", ((void**) addr)[1]);
        }
        return;
    }

    if (typeInfo->isNative())
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

            if (!ptr)
                str += "null";
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

    g_Log.print("<return>                      'step', runs to the next line or instruction (depends on 'bcmode')\n");
    g_Log.print("<shift+return>                'next', runs to the next line or instruction (depends on 'bcmode')\n");
    g_Log.print("<tab>                         contextual completion of the current word\n");
    g_Log.eol();

    g_Log.print("s(tep)                        runs to the next line\n");
    g_Log.print("n(ext)                        like s, but does not step into functions or inlined code\n");
    g_Log.print("f(inish)                      runs until the current function is done\n");
    g_Log.print("c(ont(inue))                  runs until another breakpoint is reached\n");
    g_Log.print("un(til) <num>                 runs to the given line or instruction (depends on 'bcmode')\n");
    g_Log.print("j(ump)  <num>                 jump to the given line or instruction (depends on 'bcmode')\n");
    g_Log.eol();

    g_Log.print("l(ist) [num]                  print the current source code line and [num] lines around\n");
    g_Log.print("ll [name]                     print the current function (or function [name]) source code\n");
    g_Log.eol();

    g_Log.print("e(xec(ute)) <stmt>            execute the Swag code statement <stmt> in the current context\n");
    g_Log.print("$<expr|stmt>                  execute the Swag code expression <expr> or statement <stmt> in the current context\n");
    g_Log.print("<expr|stmt>                   execute the Swag code expression <expr> or statement <stmt> in the current context (if not a valid debugger command)\n");
    g_Log.eol();

    g_Log.print("p(rint) [@format] <expr>      print the value of the Swag code expression <expr> in the current context (format is the same as 'x' command)\n");
    g_Log.print("info locals                   print all current local variables\n");
    g_Log.print("info args                     print all current function arguments\n");
    g_Log.print("info func [filter]            print all functions which contains [filter] in their names\n");
    g_Log.print("info modules                  print all modules\n");
    g_Log.print("info regs [@format]           print all registers (format is the same as 'x' command)\n");
    g_Log.print("(w)here                       print contextual informations\n");
    g_Log.eol();

    g_Log.print("b(reak)                       print all breakpoints\n");
    g_Log.print("b(reak) func <name>           add breakpoint when entering function <name> in the current file\n");
    g_Log.print("b(reak) <line>                add breakpoint in the current source file at <line>\n");
    g_Log.print("b(reak) <file>:<line>         add breakpoint in <file> at <line>\n");
    g_Log.print("b(reak) clear                 remove all breakpoints\n");
    g_Log.print("b(reak) clear <num>           remove breakpoint <num>\n");
    g_Log.print("b(reak) enable <num>          enable breakpoint <num>\n");
    g_Log.print("b(reak) disable <num>         disable breakpoint <num>\n");
    g_Log.print("tb(reak) ...                  same as 'b(reak)' except that the breakpoint will be automatically removed on hit\n");
    g_Log.eol();

    g_Log.print("stack                         print callstack\n");
    g_Log.print("u(p)   [num]                  move stack frame <num> level up\n");
    g_Log.print("d(own) [num]                  move stack frame <num> level down\n");
    g_Log.print("frame  <num>                  move stack frame to level <num>\n");
    g_Log.eol();

    g_Log.print("i [num]                       print the current bytecode instruction and <num> instructions around\n");
    g_Log.print("bc                            print the current function bytecode\n");
    g_Log.print("x [@format] [@num] <expr>     print memory (format = s8|s16|s32|s64|u8|u16|u32|u64|x8|x16|x32|x64|f32|f64)\n");
    g_Log.eol();

    g_Log.print("bcmode                        swap between bytecode mode and source mode\n");
    g_Log.eol();

    g_Log.print("?                             print this list of commands\n");
    g_Log.print("q(uit)                        quit the compiler\n");
    g_Log.eol();
}

static void printBreakpoints(ByteCodeRunContext* context)
{
    if (context->debugBreakpoints.empty())
    {
        g_Log.printColor("no breakpoint\n", LogColor::Red);
        return;
    }

    g_Log.setColor(LogColor::Gray);
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
        if (bkp.autoRemove)
            g_Log.print(" (ONE SHOT)");
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
            if (file && location && file->name == bkp.name && location->line == bkp.line)
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
        if (b.type == bkp.type && b.name == bkp.name && b.line == bkp.line && b.autoRemove == bkp.autoRemove)
        {
            g_Log.printColor("breakpoint already exists\n", LogColor::Red);
            return false;
        }
    }

    context->debugBreakpoints.push_back(bkp);
    return true;
}

static void printContextInstruction(ByteCodeRunContext* context)
{
    SWAG_ASSERT(context->debugCxtBc);
    SWAG_ASSERT(context->debugCxtIp);

    SourceFile*     file;
    SourceLocation* location;
    ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &file, &location);

    // Print file
    if (file && file != context->debugStepLastFile)
    {
        g_Log.printColor(Fmt("=> file %s\n", file->name.c_str()), LogColor::Yellow);
    }

    // Print function name
    AstNode* newFunc   = nullptr;
    bool     isInlined = false;
    if (context->debugCxtIp->node && context->debugCxtIp->node->ownerInline)
    {
        isInlined = true;
        newFunc   = context->debugCxtIp->node->ownerInline->func;
    }
    else if (context->debugCxtIp->node)
    {
        newFunc = context->debugCxtIp->node->ownerFct;
    }

    if (newFunc && newFunc != context->debugStepLastFunc)
    {
        if (isInlined)
            g_Log.printColor(Fmt("=> inlined function %s\n", newFunc->getScopedName().c_str()), LogColor::Yellow);
        else
            g_Log.printColor(Fmt("=> function %s\n", newFunc->getScopedName().c_str()), LogColor::Yellow);
    }

    // Print source line
    if (location && (context->debugStepLastFile != file || context->debugStepLastLocation && context->debugStepLastLocation->line != location->line))
    {
        if (file)
        {
            g_Log.printColor(Fmt("%08u -> ", location->line), LogColor::Yellow);
            auto str1 = file->getLine(location->line);
            g_Log.printColor(str1, LogColor::Yellow);
            g_Log.eol();
        }
    }

    // Print instruction
    if (context->debugBcMode)
        printInstruction(context, context->debugCxtBc, context->debugCxtIp);

    context->debugLastBc           = context->debugCxtBc;
    context->debugStepLastFile     = file;
    context->debugStepLastLocation = location;
    context->debugStepLastFunc     = newFunc;
}

static Utf8 completion(ByteCodeRunContext* context, const Utf8& line, Utf8& toComplete)
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

static Utf8 getCommandLine(ByteCodeRunContext* context, bool& ctrl, bool& shift)
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

bool ByteCodeRun::debugger(ByteCodeRunContext* context)
{
    auto ip                              = context->ip;
    auto module                          = context->bc->sourceFile->module;
    g_ByteCodeStackTrace->currentContext = context;

    if (context->debugEntry)
    {
        static bool firstOne = true;

        g_Log.lock();
        g_Log.setColor(LogColor::Gray);

        if (firstOne)
        {
            firstOne = false;
            g_Log.eol();
            g_Log.printColor("#############################################\n", LogColor::Magenta);
            g_Log.printColor("entering bytecode debugger, type '?' for help\n", LogColor::Magenta);
            g_Log.printColor("---------------------------------------------\n", LogColor::Magenta);
            g_Log.printColor(Fmt("build configuration            = '%s'\n", g_CommandLine->buildCfg.c_str()), LogColor::Magenta);
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

        g_Log.unlock();
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
        g_Log.lock();

        computeDebugContext(context);
        printContextInstruction(context);

        while (true)
        {
            g_Log.setColor(LogColor::Cyan);
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
                else if (cmds[i].length() > 2 && cmds[i][0] == '$' && cmds[i][1] == 'r' && isNumber(cmds[i] + 2))
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
                printHelp();
                continue;
            }

            // Print expression
            /////////////////////////////////////////
            if (cmd == "p" || cmd == "print")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                ValueFormat fmt;
                fmt.isHexa     = true;
                fmt.bitCount   = 64;
                bool hasFormat = false;
                if (cmds.size() > 1)
                {
                    if (getValueFormat(cmds[1], fmt))
                    {
                        hasFormat = true;
                        cmdExpr.clear();
                        for (int i = 2; i < cmds.size(); i++)
                            cmdExpr += cmds[i] + " ";
                        cmdExpr.trim();
                        if (cmdExpr.empty())
                            goto evalDefault;
                    }
                }

                EvaluateResult res;
                if (evalExpression(context, cmdExpr, res))
                {
                    if (!res.type->isNative(NativeTypeKind::Void))
                    {
                        Utf8 str;
                        if (hasFormat)
                        {
                            if (!res.addr && res.value)
                                res.addr = &res.value->reg;
                            appendLiteralValue(context, str, fmt, res.addr);
                        }
                        else
                        {
                            str = Fmt("%s: ", res.type->getDisplayNameC());
                            appendValue(str, res);
                        }

                        g_Log.printColor(str);
                        str.trim();
                        if (str.back() != '\n')
                            g_Log.eol();
                    }
                }

                continue;
            }

            // Execute expression
            /////////////////////////////////////////
            if (cmd == "e" || cmd == "exec" || cmd == "execute")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                EvaluateResult res;
                evalDynExpression(context, cmdExpr, res, CompilerAstKind::EmbeddedInstruction);
                continue;
            }

            // Info functions
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "func")
            {
                if (cmds.size() > 3)
                    goto evalDefault;
                auto filter = cmds.size() == 3 ? cmds[2] : Utf8("");
                g_Log.setColor(LogColor::Gray);
                for (auto m : g_Workspace->modules)
                {
                    for (auto bc : m->byteCodeFunc)
                    {
                        if (filter.empty() || bc->name.find(filter) != -1)
                        {
                            g_Log.print(Fmt("%s ", bc->name.c_str()));
                            SourceFile*     bcFile;
                            SourceLocation* bcLocation;
                            ByteCode::getLocation(bc, bc->out, &bcFile, &bcLocation);
                            if (bcFile)
                                g_Log.print(Fmt("%s", bcFile->name.c_str()));
                            if (bcLocation)
                                g_Log.print(Fmt(":%d", bcLocation->line));
                            g_Log.eol();
                        }
                    }
                }

                continue;
            }

            // Info modules
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() == 2 && cmds[1] == "modules")
            {
                g_Log.setColor(LogColor::Gray);
                for (auto m : g_Workspace->modules)
                {
                    g_Log.print(Fmt("%s\n", m->name.c_str()));
                }

                continue;
            }

            // Info locals
            /////////////////////////////////////////
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
                        // Generated
                        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
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
            /////////////////////////////////////////
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
            /////////////////////////////////////////
            if (cmd == "x")
            {
                if (cmds.size() < 2)
                    goto evalDefault;

                printMemory(context, cmdExpr);
                continue;
            }

            // Print stack
            /////////////////////////////////////////
            if (cmd == "stack")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                g_ByteCodeStackTrace->currentContext = context;
                g_ByteCodeStackTrace->log();
                continue;
            }

            // Set stack frame
            /////////////////////////////////////////
            if (cmd == "frame")
            {
                if (cmds.size() == 1)
                    goto evalDefault;
                if (cmds.size() > 2)
                    goto evalDefault;

                if (!isNumber(cmds[1]))
                {
                    g_Log.printColor("invalid 'frame' number\n", LogColor::Red);
                    continue;
                }

                uint32_t off      = atoi(cmds[1].c_str());
                uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
                off               = min(off, maxLevel);

                auto oldIndex                  = context->debugStackFrameOffset;
                context->debugStackFrameOffset = maxLevel - off;
                computeDebugContext(context);
                if (!context->debugCxtIp)
                {
                    context->debugStackFrameOffset = oldIndex;
                    computeDebugContext(context);
                    g_Log.printColor("this frame is external; you cannot go there\n", LogColor::Red);
                    continue;
                }

                printContextInstruction(context);
                continue;
            }

            // Stack frame up
            /////////////////////////////////////////
            if (cmd == "u" || cmd == "up")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
                if (context->debugStackFrameOffset == maxLevel)
                    g_Log.printColor("initial frame selected; you cannot go up\n", LogColor::Red);
                else
                {
                    auto oldIndex = context->debugStackFrameOffset;
                    context->debugStackFrameOffset += off;
                    computeDebugContext(context);
                    if (!context->debugCxtIp)
                    {
                        context->debugStackFrameOffset = oldIndex;
                        computeDebugContext(context);
                        g_Log.printColor("the up frame is external; you cannot go there\n", LogColor::Red);
                        continue;
                    }

                    printContextInstruction(context);
                }

                continue;
            }

            // Stack frame down
            /////////////////////////////////////////
            if (cmd == "d" || cmd == "down")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                uint32_t off = 1;
                if (cmds.size() == 2)
                    off = atoi(cmds[1].c_str());
                if (context->debugStackFrameOffset == 0)
                    g_Log.printColor("bottom(innermost) frame selected; you cannot go down\n", LogColor::Red);
                else
                {
                    auto oldIndex = context->debugStackFrameOffset;
                    context->debugStackFrameOffset -= min(context->debugStackFrameOffset, off);
                    computeDebugContext(context);
                    if (!context->debugCxtIp)
                    {
                        context->debugStackFrameOffset = oldIndex;
                        computeDebugContext(context);
                        g_Log.printColor("the down frame is external; you cannot go there\n", LogColor::Red);
                        continue;
                    }

                    printContextInstruction(context);
                }

                continue;
            }

            // Exit
            /////////////////////////////////////////
            if (cmd == "q" || cmd == "quit")
            {
                if (cmds.size() != 1)
                    goto evalDefault;
                g_Log.unlock();
                return false;
            }

            // Print current instruction
            /////////////////////////////////////////
            if (cmd == "i")
            {
                int regN = 1;

                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() != 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                if (cmds.size() == 2)
                    regN = atoi(cmds[1].c_str());

                printInstruction(context, context->debugCxtBc, context->debugCxtIp, regN);
                continue;
            }

            // Print context
            /////////////////////////////////////////
            if (cmd == "w" || cmd == "where")
            {
                if (cmds.size() != 1)
                    goto evalDefault;
                printContext(context);
                continue;
            }

            // Print all registers
            /////////////////////////////////////////
            if (cmd == "info" && cmds.size() >= 2 && cmds[1] == "regs")
            {
                if (cmds.size() > 3)
                    goto evalDefault;

                ValueFormat fmt;
                fmt.isHexa   = true;
                fmt.bitCount = 64;
                if (cmds.size() > 2)
                {
                    if (!getValueFormat(cmds[2], fmt))
                        goto evalDefault;
                }

                g_Log.setColor(LogColor::Gray);
                for (int i = 0; i < context->getRegCount(context->debugCxtRc); i++)
                {
                    auto& regP = context->getRegBuffer(context->debugCxtRc)[i];
                    Utf8  str;
                    appendLiteralValue(context, str, fmt, &regP);
                    str.trim();
                    g_Log.print(Fmt("$r%d = ", i));
                    g_Log.print(str);
                    g_Log.eol();
                }

                g_Log.print(Fmt("$sp = %016llx\n", context->sp));
                g_Log.print(Fmt("$bp = %016llx\n", context->bp));
                continue;
            }

            // Bytecode dump
            /////////////////////////////////////////
            if (cmd == "bc")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                g_Log.unlock();
                context->debugCxtBc->print(context->debugCxtIp);
                g_Log.lock();
                continue;
            }

            // Bc mode
            /////////////////////////////////////////
            if (cmd == "bcmode")
            {
                if (cmds.size() != 1)
                    goto evalDefault;

                context->debugBcMode = !context->debugBcMode;
                if (context->debugBcMode)
                    g_Log.printColor("bytecode mode\n", LogColor::White);
                else
                    g_Log.printColor("source code mode\n", LogColor::White);
                continue;
            }

            // Function code
            /////////////////////////////////////////
            if (cmd == "l" || cmd == "list" || cmd == "ll")
            {
                if (cmd == "l" || cmd == "list")
                {
                    if (cmds.size() > 2)
                        goto evalDefault;
                    if (cmds.size() > 1 && !isNumber(cmds[1]))
                        goto evalDefault;
                }

                auto toLogBc = context->debugCxtBc;
                auto toLogIp = context->debugCxtIp;

                if (cmd == "ll" && cmds.size() > 1)
                {
                    if (cmds.size() > 2)
                        goto evalDefault;
                    auto name = cmds[1];
                    auto bc   = g_Workspace->findBc(name);
                    if (bc)
                    {
                        toLogBc = bc;
                        toLogIp = bc->out;
                    }
                    else
                    {
                        g_Log.printColor(Fmt("cannot find function '%s'\n", name.c_str()), LogColor::Red);
                        continue;
                    }
                }

                if (toLogBc->node && toLogBc->node->kind == AstNodeKind::FuncDecl && toLogBc->node->sourceFile)
                {
                    SourceFile*     curFile;
                    SourceLocation* curLocation;
                    ByteCode::getLocation(toLogBc, toLogIp, &curFile, &curLocation);

                    auto funcNode = CastAst<AstFuncDecl>(toLogBc->node, AstNodeKind::FuncDecl);

                    uint32_t startLine = toLogBc->node->token.startLocation.line;
                    uint32_t endLine   = funcNode->content->token.endLocation.line;

                    if (cmd == "l")
                    {
                        uint32_t offset = 3;
                        if (cmds.size() == 2)
                            offset = atoi(cmds[1]);
                        if (offset > curLocation->line)
                            startLine = 0;
                        else
                            startLine = curLocation->line - offset;
                        endLine = curLocation->line + offset;
                    }

                    vector<Utf8> lines;
                    bool         eof = false;
                    for (uint32_t l = startLine; l <= endLine && !eof; l++)
                        lines.push_back(toLogBc->node->sourceFile->getLine(l, &eof));

                    g_Log.setColor(LogColor::Yellow);
                    uint32_t lineIdx = 0;
                    for (const auto& l : lines)
                    {
                        g_Log.print(Fmt("%08u ", startLine + lineIdx));
                        if (curLocation->line == startLine + lineIdx)
                            g_Log.print(" -> ");
                        else
                            g_Log.print("    ");
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
            /////////////////////////////////////////
            if (cmd == "b" || cmd == "break" || cmd == "tb" || cmd == "tbreak")
            {
                if (cmds.size() == 1)
                {
                    printBreakpoints(context);
                    continue;
                }

                if (cmds.size() > 3)
                    goto evalDefault;

                bool oneShot = false;
                if (cmd == "tb" || cmd == "tbreak")
                    oneShot = true;

                // At line
                vector<Utf8> fileFunc;
                Utf8::tokenize(cmds[1], ':', fileFunc);
                if (cmds.size() == 2 && ((fileFunc.size() == 1 && isNumber(cmds[1])) || (fileFunc.size() == 2 && isNumber(fileFunc[1]))))
                {
                    SourceFile*     curFile;
                    SourceLocation* curLocation;
                    ByteCode::getLocation(context->debugCxtBc, context->debugCxtIp, &curFile, &curLocation);

                    ByteCodeRunContext::DebugBreakpoint bkp;
                    bkp.name = curFile->name;

                    if (fileFunc.size() == 2)
                    {
                        if (fileFunc[0] != curFile->name && fileFunc[0] + ".swg" != curFile->name && fileFunc[0] + ".swgs" != curFile->name)
                        {
                            auto f = g_Workspace->findFile(fileFunc[0]);
                            if (!f)
                                f = g_Workspace->findFile(fileFunc[0] + ".swg");
                            if (!f)
                                f = g_Workspace->findFile(fileFunc[0] + ".swgs");
                            if (!f)
                            {
                                g_Log.printColor(Fmt("cannot find file '%s'\n", fileFunc[0].c_str()), LogColor::Red);
                                continue;
                            }
                            bkp.name = f->name;
                        }
                    }

                    int lineNo     = fileFunc.size() == 2 ? atoi(fileFunc[1]) : atoi(cmds[1]);
                    bkp.type       = ByteCodeRunContext::DebugBkpType::FileLine;
                    bkp.line       = lineNo;
                    bkp.autoRemove = oneShot;
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("breakpoint #%d, file '%s', line '%d'\n", context->debugBreakpoints.size(), curFile->name.c_str(), lineNo), LogColor::White);
                    continue;
                }

                // Clear all breakpoints
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

                if (cmds.size() == 2)
                    goto evalDefault;

                // Break on function
                if (cmds[1] == "func")
                {
                    ByteCodeRunContext::DebugBreakpoint bkp;
                    bkp.type = ByteCodeRunContext::DebugBkpType::FuncName;
                    auto bc  = g_Workspace->findBc(cmds[2]);
                    if (!bc)
                    {
                        g_Log.printColor(Fmt("cannot find function '%s'", cmds[2].c_str()), LogColor::Red);
                        continue;
                    }

                    bkp.name       = cmds[2];
                    bkp.autoRemove = oneShot;
                    if (addBreakpoint(context, bkp))
                        g_Log.printColor(Fmt("breakpoint #%d entering function '%s'\n", context->debugBreakpoints.size(), bkp.name.c_str()), LogColor::White);
                    continue;
                }

                if (!isNumber(cmds[2]))
                    goto evalDefault;

                // Clear one breakpoint
                if (cmds[1] == "clear")
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

                // Disable one breakpoint
                if (cmds[1] == "disable")
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

                // Enable one breakpoint
                if (cmds[1] == "enable")
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
            /////////////////////////////////////////
            if (cmd.empty())
            {
                if (context->debugBcMode)
                    break;

                context->debugStackFrameOffset = 0;
                if (shift)
                {
                    context->debugStepRC   = context->curRC;
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                }
                else
                {
                    context->debugStepMode = ByteCodeRunContext::DebugStepMode::NextLine;
                }

                break;
            }

            // Step to next line
            /////////////////////////////////////////
            if (cmd == "s" || cmd == "step")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLine;
                break;
            }

            // Step to next line, step out
            /////////////////////////////////////////
            if (cmd == "n" || cmd == "next")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::NextLineStepOut;
                context->debugStepRC           = context->curRC;
                break;
            }

            // Run to the end of the current function
            /////////////////////////////////////////
            if (cmd == "f" || cmd == "finish")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::FinishedFunction;
                context->debugStepRC           = context->curRC - 1;
                break;
            }

            // Continue
            /////////////////////////////////////////
            if (cmd == "c" || cmd == "cont" || cmd == "continue")
            {
                if (cmds.size() > 1)
                    goto evalDefault;

                g_Log.printColor("continue...\n", LogColor::White);
                context->debugStackFrameOffset = 0;
                context->debugStepMode         = ByteCodeRunContext::DebugStepMode::ToNextBreakpoint;
                context->debugOn               = false;
                break;
            }

            // Jump to line/instruction
            /////////////////////////////////////////
            if (cmd == "j" || cmd == "jump")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() > 1 && !isNumber(cmds[1]))
                    goto evalDefault;

                context->debugStackFrameOffset = 0;

                uint32_t to = (uint32_t) atoi(cmds[1]);
                if (to >= context->bc->numInstructions - 1)
                {
                    g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
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
                            g_Log.printColor("cannot reach this 'jump' destination\n", LogColor::Red);
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

                printContextInstruction(context);
                continue;
            }

            // Run until a line/instruction is reached
            /////////////////////////////////////////
            if (cmd == "un" || cmd == "until")
            {
                if (cmds.size() > 2)
                    goto evalDefault;
                if (cmds.size() > 1 && !isNumber(cmds[1]))
                    goto evalDefault;

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

        evalDefault:
            // Default to 'print' / 'execute'
            /////////////////////////////////////////
            line.trim();
            if (line[0] == '$')
                line.remove(0, 1);
            if (line.empty())
                continue;
            EvaluateResult res;
            if (evalExpression(context, line, res, true))
            {
                if (!res.type->isNative(NativeTypeKind::Void))
                {
                    Utf8 str = Fmt("%s: ", res.type->getDisplayNameC());
                    appendValue(str, res);
                    g_Log.printColor(str);
                    if (str.back() != '\n')
                        g_Log.eol();
                }

                continue;
            }

            evalDynExpression(context, line, res, CompilerAstKind::EmbeddedInstruction);
        }

        g_Log.unlock();
    }

    context->debugLastCurRC = context->curRC;
    context->debugLastIp    = ip;
    return true;
}
