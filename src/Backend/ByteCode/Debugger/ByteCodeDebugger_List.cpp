#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Format/FormatAst.h"
#include "Format/FormatConcat.h"
#include "Report/Log.h"
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
    g_ByteCodeDebugger.printInstructionsAround(context, g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, regN);
    g_Log.setStoreMode(false);
    printLong(g_Log.store);
    g_ByteCodeDebugger.bcMode = true;

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInstructionDump(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    const auto toLogBc = g_ByteCodeDebugger.cxtBc;
    const auto toLogIp = g_ByteCodeDebugger.cxtIp;

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

    int num = 3;
    if (arg.split.size() == 2)
        num = arg.split[1].toInt();

    const auto loc = ByteCode::getLocation(g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, true);
    if (!loc.node)
    {
        printCmdError("no source code");
        return BcDbgCommandResult::Error;
    }

    g_ByteCodeDebugger.printSourceLinesAround(context, loc.file, static_cast<int>(loc.location->line), num);
    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdLongList(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    g_ByteCodeDebugger.bcMode = false;

    const auto loc = ByteCode::getLocation(g_ByteCodeDebugger.cxtBc, g_ByteCodeDebugger.cxtIp, true);
    if (!loc.node)
    {
        printCmdError("no source code");
        return BcDbgCommandResult::Error;
    }

    const auto funcNode = loc.node->ownerFct;
    if (!funcNode || !funcNode->content || !funcNode->token.sourceFile)
    {
        printCmdError("no source code");
        return BcDbgCommandResult::Error;
    }

    const int startLine = static_cast<int>(funcNode->token.startLocation.line);
    const int endLine   = static_cast<int>(funcNode->content->token.endLocation.line);
    g_ByteCodeDebugger.printSourceLines(context, loc.file, startLine, endLine);
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
            toPrint.push_back(doSyntaxColor(line, cxt));
    }

    printLong(toPrint);
    return BcDbgCommandResult::Continue;
}
