#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Format/FormatAst.h"
#include "Format/FormatConcat.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Workspace.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdInstruction(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() != 1 && !Utf8::isNumber(arg.split[1].c_str()))
    {
        printCmdError(form("invalid instruction count [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    int regN = 3;
    if (arg.split.size() == 2)
        regN = arg.split[1].toInt();

    g_Log.setStoreMode(true);
    g_ByteCodeDebugger.printInstructions(context, g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, regN);
    g_Log.setStoreMode(false);
    printLong(g_Log.store);
    g_ByteCodeDebugger.bcMode = true;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstructionDump(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    auto toLogBc = g_ByteCodeDebugger.cxtBc;
    auto toLogIp = g_ByteCodeDebugger.cxtIp;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Error;
        toLogIp = toLogBc->out;
    }

    ByteCodePrintOptions opt;
    opt.curIp           = toLogIp;
    opt.printSourceCode = g_ByteCodeDebugger.printBcCode;

    g_Log.setStoreMode(true);
    toLogBc->print(opt);
    g_Log.setStoreMode(false);
    printLong(g_Log.store);
    g_ByteCodeDebugger.bcMode = true;

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (arg.split.size() > 1 && !Utf8::isNumber(arg.split[1].c_str()))
    {
        printCmdError(form("invalid line count [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto toLogBc        = g_ByteCodeDebugger.cxtBc;
    const auto toLogIp        = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (toLogBc->node && toLogBc->node->is(AstNodeKind::FuncDecl) && toLogBc->node->token.sourceFile)
    {
        uint32_t offset = 3;
        if (arg.split.size() == 2)
            offset = arg.split[1].toInt();

        const auto inl = g_ByteCodeDebugger.lastBreakIp->node->safeOwnerInline();
        if (inl)
        {
            const auto loc = ByteCode::getLocation(toLogBc, toLogIp, true);
            g_ByteCodeDebugger.printSourceLines(context, toLogBc, inl->func->token.sourceFile, loc.location, offset);
        }
        else
        {
            const auto loc = ByteCode::getLocation(toLogBc, toLogIp, false);
            g_ByteCodeDebugger.printSourceLines(context, toLogBc, toLogBc->node->token.sourceFile, loc.location, offset);
        }
    }
    else
        printCmdError("no source code");

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdLongList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    auto toLogBc              = g_ByteCodeDebugger.cxtBc;
    auto toLogIp              = g_ByteCodeDebugger.cxtIp;
    g_ByteCodeDebugger.bcMode = false;

    if (arg.split.size() > 1)
    {
        const auto name = arg.split[1];
        toLogBc         = findCmdBc(name);
        if (!toLogBc)
            return BcDbgCommandResult::Error;
        toLogIp = toLogBc->out;
    }

    if (toLogBc->node && toLogBc->node->is(AstNodeKind::FuncDecl) && toLogBc->node->token.sourceFile)
    {
        const auto funcNode = castAst<AstFuncDecl>(toLogBc->node, AstNodeKind::FuncDecl);
        if (funcNode->content)
        {
            toLogBc->printName();

            const auto inl = g_ByteCodeDebugger.lastBreakIp->node->safeOwnerInline();
            if (inl)
            {
                const auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, true);
                const uint32_t startLine = inl->func->token.startLocation.line;
                const uint32_t endLine   = inl->func->content->token.endLocation.line;
                g_ByteCodeDebugger.printSourceLines(context, toLogBc, inl->func->token.sourceFile, loc.location, startLine, endLine);
            }
            else
            {
                const auto     loc       = ByteCode::getLocation(toLogBc, toLogIp, false);
                const uint32_t startLine = toLogBc->node->token.startLocation.line;
                const uint32_t endLine   = funcNode->content->token.endLocation.line;
                g_ByteCodeDebugger.printSourceLines(context, toLogBc, toLogBc->node->token.sourceFile, loc.location, startLine, endLine);
            }
        }
        else
            printCmdError("no source code");
    }
    else
        printCmdError("no source code");

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdSource(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;
    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalExpression(context, arg.cmdExpr, res, false, false))
        return BcDbgCommandResult::Error;

    if (!res.node || !res.node->resolvedSymbolOverload())
    {
        printCmdError(form("cannot evaluate expression [[%s]]", arg.cmdExpr.c_str()));
        return BcDbgCommandResult::Error;
    }

    const auto   node = res.node->resolvedSymbolOverload()->node;
    Vector<Utf8> toPrint;
    toPrint.push_back(form("%s%s:%d", Log::colorToVTS(LogColor::Location).c_str(), node->token.sourceFile->path.c_str(), node->token.startLocation.line + 1));

    FormatConcat  tmpConcat;
    FormatAst     fmtAst{tmpConcat};
    FormatContext fmtCxt;
    tmpConcat.init(1024);
    if (fmtAst.outputNode(fmtCxt, node))
    {
        const Utf8   result = fmtAst.getUtf8();
        Vector<Utf8> lines;
        Utf8::tokenize(result, '\n', lines);

        SyntaxColorContext cxt;
        cxt.mode = SyntaxColorMode::ForLog;
        for (const auto& line : lines)
            toPrint.push_back(syntaxColor(line, cxt));
    }

    printLong(toPrint);
    return BcDbgCommandResult::Continue;
}
