#include "pch.h"
#include "ByteCode.h"
#include "Workspace.h"
#include "Module.h"
#include "Ast.h"
#include "ByteCodeDebugger.h"

BcDbgCommandResult ByteCodeDebugger::cmdInfoFuncs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 3)
        return BcDbgCommandResult::BadArguments;

    auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    Vector<Utf8> all;
    g_Log.setColor(LogColor::Gray);

    uint32_t total = 0;
    for (auto m : g_Workspace->modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (!bc->out)
                continue;
            total++;
            if (g_ByteCodeDebugger.testNameFilter(bc->getPrintName(), filter) ||
                g_ByteCodeDebugger.testNameFilter(bc->getPrintFileName(), filter))
            {
                all.push_back(bc->getPrintRefName());
            }
        }
    }

    if (all.empty())
    {
        g_ByteCodeDebugger.printCmdError(Fmt("...no match (%d parsed functions)", total));
        return BcDbgCommandResult::Continue;
    }

    sort(all.begin(), all.end(), [](const Utf8& a, const Utf8& b)
         { return strcmp(a.c_str(), b.c_str()) < 0; });
    g_ByteCodeDebugger.printLong(all);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoModules(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    Vector<Utf8> all;
    for (auto m : g_Workspace->modules)
        all.push_back(m->name);
    sort(all.begin(), all.end(), [](const Utf8& a, const Utf8& b)
         { return strcmp(a.c_str(), b.c_str()) < 0; });
    g_ByteCodeDebugger.printLong(all);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoVariables(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 3)
        return BcDbgCommandResult::BadArguments;
    auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    Utf8 result;
    auto m = context->jc.sourceFile->module;

    for (auto n : m->globalVarsBss)
    {
        if (!n->resolvedSymbolOverload)
            continue;
        uint8_t* addr = m->bssSegment.address(n->resolvedSymbolOverload->computedValue.storageOffset);
        g_ByteCodeDebugger.appendTypedValue(context, filter, n, nullptr, addr, result);
    }

    for (auto n : m->globalVarsMutable)
    {
        if (!n->resolvedSymbolOverload)
            continue;
        uint8_t* addr = m->mutableSegment.address(n->resolvedSymbolOverload->computedValue.storageOffset);
        g_ByteCodeDebugger.appendTypedValue(context, filter, n, nullptr, addr, result);
    }

    g_ByteCodeDebugger.printLong(result);
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoLocals(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 3)
        return BcDbgCommandResult::BadArguments;
    auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    if (g_ByteCodeDebugger.debugCxtBc->localVars.empty())
    {
        g_ByteCodeDebugger.printCmdError("no locals");
        return BcDbgCommandResult::Continue;
    }

    Utf8 result;
    for (auto l : g_ByteCodeDebugger.debugCxtBc->localVars)
        g_ByteCodeDebugger.appendTypedValue(context, filter, l, g_ByteCodeDebugger.debugCxtBp, nullptr, result);
    g_ByteCodeDebugger.printLong(result);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoArgs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 3)
        return BcDbgCommandResult::BadArguments;
    auto filter = arg.split.size() == 3 ? arg.split[2] : Utf8("");

    auto funcDecl = CastAst<AstFuncDecl>(g_ByteCodeDebugger.debugCxtBc->node, AstNodeKind::FuncDecl);
    if (!funcDecl->parameters || funcDecl->parameters->childs.empty())
    {
        g_ByteCodeDebugger.printCmdError("no arguments");
        return BcDbgCommandResult::Continue;
    }

    Utf8 result;
    for (auto l : funcDecl->parameters->childs)
        g_ByteCodeDebugger.appendTypedValue(context, filter, l, g_ByteCodeDebugger.debugCxtBp, nullptr, result);
    g_ByteCodeDebugger.printLong(result);

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoRegs(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 3)
        return BcDbgCommandResult::BadArguments;

    ValueFormat fmt;
    fmt.isHexa   = true;
    fmt.bitCount = 64;
    if (arg.split.size() > 2)
    {
        if (!g_ByteCodeDebugger.getValueFormat(arg.split[2], fmt))
            return BcDbgCommandResult::BadArguments;
    }

    g_Log.setColor(LogColor::Gray);
    for (int i = 0; i < context->getRegCount(g_ByteCodeDebugger.debugCxtRc); i++)
    {
        auto& regP = context->getRegBuffer(g_ByteCodeDebugger.debugCxtRc)[i];
        Utf8  str;
        g_ByteCodeDebugger.appendLiteralValue(context, str, fmt, &regP);
        str.trim();
        g_Log.print(Fmt("%s$r%d%s = ", COLOR_VTS_NAME.c_str(), i, COLOR_VTS_DEFAULT.c_str()));
        g_Log.print(str);
        g_Log.eol();
    }

    g_Log.print(Fmt("%s$sp%s = 0x%016llx\n", COLOR_VTS_NAME.c_str(), COLOR_VTS_DEFAULT.c_str(), context->sp));
    g_Log.print(Fmt("%s$bp%s = 0x%016llx\n", COLOR_VTS_NAME.c_str(), COLOR_VTS_DEFAULT.c_str(), context->bp));

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfo(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() < 2)
        return BcDbgCommandResult::BadArguments;

    if (arg.split[1] == "locals" || arg.split[1] == "loc")
        return cmdInfoLocals(context, arg);
    if (arg.split[1] == "arguments" || arg.split[1] == "arg")
        return cmdInfoArgs(context, arg);
    if (arg.split[1] == "registers" || arg.split[1] == "reg")
        return cmdInfoRegs(context, arg);
    if (arg.split[1] == "functions" || arg.split[1] == "func")
        return cmdInfoFuncs(context, arg);
    if (arg.split[1] == "variables" || arg.split[1] == "var")
        return cmdInfoVariables(context, arg);
    if (arg.split[1] == "modules")
        return cmdInfoModules(context, arg);

    return BcDbgCommandResult::BadArguments;
}