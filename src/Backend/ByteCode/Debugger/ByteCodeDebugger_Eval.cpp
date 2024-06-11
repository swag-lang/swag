#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Backend/ByteCode/Gen/ByteCodeGenJob.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Backend/Context.h"
#include "Report/Diagnostic.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool ByteCodeDebugger::evalDynExpression(ByteCodeRunContext* context, const Utf8& inExpr, EvaluateResult& res, CompilerAstKind kind, bool silent) const
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

#ifdef SWAG_DEV_MODE
    // ext->bc->print({});
#endif

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

bool ByteCodeDebugger::evalExpression(ByteCodeRunContext* context, const Utf8& expr, EvaluateResult& res, bool silent) const
{
    return evalDynExpression(context, expr, res, CompilerAstKind::Expression, silent);
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

BcDbgCommandResult ByteCodeDebugger::cmdPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of the result, if applicable. Can be one of the following values:\n");
        g_Log.setColor(LogColor::Type);
        g_Log.print("    s8 s16 s32 s64 u8 u16 u32 u64 x8 x16 x32 x64 f32 f64\n");

        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    auto expr = arg.cmdExpr;

    ValueFormat fmt;
    fmt.isHexa     = true;
    fmt.bitCount   = 64;
    bool hasFormat = false;
    if (arg.split.size() > 1)
    {
        if (getValueFormat(arg.split.back(), fmt))
        {
            hasFormat = true;
            expr.remove(expr.length() - arg.split.back().length(), arg.split.back().length());
            expr.trim();
            if (expr.empty())
            {
                printCmdError("expression to evaluate is empty");
                return BcDbgCommandResult::Error;
            }
        }
    }

    EvaluateResult res;

    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    if (res.type->isVoid())
        return BcDbgCommandResult::Continue;

    const auto concrete = res.type->getConcreteAlias();
    Utf8       str;
    Utf8       strRes;

    if (hasFormat)
    {
        if (!concrete->isNativeIntegerOrRune() && !concrete->isNativeFloat())
        {
            printCmdError(form("cannot apply print format to type [[%s]]", concrete->getDisplayNameC()));
            return BcDbgCommandResult::Error;
        }

        if (!res.addr && res.value)
            res.addr = &res.value->reg;

        appendLiteralValue(context, strRes, fmt, res.addr);
    }
    else
    {
        str = form("(%s%s%s) ", Log::colorToVTS(LogColor::Type).c_str(), res.type->getDisplayNameC(), Log::colorToVTS(LogColor::Default).c_str());
        appendTypedValue(context, strRes, res, 0, 0);
    }

    str += strRes;

    // Append to automatic expressions
    if (res.type)
    {
        g_ByteCodeDebugger.evalExpr.push_back(expr);
        if (res.type->isNativeIntegerOrRune() || res.type->isPointer() || res.type->isNativeFloat() || res.type->isBool())
            g_ByteCodeDebugger.evalExprResult.push_back(strRes);
        else
            g_ByteCodeDebugger.evalExprResult.push_back(expr);
        g_Log.setColor(LogColor::Gray);
        g_Log.print(form("$%d = ", g_ByteCodeDebugger.evalExpr.size() - 1));
    }

    g_Log.setColor(LogColor::Name);
    g_Log.print(expr);
    g_Log.setColor(LogColor::Default);
    g_Log.write(" = ");

    printLong(str);
    return BcDbgCommandResult::Continue;
}
