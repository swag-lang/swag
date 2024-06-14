#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Backend/ByteCode/Gen/ByteCodeGenJob.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Backend/Context.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/SyntaxColor.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool ByteCodeDebugger::evalDynExpression(ByteCodeRunContext* context, const Utf8& inExpr, EvaluateResult& res, CompilerAstKind kind, bool silent, bool execute) const
{
    PushSilentError se;

    g_Log.setColor(LogColor::Default);

    auto expr = inExpr;
    if (!g_ByteCodeDebugger.commandSubstitution(context, expr))
        return false;

    auto sourceFile = cxtBc->sourceFile;

    // Syntax
    AstNode parent;
    Ast::constructNode(&parent);
    parent.ownerScope       = cxtIp->node ? cxtIp->node->ownerScope : nullptr;
    parent.ownerFct         = castAst<AstFuncDecl>(cxtBc->node, AstNodeKind::FuncDecl);
    parent.token.sourceFile = sourceFile;
    parent.parent           = cxtIp->node;
    parent.addAstFlag(AST_DEBUG_NODE);

    if (cxtIp->node->hasOwnerInline())
    {
        parent.allocateExtension(ExtensionKind::Owner);
        parent.extOwner()->ownerInline = cxtIp->node->ownerInline();
    }

    JobContext jobContext;
    Parser     parser;
    parser.setup(&jobContext, sourceFile->module, sourceFile, PARSER_DEFAULT);
    if (!parser.constructEmbeddedAst(expr, &parent, nullptr, kind, false))
    {
        if (silent)
            return false;

        if (g_SilentErrorMsg.empty())
        {
            printCmdError("expression syntax error");
        }
        else
        {
            g_SilentErrorMsg = Diagnostic::oneLiner(g_SilentErrorMsg);
            printCmdError(form("%s", g_SilentErrorMsg.c_str()));
        }

        return false;
    }

    // Semantic
    auto child                       = parent.firstChild();
    auto semanticJob                 = SemanticJob::newJob(nullptr, sourceFile, child, false);
    semanticJob->context.forDebugger = true;
    g_ThreadMgr.debuggerMode         = true;
    g_ThreadMgr.addJob(semanticJob);
    g_ThreadMgr.waitEndJobsSync();
    g_ThreadMgr.debuggerMode = true;
    res.node                 = child;

    if (kind == CompilerAstKind::Expression && execute)
        Semantic::checkIsConcrete(&semanticJob->context, child);

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        g_SilentErrorMsg = Diagnostic::oneLiner(g_SilentErrorMsg);
        printCmdError(form("%s", g_SilentErrorMsg.c_str()));
        return false;
    }

    // This has been evaluated to a constant
    if (child->hasFlagComputedValue())
    {
        res.type  = child->typeInfo;
        res.value = child->computedValue();
        return true;
    }

    if(!execute)
        return true;

    // Gen bytecode for expression
    auto genJob        = Allocator::alloc<ByteCodeGenJob>();
    genJob->sourceFile = sourceFile;
    genJob->module     = sourceFile->module;
    genJob->context.contextFlags |= BCC_FLAG_FOR_DEBUGGER;
    genJob->nodes.push_back(child);
    child->allocateExtension(ExtensionKind::ByteCode);
    auto ext                 = child->extByteCode();
    ext->bc                  = Allocator::alloc<ByteCode>();
    ext->bc->node            = child;
    ext->bc->sourceFile      = sourceFile;
    ext->bc->stackSize       = context->bc->stackSize;
    ext->bc->registerStoreRR = context->bc->registerStoreRR;

    g_ThreadMgr.debuggerMode = true;
    g_ThreadMgr.addJob(genJob);
    g_ThreadMgr.waitEndJobsSync();
    g_ThreadMgr.debuggerMode = false;

    if (g_ByteCodeDebugger.printEvalBc)
        ext->bc->print({});

    if (!g_SilentErrorMsg.empty())
    {
        if (silent)
            return false;
        g_SilentErrorMsg = Diagnostic::oneLiner(g_SilentErrorMsg);
        printCmdError(form("%s", g_SilentErrorMsg.c_str()));
        return false;
    }

    // Execute node
    JobContext         callerContext;
    ByteCodeRunContext runContext;
    ByteCodeStack      stackTrace;
    ExecuteNodeParams  execParams;
    execParams.inheritSp      = cxtSp;
    execParams.inheritSpAlt   = cxtSpAlt;
    execParams.inheritStack   = cxtStack;
    execParams.inheritBp      = cxtBp;
    execParams.forDebugger    = true;
    execParams.inheritRR      = true;
    runContext.debugAccept    = false;
    runContext.sharedStack    = true;
    runContext.registersRR[0] = g_RunContext->registersRR[0];
    runContext.registersRR[1] = g_RunContext->registersRR[1];

    g_RunContext         = &runContext;
    g_ByteCodeStackTrace = &stackTrace;

    auto resExec = sourceFile->module->executeNode(sourceFile, child, &callerContext, &execParams);

    ext->bc->release();
    g_RunContext         = &g_RunContextVal;
    g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

    if (!resExec)
    {
        if (silent)
            return false;
        if (g_SilentErrorMsg.empty())
        {
            printCmdError("cannot run expression");
        }
        else
        {
            g_SilentErrorMsg = Diagnostic::oneLiner(g_SilentErrorMsg);
            printCmdError(form("%s", g_SilentErrorMsg.c_str()));
        }

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

bool ByteCodeDebugger::evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent, bool execute) const
{
    return evalDynExpression(context, expr, res, CompilerAstKind::Expression, silent, execute);
}

BcDbgCommandResult ByteCodeDebugger::cmdExecute(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalDynExpression(context, arg.cmdExpr, res, CompilerAstKind::EmbeddedInstruction))
        return BcDbgCommandResult::Error;

    return BcDbgCommandResult::Continue;
}
