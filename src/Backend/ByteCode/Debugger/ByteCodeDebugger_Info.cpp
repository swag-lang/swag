#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Utf8 ByteCodeDebugger::getPrintSymbols(ByteCodeRunContext* context, const Utf8& filter, uint8_t* baseAddr, const VectorNative<AstNode*>& nodes, const VectorNative<uint8_t*>& addrs)
{
    Utf8 result;
    for (uint32_t i = 0; i < nodes.size(); i++)
    {
        const auto n    = nodes[i];
        const auto over = n->resolvedSymbolOverload();
        const Utf8 name = over->symbol->getFullName();
        if (!testNameFilter(name, filter))
            continue;
        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
            continue;

        if (over->typeInfo->isNative() || over->typeInfo->isPointer())
        {
            appendTypedValue(context, filter, n, baseAddr, addrs[i], result);
        }
        else
        {
            const Utf8 str = form("(%s%s%s) %s%s%s",
                                  Log::colorToVTS(LogColor::Type).c_str(),
                                  over->typeInfo->getDisplayNameC(),
                                  Log::colorToVTS(LogColor::Default).c_str(),
                                  Log::colorToVTS(LogColor::Name).c_str(),
                                  name.c_str(),
                                  Log::colorToVTS(LogColor::Default).c_str());
            result += str;
            result += " = ...";
            result += "\n";
        }
    }

    return result;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoFunctions(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    Vector<Utf8> all;
    g_Log.setColor(LogColor::Gray);

    uint32_t total = 0;
    for (const auto m : g_Workspace->modules)
    {
        for (const auto bc : m->byteCodeFunc)
        {
            if (!bc->out)
                continue;
            total++;
            if (testNameFilter(bc->getPrintName(), filter) ||
                testNameFilter(bc->getPrintFileName(), filter))
            {
                all.push_back(bc->getPrintRefName());
            }
        }
    }

    if (all.empty())
    {
        printCmdError(form("...no match with filter [[%s]] (%d found functions)", filter.c_str(), total));
        return BcDbgCommandResult::Continue;
    }

    std::ranges::sort(all, [](const Utf8& a, const Utf8& b) { return strcmp(a, b) < 0; });
    printLong(all);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoModules(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
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

BcDbgCommandResult ByteCodeDebugger::cmdInfoExpressions(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

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

BcDbgCommandResult ByteCodeDebugger::cmdInfoLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
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

    const auto result = getPrintSymbols(context, filter, g_ByteCodeDebugger.cxtBp, nodes, addrs);
    if (result.empty())
    {
        printCmdError(form("...no match with filter [[%s]] (%d found local variables)", filter.c_str(), nodes.size()));
        return BcDbgCommandResult::Continue;
    }

    printLong(result);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
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

    const auto result = getPrintSymbols(context, filter, g_ByteCodeDebugger.cxtBp, nodes, addrs);
    if (result.empty())
    {
        printCmdError(form("...no match with filter [[%s]] (%d found arguments)", filter.c_str(), nodes.size()));
        return BcDbgCommandResult::Continue;
    }

    printLong(result);
    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdInfoGlobals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
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
    const auto result = getPrintSymbols(context, filter, nullptr, nodes, addrs);
    if (result.empty())
    {
        printCmdError(form("...no match with filter [[%s]] (%d found global variables)", filter.c_str(), nodes.size()));
        return BcDbgCommandResult::Continue;
    }

    printLong(result);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of each register. Can be one of the following values:\n");
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

BcDbgCommandResult ByteCodeDebugger::cmdInfo(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    if (arg.split[1] == "locals" || arg.split[1] == "loc")
        return cmdInfoLocals(context, arg);
    if (arg.split[1] == "arguments" || arg.split[1] == "arg" || arg.split[1] == "args")
        return cmdInfoArgs(context, arg);
    if (arg.split[1] == "registers" || arg.split[1] == "reg" || arg.split[1] == "regs")
        return cmdInfoRegs(context, arg);
    if (arg.split[1] == "functions" || arg.split[1] == "fun" || arg.split[1] == "func" || arg.split[1] == "fn")
        return cmdInfoFunctions(context, arg);
    if (arg.split[1] == "globals" || arg.split[1] == "glo" || arg.split[1] == "globs")
        return cmdInfoGlobals(context, arg);
    if (arg.split[1] == "expressions" || arg.split[1] == "exp" || arg.split[1] == "expr" || arg.split[1] == "exprs")
        return cmdInfoExpressions(context, arg);
    if (arg.split[1] == "modules" || arg.split[1] == "mod")
        return cmdInfoModules(context, arg);

    printCmdError(form("invalid [[info]] command [[%s]]", arg.split[1].c_str()));
    return BcDbgCommandResult::Error;
}
