#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

BcDbgCommandResult ByteCodeDebugger::cmdShowFunctions(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    Vector<Utf8> all;
    g_Log.setColor(LogColor::Gray);

    for (const auto m : g_Workspace->modules)
    {
        for (const auto bc : m->byteCodeFunc)
        {
            if (!bc->out)
                continue;
            all.push_back(bc->getPrintRefName());
        }
    }

    std::ranges::sort(all, [](const Utf8& a, const Utf8& b) { return strcmp(a, b) < 0; });
    printLong(all, &filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowModules(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    Vector<Utf8> all;
    for (const auto m : g_Workspace->modules)
        all.push_back(m->name);

    std::ranges::sort(all, [](const Utf8& a, const Utf8& b) { return strcmp(a, b) < 0; });
    printLong(all);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowFiles(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    SetUtf8 set;
    for (const auto m : g_Workspace->modules)
    {
        for (const auto f : m->files)
        {
            set.insert(f->path);
        }
    }

    Vector<Utf8> all;
    for (const auto f : set)
        all.push_back(f);

    std::ranges::sort(all, [](const Utf8& a, const Utf8& b) { return strcmp(a, b) < 0; });
    printLong(all, &filter, LogColor::Location);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowTypes(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    SetUtf8 set;
    for (const auto m : g_Workspace->modules)
    {
        const auto& map = m->typeGen.getMapPerSeg(&m->constantSegment);
        for (const auto& exp : map.exportedTypes)
        {
            const auto typeInfo = exp.second.realType->getStructOrPointedStruct();
            if (!typeInfo)
                continue;
            Utf8 value;
            value += Log::colorToVTS(LogColor::Name);
            value += typeInfo->declNode->getScopedName();
            value += " ";
            value += Log::colorToVTS(LogColor::Location);
            value += typeInfo->declNode->token.sourceFile->path;
            value += ":";
            value += form("%d", typeInfo->declNode->token.startLocation.line + 1);
            set.insert(value);
        }
    }

    Vector<Utf8> all;
    for (const auto& f : set)
        all.push_back(f);

    std::ranges::sort(all, [](const Utf8& a, const Utf8& b) { return strcmp(a, b) < 0; });
    printLong(all, &filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowValues(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    Vector<Utf8> all;
    for (uint32_t idx = g_ByteCodeDebugger.evalExpr.size() - 1; idx != UINT32_MAX; idx--)
    {
        const auto expr = g_ByteCodeDebugger.evalExpr[idx];
        const auto val  = g_ByteCodeDebugger.evalExprResult[idx];
        Utf8       str  = form("$%d = %s%s%s = %s", idx, Log::colorToVTS(LogColor::Name).c_str(), expr.c_str(), Log::colorToVTS(LogColor::Default).c_str(), val.c_str());
        all.push_back(str);
    }

    printLong(all, nullptr, LogColor::Gray);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowExpressions(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (g_ByteCodeDebugger.evalExpr.empty())
    {
        printCmdError("no expressions");
        return BcDbgCommandResult::Continue;
    }

    Utf8 result;
    for (uint32_t i = g_ByteCodeDebugger.evalExpr.size() - 1; i != UINT32_MAX; i--)
    {
        result += form("$%d = ", i);
        result += Log::colorToVTS(LogColor::Name);
        result += g_ByteCodeDebugger.evalExpr[i];
        result += Log::colorToVTS(LogColor::Gray);
        result += " = ";
        result += g_ByteCodeDebugger.evalExprResult[i];
        result += "\n";
    }

    printLong(result);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    if (g_ByteCodeDebugger.cxtBc->localVars.empty())
    {
        printCmdError("no local variables");
        return BcDbgCommandResult::Continue;
    }

    VectorNative<AstNode*> nodes;
    VectorNative<uint8_t*> addrs;
    for (const auto n : g_ByteCodeDebugger.cxtBc->localVars)
    {
        const auto over = n->resolvedSymbolOverload();
        if (!over)
            continue;
        nodes.push_back(n);
        addrs.push_back(nullptr);
    }

    if (nodes.empty())
    {
        printCmdError("no local variables");
        return BcDbgCommandResult::Continue;
    }

    const auto result = getPrintSymbols(context, nodes, addrs);
    printLong(result, &filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    const auto funcDecl = castAst<AstFuncDecl>(g_ByteCodeDebugger.cxtBc->node, AstNodeKind::FuncDecl);
    if (!funcDecl->parameters || funcDecl->parameters->children.empty())
    {
        printCmdError("no arguments");
        return BcDbgCommandResult::Continue;
    }

    VectorNative<AstNode*> nodes;
    VectorNative<uint8_t*> addrs;
    for (const auto n : funcDecl->parameters->children)
    {
        const auto over = n->resolvedSymbolOverload();
        if (!over)
            continue;
        nodes.push_back(n);
        addrs.push_back(g_ByteCodeDebugger.cxtBp + over->computedValue.storageOffset + g_ByteCodeDebugger.cxtBc->stackSize);
    }

    if (nodes.empty())
    {
        printCmdError("no arguments");
        return BcDbgCommandResult::Continue;
    }

    const auto result = getPrintSymbols(context, nodes, addrs);
    printLong(result, &filter);

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdShowGlobals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    VectorNative<AstNode*> nodes;
    VectorNative<uint8_t*> addrs;

    const auto m = context->jc.sourceFile->module;
    for (const auto n : m->globalVarsBss)
    {
        const auto over = n->resolvedSymbolOverload();
        if (!over)
            continue;
        nodes.push_back(n);
        addrs.push_back(m->bssSegment.address(over->computedValue.storageOffset));
    }

    for (const auto n : m->globalVarsMutable)
    {
        const auto over = n->resolvedSymbolOverload();
        if (!over)
            continue;
        nodes.push_back(n);
        addrs.push_back(m->mutableSegment.address(over->computedValue.storageOffset));
    }

    if (nodes.empty())
    {
        printCmdError("no global variables");
        return BcDbgCommandResult::Continue;
    }

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");
    const auto result = getPrintSymbols(context, nodes, addrs);
    printLong(result, &filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of each register. Can be one of the following:\n");
        g_Log.setColor(LogColor::Type);
        g_Log.print("    s8 s16 s32 s64 u8 u16 u32 u64 x8 x16 x32 x64 f32 f64\n");
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    ValueFormat fmt;
    fmt.isHexa   = true;
    fmt.bitCount = 64;
    if (arg.split.size() > 2)
    {
        if (!getValueFormat(arg.split.back(), fmt))
        {
            printCmdError(form("invalid format [[%s]]", arg.split.back().c_str()));
            return BcDbgCommandResult::Error;
        }
    }

    g_Log.setColor(LogColor::Gray);
    for (uint32_t i = 0; i < context->getRegCount(g_ByteCodeDebugger.cxtRc); i++)
    {
        auto& regP = context->getRegBuffer(g_ByteCodeDebugger.cxtRc)[i];
        Utf8  str;
        appendLiteralValue(context, str, fmt, &regP);
        str.trim();
        g_Log.print(form("%s$r%d%s = ", Log::colorToVTS(LogColor::Name).c_str(), i, Log::colorToVTS(LogColor::Default).c_str()));
        g_Log.print(str);
        g_Log.writeEol();
    }

    g_Log.print(form("%s$sp%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->sp));
    g_Log.print(form("%s$bp%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->bp));
    g_Log.print(form("%s$rr0%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->registersRR[0]));
    g_Log.print(form("%s$rr1%s = 0x%016llx\n", Log::colorToVTS(LogColor::Name).c_str(), Log::colorToVTS(LogColor::Default).c_str(), context->registersRR[1]));

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShow(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    if (arg.split[1] == "locals" || arg.split[1] == "loc")
        return cmdShowLocals(context, arg);
    if (arg.split[1] == "arguments" || arg.split[1] == "args")
        return cmdShowArgs(context, arg);
    if (arg.split[1] == "registers" || arg.split[1] == "regs")
        return cmdShowRegs(context, arg);
    if (arg.split[1] == "functions" || arg.split[1] == "funcs")
        return cmdShowFunctions(context, arg);
    if (arg.split[1] == "globals")
        return cmdShowGlobals(context, arg);
    if (arg.split[1] == "expressions" || arg.split[1] == "exprs")
        return cmdShowExpressions(context, arg);
    if (arg.split[1] == "modules")
        return cmdShowModules(context, arg);
    if (arg.split[1] == "files")
        return cmdShowFiles(context, arg);
    if (arg.split[1] == "values" || arg.split[1] == "vals")
        return cmdShowValues(context, arg);
    if (arg.split[1] == "types")
        return cmdShowTypes(context, arg);

    printCmdError(form("invalid [[show]] command [[%s]]", arg.split[1].c_str()));
    return BcDbgCommandResult::Error;
}
