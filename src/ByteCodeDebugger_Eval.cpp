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

bool ByteCodeDebugger::evalDynExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, CompilerAstKind kind, bool silent)
{
    PushSilentError se;

    auto sourceFile = context->debugCxtBc->sourceFile;

    // Syntax
    SyntaxJob syntaxJob;
    AstNode   parent;
    syntaxJob.module  = sourceFile->module;
    parent.ownerScope = context->debugCxtIp->node ? context->debugCxtIp->node->ownerScope : nullptr;
    parent.ownerFct   = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
    parent.sourceFile = sourceFile;
    parent.parent     = context->debugCxtIp->node;

    if (!syntaxJob.constructEmbedded(expr, &parent, nullptr, kind, false))
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.printColor("expression syntax error\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
        return false;
    }

    // Semantic
    auto        child = parent.childs.front();
    SemanticJob semanticJob;
    semanticJob.sourceFile = sourceFile;
    semanticJob.module     = sourceFile->module;
    semanticJob.nodes.push_back(child);
    semanticJob.context.forDebugger = true;

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

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.printColor("cannot solve expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
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

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.printColor("cannot generate expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
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
    g_RunContext         = &g_RunContextVal;
    g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

    if (!resExec)
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
            g_Log.printColor("cannot run expression\n", LogColor::Red);
        else
            g_Log.printColor(Fmt("%s\n", g_SilentErrorMsg.c_str()), LogColor::Red);
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

BcDbgCommandResult ByteCodeDebugger::cmdExecute(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() < 2)
        return BcDbgCommandResult::EvalDefault;

    EvaluateResult res;
    evalDynExpression(context, cmdExpr, res, CompilerAstKind::EmbeddedInstruction);
    return BcDbgCommandResult::Continue;
}

void ByteCodeDebugger::evalDefault(ByteCodeRunContext* context, const Utf8& cmd)
{
    auto line = cmd;

    line.trim();
    if (line[0] == '$')
        line.remove(0, 1);
    if (line.empty())
        return;

    EvaluateResult res;
    if (evalExpression(context, line, res, true))
    {
        if (!res.type->isVoid())
        {
            Utf8 str = Fmt("(%s%s%s) ", COLOR_TYPE, res.type->getDisplayNameC(), COLOR_DEFAULT);
            appendTypedValue(context, str, res, 0);
            g_Log.printColor(str);
            if (str.back() != '\n')
                g_Log.eol();
        }
    }
    else
    {
        evalDynExpression(context, line, res, CompilerAstKind::EmbeddedInstruction);
    }
}