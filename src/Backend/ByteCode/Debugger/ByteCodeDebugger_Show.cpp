#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

BcDbgCommandResult ByteCodeDebugger::cmdShowFunctions(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    VectorNative<ByteCode*> result;
    g_Workspace->getAllByteCodes(result);

    Vector<std::pair<Utf8, Utf8>> all;
    for (const auto bc : result)
    {
        all.push_back({bc->getPrintRefName(), bc->getPrintName()});
    }

    std::ranges::sort(all, [](const std::pair<Utf8, Utf8>& a, const std::pair<Utf8, Utf8>& b) { return strcmp(a.first, b.first) < 0; });
    printLong(all, filter);

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
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }

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

    Vector<std::pair<Utf8, Utf8>> all;
    for (const auto& f : set)
        all.push_back({f, f});

    std::ranges::sort(all, [](const std::pair<Utf8, Utf8>& a, const std::pair<Utf8, Utf8>& b) { return strcmp(a.first, b.first) < 0; });
    printLong(all, filter, LogColor::Location);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowTypes(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    const auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    SetUtf8 set;
    for (const auto m : g_Workspace->modules)
    {
        const auto& map = m->typeGen.getMapPerSeg(&m->constantSegment);
        for (const auto& val : map.exportedTypes | std::views::values)
        {
            const auto typeInfo = val.realType->getStructOrPointedStruct();
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

    Vector<std::pair<Utf8, Utf8>> all;
    for (const auto& f : set)
        all.push_back({f, f});

    std::ranges::sort(all, [](const std::pair<Utf8, Utf8>& a, const std::pair<Utf8, Utf8>& b) { return strcmp(a.first, b.first) < 0; });
    printLong(all, filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowValues(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    Vector<std::pair<Utf8, Utf8>> all;
    for (uint32_t idx = g_ByteCodeDebugger.evalExpr.size() - 1; idx != UINT32_MAX; idx--)
    {
        const auto expr = g_ByteCodeDebugger.evalExpr[idx];
        const auto val  = g_ByteCodeDebugger.evalExprResult[idx];
        Utf8       str  = form("$%d = %s%s%s = %s", idx, Log::colorToVTS(LogColor::Name).cstr(), expr.cstr(), Log::colorToVTS(LogColor::Default).cstr(), val.cstr());
        all.push_back({str, ""});
    }

    printLong(all, "");
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
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }

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
        addrs.push_back(g_ByteCodeDebugger.cxtBp + over->computedValue.storageOffset);
    }

    if (nodes.empty())
    {
        printCmdError("no local variables");
        return BcDbgCommandResult::Continue;
    }

    Vector<std::pair<Utf8, Utf8>> result;
    getPrintSymbols(context, result, nodes, addrs);
    printLong(result, filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowArguments(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }
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

    Vector<std::pair<Utf8, Utf8>> result;
    getPrintSymbols(context, result, nodes, addrs);
    printLong(result, filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowScopes(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    Vector<std::pair<Utf8, Utf8>> result;

    auto node = g_ByteCodeDebugger.cxtIp->node;
    if (!node || !node->ownerScope)
    {
        printCmdError("cannot evaluate current scope");
        return BcDbgCommandResult::Error;
    }

    auto     scope = node->ownerScope;
    uint32_t idx   = 0;
    while (scope)
    {
        Utf8 value = form("%d ", idx++);

        switch (scope->kind)
        {
            case ScopeKind::Module:
                value += "[module] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::File:
                value += "[file] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Namespace:
                value += "[namespace] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Enum:
                value += "[enum] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Struct:
                value += "[struct] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Impl:
                value += "[impl] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Function:
                value += "[function] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::FunctionBody:
                value += "[function body] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::TypeLambda:
                value += "[type lambda] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Attribute:
                value += "[attribute] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Statement:
                value += "[statement] ";
                break;
            case ScopeKind::EmptyStatement:
                value += "[statement] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Breakable:
                value += "[breakable] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::TypeList:
                value += "[type list] ";
                value += scope->getDisplayFullName();
                break;
            case ScopeKind::Inline:
                value += "[inline] ";
                value += scope->owner->getScopedName();
                break;
            case ScopeKind::Macro:
                value += "[macro] ";
                value += scope->owner->getScopedName();
                break;
        }

        result.push_back({value, ""});
        scope = scope->parentScope;
    }

    printLong(result, "");

    return BcDbgCommandResult::Continue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdShowGlobals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFilter();
        return BcDbgCommandResult::Continue;
    }

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

    Vector<std::pair<Utf8, Utf8>> result;
    getPrintSymbols(context, result, nodes, addrs);
    printLong(result, filter);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShowRegisters(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFormat();
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
            printCmdError(form("invalid format [[%s]]", arg.split.back().cstr()));
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
        g_Log.print(form("%s$r%d%s = ", syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(), i, Log::colorToVTS(LogColor::Default).cstr()));
        g_Log.print(str);
        g_Log.writeEol();
    }

    g_Log.print(form("%s$sp%s = 0x%016llx\n", syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(), Log::colorToVTS(LogColor::Default).cstr(), context->sp));
    g_Log.print(form("%s$bp%s = 0x%016llx\n", syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(), Log::colorToVTS(LogColor::Default).cstr(), context->bp));
    g_Log.print(form("%s$rr0%s = 0x%016llx\n", syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(), Log::colorToVTS(LogColor::Default).cstr(), context->registersRR[0].u64));
    g_Log.print(form("%s$rr1%s = 0x%016llx\n", syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(), Log::colorToVTS(LogColor::Default).cstr(), context->registersRR[1].u64));

    return BcDbgCommandResult::Continue;
}

namespace
{
    void printSegmentRegisters(Module* module, SegmentKind kind)
    {
        const auto seg  = module->getSegment(kind);
        const auto name = seg->shortName;

        uint32_t index     = 0;
        uint32_t lastIndex = 0;
        for (const auto& b : seg->buckets)
        {
            g_Log.print(form("%s$%s%d%s = 0x%016llx (%d - %d)\n",
                             syntaxColorToVTS(SyntaxColor::SyntaxRegister).cstr(),
                             name,
                             index++,
                             Log::colorToVTS(LogColor::Default).cstr(),
                             b.buffer,
                             lastIndex,
                             lastIndex + b.count));
            lastIndex += b.count;
        }
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdShowSegments(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        printHelpFormat();
        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() > 2)
        return BcDbgCommandResult::TooManyArguments;

    if (!g_ByteCodeDebugger.cxtBc || !g_ByteCodeDebugger.cxtBc->sourceFile || !g_ByteCodeDebugger.cxtBc->sourceFile->module)
    {
        printCmdError("cannot find segments");
        return BcDbgCommandResult::Error;
    }

    g_Log.setColor(LogColor::Gray);

    printSegmentRegisters(g_ByteCodeDebugger.cxtBc->sourceFile->module, SegmentKind::Bss);
    printSegmentRegisters(g_ByteCodeDebugger.cxtBc->sourceFile->module, SegmentKind::Data);
    printSegmentRegisters(g_ByteCodeDebugger.cxtBc->sourceFile->module, SegmentKind::Constant);
    printSegmentRegisters(g_ByteCodeDebugger.cxtBc->sourceFile->module, SegmentKind::Compiler);
    printSegmentRegisters(g_ByteCodeDebugger.cxtBc->sourceFile->module, SegmentKind::Tls);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdShow(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    if (arg.split[1] == "locals" || arg.split[1] == "loc")
        return cmdShowLocals(context, arg);
    if (arg.split[1] == "globals" || arg.split[1] == "glob")
        return cmdShowGlobals(context, arg);
    if (arg.split[1] == "arguments" || arg.split[1] == "arg" || arg.split[1] == "args")
        return cmdShowArguments(context, arg);
    if (arg.split[1] == "scopes" || arg.split[1] == "sc")
        return cmdShowScopes(context, arg);

    if (arg.split[1] == "registers" || arg.split[1] == "reg" || arg.split[1] == "regs")
        return cmdShowRegisters(context, arg);
    if (arg.split[1] == "segments" || arg.split[1] == "seg" || arg.split[1] == "segs")
        return cmdShowSegments(context, arg);
    if (arg.split[1] == "expressions" || arg.split[1] == "expr" || arg.split[1] == "exprs")
        return cmdShowExpressions(context, arg);
    if (arg.split[1] == "values" || arg.split[1] == "val" || arg.split[1] == "vals")
        return cmdShowValues(context, arg);

    if (arg.split[1] == "modules" || arg.split[1] == "mod" || arg.split[1] == "mods")
        return cmdShowModules(context, arg);
    if (arg.split[1] == "files" || arg.split[1] == "fil")
        return cmdShowFiles(context, arg);
    if (arg.split[1] == "functions" || arg.split[1] == "func" || arg.split[1] == "funcs")
        return cmdShowFunctions(context, arg);
    if (arg.split[1] == "types" || arg.split[1] == "tp")
        return cmdShowTypes(context, arg);

    printCmdError(form("invalid [[show]] command [[%s]]", arg.split[1].cstr()));
    return BcDbgCommandResult::Error;
}
