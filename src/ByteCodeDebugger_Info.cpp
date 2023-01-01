#include "pch.h"
#include "TypeInfo.h"
#include "ByteCode.h"
#include "Workspace.h"
#include "Module.h"
#include "Ast.h"
#include "ByteCodeDebugger.h"

BcDbgCommandResult ByteCodeDebugger::cmdInfoFuncs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    auto           filter = cmds.size() == 3 ? cmds[2] : Utf8("");
    vector<string> all;
    g_Log.setColor(LogColor::Gray);
    for (auto m : g_Workspace->modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (filter.empty() || bc->name.find(filter) != -1)
            {
                string          str = Fmt("%s%s%s ", COLOR_NAME, bc->getCallName().c_str(), COLOR_DEFAULT).c_str();
                SourceFile*     bcFile;
                SourceLocation* bcLocation;
                ByteCode::getLocation(bc, bc->out, &bcFile, &bcLocation);
                str += Fmt(" (%s%s%s) ", COLOR_TYPE, bc->getCallType()->getDisplayNameC(), COLOR_DEFAULT);
                if (bcFile)
                    str += Fmt("%s", bcFile->name.c_str());
                if (bcLocation)
                    str += Fmt(":%d", bcLocation->line);
                all.push_back(str);
            }
        }
    }

    sort(all.begin(), all.end(), [](const string& a, const string& b)
         { return a < b; });
    for (auto& o : all)
    {
        if (OS::longOpStopKeyPressed())
        {
            g_Log.printColor("...stopped\n", LogColor::Red);
            break;
        }

        g_Log.print(o);
        g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoModules(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    g_Log.setColor(LogColor::Gray);
    for (auto m : g_Workspace->modules)
    {
        g_Log.print(Fmt("%s\n", m->name.c_str()));
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoLocals(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    if (context->debugCxtBc->localVars.empty())
    {
        g_Log.printColor("no locals\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    for (auto l : context->debugCxtBc->localVars)
    {
        auto over = l->resolvedSymbolOverload;
        if (!over)
            continue;
        // Generated
        if (over->symbol->name.length() > 2 && over->symbol->name[0] == '_' && over->symbol->name[1] == '_')
            continue;
        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_TYPE, over->typeInfo->getDisplayNameC(), COLOR_DEFAULT, COLOR_NAME, over->symbol->name.c_str(), COLOR_DEFAULT);
        EvaluateResult res;
        res.type = over->typeInfo;
        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
        appendTypedValue(context, str, res, 0);
        g_Log.printColor(str);
        if (str.back() != '\n')
            g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoRegs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    ValueFormat fmt;
    fmt.isHexa   = true;
    fmt.bitCount = 64;
    if (cmds.size() > 2)
    {
        if (!getValueFormat(cmds[2], fmt))
            return BcDbgCommandResult::EvalDefault;
    }

    g_Log.setColor(LogColor::Gray);
    for (int i = 0; i < context->getRegCount(context->debugCxtRc); i++)
    {
        auto& regP = context->getRegBuffer(context->debugCxtRc)[i];
        Utf8  str;
        appendLiteralValue(context, str, fmt, &regP);
        str.trim();
        g_Log.print(Fmt("%s$r%d%s = ", COLOR_NAME, i, COLOR_DEFAULT));
        g_Log.print(str);
        g_Log.eol();
    }

    g_Log.print(Fmt("%s$sp%s = 0x%016llx\n", COLOR_NAME, COLOR_DEFAULT, context->sp));
    g_Log.print(Fmt("%s$bp%s = 0x%016llx\n", COLOR_NAME, COLOR_DEFAULT, context->bp));

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdInfoArgs(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 3)
        return BcDbgCommandResult::EvalDefault;

    auto funcDecl = CastAst<AstFuncDecl>(context->debugCxtBc->node, AstNodeKind::FuncDecl);
    if (!funcDecl->parameters || funcDecl->parameters->childs.empty())
    {
        g_Log.printColor("no arguments\n", LogColor::Red);
        return BcDbgCommandResult::Continue;
    }

    for (auto l : funcDecl->parameters->childs)
    {
        auto over = l->resolvedSymbolOverload;
        if (!over)
            continue;
        Utf8           str = Fmt("(%s%s%s) %s%s%s = ", COLOR_TYPE, over->typeInfo->getDisplayNameC(), COLOR_DEFAULT, COLOR_NAME, over->symbol->name.c_str(), COLOR_DEFAULT);
        EvaluateResult res;
        res.type = over->typeInfo;
        res.addr = context->debugCxtBp + over->computedValue.storageOffset;
        appendTypedValue(context, str, res, 0);
        g_Log.printColor(str);
        if (str.back() != '\n')
            g_Log.eol();
    }

    return BcDbgCommandResult::Continue;
}