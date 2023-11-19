#include "pch.h"
#include "ByteCode.h"
#include "Workspace.h"
#include "Module.h"
#include "Ast.h"
#include "ByteCodeDebugger.h"

Utf8 ByteCodeDebugger::getByteCodeName(ByteCode* bc)
{
    if (bc->node)
        return bc->node->getScopedName();
    return bc->name;
}

Utf8 ByteCodeDebugger::getByteCodeFileName(ByteCode* bc)
{
    if (bc->sourceFile)
        return bc->sourceFile->name;
    return "";
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoFuncs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::BadArguments;

    auto         filter = cmds.size() == 3 ? cmds[2] : Utf8("");
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
            if (filter.empty() || g_ByteCodeDebugger.getByteCodeName(bc).find(filter) != -1 || g_ByteCodeDebugger.getByteCodeFileName(bc).find(filter) != -1)
            {
                Utf8 str = Fmt("%s%s%s ", COLOR_VTS_NAME, g_ByteCodeDebugger.getByteCodeName(bc).c_str(), COLOR_VTS_DEFAULT).c_str();
                auto loc = ByteCode::getLocation(bc, bc->out);
                str += Fmt(" %s%s%s ", COLOR_VTS_TYPE, bc->getCallType()->getDisplayNameC(), COLOR_VTS_DEFAULT);
                if (loc.file)
                    str += Fmt("%s", loc.file->name.c_str());
                if (loc.location)
                    str += Fmt(":%d", loc.location->line);
                all.push_back(str);
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
    for (auto& o : all)
    {
        if (OS::longOpStopKeyPressed())
        {
            g_ByteCodeDebugger.printCmdError("...stopped");
            break;
        }

        g_Log.print(o);
        g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoModules(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    g_Log.setColor(LogColor::Gray);
    for (auto m : g_Workspace->modules)
    {
        g_Log.print(Fmt("%s\n", m->name.c_str()));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoLocals(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (g_ByteCodeDebugger.debugCxtBc->localVars.empty())
    {
        g_ByteCodeDebugger.printCmdError("no locals");
        return BcDbgCommandResult::Continue;
    }

    for (auto l : g_ByteCodeDebugger.debugCxtBc->localVars)
    {
        auto over = l->resolvedSymbolOverload;
        if (!over)
            continue;
        // Generated
        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
            continue;
        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_VTS_TYPE, over->typeInfo->getDisplayNameC(), COLOR_VTS_DEFAULT, COLOR_VTS_NAME, over->symbol->name.c_str(), COLOR_VTS_DEFAULT);
        EvaluateResult res;
        res.type = over->typeInfo;
        res.addr = g_ByteCodeDebugger.debugCxtBp + over->computedValue.storageOffset;
        g_ByteCodeDebugger.appendTypedValue(context, str, res, 0);
        g_Log.print(str);
        if (str.back() != '\n')
            g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoRegs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::BadArguments;

    ValueFormat fmt;
    fmt.isHexa   = true;
    fmt.bitCount = 64;
    if (cmds.size() > 2)
    {
        if (!g_ByteCodeDebugger.getValueFormat(cmds[2], fmt))
            return BcDbgCommandResult::BadArguments;
    }

    g_Log.setColor(LogColor::Gray);
    for (int i = 0; i < context->getRegCount(g_ByteCodeDebugger.debugCxtRc); i++)
    {
        auto& regP = context->getRegBuffer(g_ByteCodeDebugger.debugCxtRc)[i];
        Utf8  str;
        g_ByteCodeDebugger.appendLiteralValue(context, str, fmt, &regP);
        str.trim();
        g_Log.print(Fmt("%s$r%d%s = ", COLOR_VTS_NAME, i, COLOR_VTS_DEFAULT));
        g_Log.print(str);
        g_Log.eol();
    }

    g_Log.print(Fmt("%s$sp%s = 0x%016llx\n", COLOR_VTS_NAME, COLOR_VTS_DEFAULT, context->sp));
    g_Log.print(Fmt("%s$bp%s = 0x%016llx\n", COLOR_VTS_NAME, COLOR_VTS_DEFAULT, context->bp));

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoArgs(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 2)
        return BcDbgCommandResult::BadArguments;

    auto funcDecl = CastAst<AstFuncDecl>(g_ByteCodeDebugger.debugCxtBc->node, AstNodeKind::FuncDecl);
    if (!funcDecl->parameters || funcDecl->parameters->childs.empty())
    {
        g_ByteCodeDebugger.printCmdError("no arguments");
        return BcDbgCommandResult::Continue;
    }

    for (auto l : funcDecl->parameters->childs)
    {
        auto over = l->resolvedSymbolOverload;
        if (!over)
            continue;
        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_VTS_TYPE, over->typeInfo->getDisplayNameC(), COLOR_VTS_DEFAULT, COLOR_VTS_NAME, over->symbol->name.c_str(), COLOR_VTS_DEFAULT);
        EvaluateResult res;
        res.type = over->typeInfo;
        res.addr = g_ByteCodeDebugger.debugCxtBp + over->computedValue.storageOffset;
        g_ByteCodeDebugger.appendTypedValue(context, str, res, 0);
        g_Log.print(str);
        if (str.back() != '\n')
            g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfo(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() < 2)
        return BcDbgCommandResult::BadArguments;

    if (cmds[1] == "locals" || cmds[1] == "l")
        return cmdInfoLocals(context, cmds, cmdExpr);
    if (cmds[1] == "regs" || cmds[1] == "r")
        return cmdInfoRegs(context, cmds, cmdExpr);
    if (cmds[1] == "args" || cmds[1] == "a")
        return cmdInfoArgs(context, cmds, cmdExpr);
    if (cmds[1] == "breakpoints" || cmds[1] == "br")
        return cmdBreakPrint(context, cmds, cmdExpr);
    if (cmds[1] == "func")
        return cmdInfoFuncs(context, cmds, cmdExpr);
    if (cmds[1] == "module")
        return cmdInfoModules(context, cmds, cmdExpr);

    return BcDbgCommandResult::BadArguments;
}