#include "pch.h"
#include "ByteCode.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Report.h"
#include "ByteCodeDebugger.h"
#include "Parser.h"
#include "Ast.h"

bool ByteCodeDebugger::evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent)
{
    PushSilentError se;

    auto sourceFile = context->debugCxtBc->sourceFile;

    // Syntax
    AstNode parent;
    Ast::constructNode(&parent);
    parent.ownerScope = context->debugCxtIp->node ? context->debugCxtIp->node->ownerScope : nullptr;
    parent.ownerFct   = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
    parent.sourceFile = sourceFile;
    parent.parent     = context->debugCxtIp->node;

    JobContext jobContext;
    Parser     parser;
    parser.setup(&jobContext, sourceFile->module, sourceFile);
    if (!parser.constructEmbeddedAst(expr, &parent, nullptr, kind, false))
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.print("expression syntax error\n", LogColor::Red);
        else
            g_Log.print(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
        return false;
    }

    // Semantic
    auto child              = parent.childs.front();
    auto semanticJob        = Allocator::alloc<SemanticJob>();
    semanticJob->sourceFile = sourceFile;
    semanticJob->module     = sourceFile->module;
    semanticJob->nodes.push_back(child);
    semanticJob->context.forDebugger = true;

    g_ThreadMgr.debuggerMode = true;
    g_ThreadMgr.addJob(semanticJob);
    g_ThreadMgr.waitEndJobsSync();
    g_ThreadMgr.debuggerMode = true;

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.print("cannot solve expression\n", LogColor::Red);
        else
            g_Log.print(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
        return false;
    }

    // This has been evaluated to a constant
    if (child->flags & AST_VALUE_COMPUTED)
    {
        res.type  = child->typeInfo;
        res.value = child->computedValue;
        return true;
    }

    // Gen bytecode for expression
    auto genJob        = Allocator::alloc<ByteCodeGenJob>();
    genJob->sourceFile = sourceFile;
    genJob->module     = sourceFile->module;
    genJob->nodes.push_back(child);
    child->allocateExtension(ExtensionKind::ByteCode);
    child->extByteCode()->bc             = Allocator::alloc<ByteCode>();
    child->extByteCode()->bc->node       = child;
    child->extByteCode()->bc->sourceFile = sourceFile;

    g_ThreadMgr.debuggerMode = true;
    g_ThreadMgr.addJob(genJob);
    g_ThreadMgr.waitEndJobsSync();
    g_ThreadMgr.debuggerMode = false;

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.print("cannot generate expression\n", LogColor::Red);
        else
            g_Log.print(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
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

    child->extByteCode()->bc->release();
    g_RunContext         = &g_RunContextVal;
    g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

    if (!resExec)
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.print("cannot run expression\n", LogColor::Red);
        else
            g_Log.print(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
        return false;
    }

    res.type       = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
    res.storage[0] = runContext.registersRR[0].pointer;
    res.storage[1] = runContext.registersRR[1].pointer;
    if (res.type->isStruct() || res.type->isArray())
        res.addr = res.storage[0];
    else
        res.addr = &res.storage[0];
    return true;
}

bool ByteCodeDebugger::evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent)
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

BcDbgCommandResult ByteCodeDebugger::cmdExecute(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() < 2)
        return BcDbgCommandResult::BadArguments;

    EvaluateResult res;
    evalDynExpression(context, cmdExpr, res, CompilerAstKind::EmbeddedInstruction);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdPrint(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() < 2)
        return BcDbgCommandResult::BadArguments;

    auto expr = cmdExpr;

    ValueFormat fmt;
    fmt.isHexa     = true;
    fmt.bitCount   = 64;
    bool hasFormat = false;
    if (cmds.size() > 1)
    {
        if (getValueFormat(cmds[1], fmt))
        {
            hasFormat = true;
            expr.clear();
            for (size_t i = 2; i < cmds.size(); i++)
                expr += cmds[i] + " ";
            expr.trim();
            if (expr.empty())
                return BcDbgCommandResult::BadArguments;
        }
    }

    EvaluateResult res;
    if (!evalExpression(context, expr, res))
        return BcDbgCommandResult::Continue;
    if (res.type->isVoid())
        return BcDbgCommandResult::Continue;

    auto concrete = TypeManager::concreteType(res.type, CONCRETE_FORCEALIAS);
    Utf8 str;
    if (hasFormat)
    {
        if (!concrete->isNativeIntegerOrRune() && !concrete->isNativeFloat())
        {
            g_Log.print(Fmt("cannot apply print format to type '%s'\n", concrete->getDisplayNameC()), LogColor::Red);
            return BcDbgCommandResult::Continue;
        }

        if (!res.addr && res.value)
            res.addr = &res.value->reg;
        appendLiteralValue(context, str, fmt, res.addr);
    }
    else
    {
        str = Fmt("(%s%s%s) ", COLOR_TYPE, res.type->getDisplayNameC(), COLOR_DEFAULT);
        appendTypedValue(context, str, res, 0);
    }

    g_Log.print(str);
    str.trim();
    if (str.back() != '\n')
        g_Log.eol();

    return BcDbgCommandResult::Continue;
}