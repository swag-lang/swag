#include "pch.h"
#include "ByteCodeStack.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "ByteCodeRunContext.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "SyntaxColor.h"

thread_local ByteCodeStack  g_ByteCodeStackTraceVal;
thread_local ByteCodeStack* g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

uint32_t ByteCodeStack::maxLevel(ByteCodeRunContext* runContext)
{
    if (!runContext)
        return (uint32_t) steps.size() - 1;

    // The last stack step can be the same as the current context. If it's not the case,
    // then it's like we have one more step.
    const auto& back = steps.back();
    if (back.bc == runContext->bc && back.ip == runContext->ip)
        return (uint32_t) steps.size() - 1;

    // Not the case. One more step.
    return (uint32_t) steps.size();
}

Utf8 ByteCodeStack::getStepName(AstNode* node, ByteCodeInstruction* ip)
{
    const auto fct = node->ownerInline && node->ownerInline->ownerFct == ip->node->ownerFct ? node->ownerInline->func : node->ownerFct;
    if (fct && fct->hasExtByteCode() && fct->extByteCode()->bc)
        return fct->extByteCode()->bc->getPrintName();
    if (fct)
        return fct->token.text;
    return "";
}

static Utf8 sourceLine(SourceFile* sourceFile, int line)
{
    auto code = sourceFile->getLine(line);
    code.trim();
    SyntaxColorContext cxt;
    code = syntaxColor(code, cxt);
    code = "      " + code;
    return code;
}

Utf8 ByteCodeStack::getLogStep(int level, bool current, ByteCodeStackStep& step)
{
    auto bc = step.bc;
    auto ip = step.ip;

    Utf8 header;
    header += ByteCodeDebugger::COLOR_VTS_INDEX;
    if (current)
        header += Fmt("[%03u] ", level);
    else
        header += Fmt("-%03u- ", level);
    header += ByteCodeDebugger::COLOR_VTS_NAME;

    Utf8 inl;
    inl += ByteCodeDebugger::COLOR_VTS_INDEX;
    inl += "----- ";
    inl += ByteCodeDebugger::COLOR_VTS_NAME;

    if (!ip)
    {
        auto str = header;
        str += ByteCodeDebugger::COLOR_VTS_LOCATION;
        str += "<foreign code>";
        str += "\n";
        return str;
    }

    if (!ip->node)
        return "";

    // Current ip
    auto sourceFile = ip->node->sourceFile;
    auto location   = ip->location;
    auto name       = getStepName(ip->node, ip);
    if (name.empty())
        name = bc->getPrintName();

    Utf8 str = ip->node->ownerInline ? inl.c_str() : header.c_str();
    str += name;
    str += "\n";
    str += "      ";
    str += ByteCodeDebugger::COLOR_VTS_LOCATION;
    if (sourceFile)
        str += Fmt(" %s:%d:%d", sourceFile->path.string().c_str(), location->line + 1, location->column + 1);
    str += "\n";
    str += sourceLine(sourceFile, location->line);
    str += "\n";

    // #mixin
    if (ip->node->flags & AST_IN_MIXIN)
    {
        auto owner = ip->node->parent;
        while (owner && owner->kind != AstNodeKind::CompilerMixin)
            owner = owner->parent;
        if (owner)
        {
            str += owner->ownerInline ? inl.c_str() : header.c_str();
            str += ByteCodeDebugger::COLOR_VTS_NAME;
            str += getStepName(owner, ip);

            if (owner->sourceFile)
            {
                str += "\n";
                str += "      ";
                str += ByteCodeDebugger::COLOR_VTS_LOCATION;
                str += Fmt(" %s:%d:%d:%d:%d",
                           owner->sourceFile->path.string().c_str(),
                           owner->token.startLocation.line + 1,
                           owner->token.startLocation.column + 1,
                           owner->token.endLocation.line + 1,
                           owner->token.endLocation.column + 1);
                str += "\n";
                str += sourceLine(owner->sourceFile, owner->token.startLocation.line);
            }

            str += "\n";
        }
    }

    // Inline chain
    auto parent = ip->node->ownerInline;
    while (parent && parent->ownerFct == ip->node->ownerFct)
    {
        str += parent->ownerInline ? inl.c_str() : header.c_str();
        str += ByteCodeDebugger::COLOR_VTS_NAME;
        str += getStepName(parent, ip);

        if (parent->sourceFile)
        {
            str += "\n";
            str += "      ";
            str += ByteCodeDebugger::COLOR_VTS_LOCATION;
            str += Fmt(" %s:%d:%d:%d:%d",
                       parent->sourceFile->path.string().c_str(),
                       parent->token.startLocation.line + 1,
                       parent->token.startLocation.column + 1,
                       parent->token.endLocation.line + 1,
                       parent->token.endLocation.column + 1);
            str += "\n";
            str += sourceLine(parent->sourceFile, parent->token.startLocation.line);
        }

        str += "\n";
        parent = parent->ownerInline;
    }

    return str;
}

void ByteCodeStack::getSteps(VectorNative<ByteCodeStackStep>& copySteps, const ByteCodeRunContext* runContext) const
{
    // Add one step for the current context if necessary
    copySteps = steps;
    if (runContext)
    {
        if (copySteps.empty() || copySteps.back().bc != runContext->bc || copySteps.back().ip != runContext->ip)
        {
            ByteCodeStackStep step;
            step.bc    = runContext->bc;
            step.ip    = runContext->ip;
            step.bp    = runContext->bp;
            step.sp    = runContext->sp;
            step.spAlt = runContext->spAlt;
            step.stack = runContext->stack;
            copySteps.push_back(step);
        }
    }
}

Utf8 ByteCodeStack::log(ByteCodeRunContext* runContext)
{
    // Add one step for the current context if necessary
    VectorNative<ByteCodeStackStep> copySteps;
    getSteps(copySteps, runContext);
    if (copySteps.empty())
        return "";

    Utf8      str;
    int       maxSteps = 20;
    const int nb       = (int) copySteps.size() - 1;
    for (int i = nb; i >= 0; i--)
    {
        bool current = false;
        if (runContext && runContext->debugOn)
            current = (size_t) i == (copySteps.size() - 1) - runContext->debugStackFrameOffset;
        str += getLogStep(i, current, copySteps[(size_t) i]);

        maxSteps--;
        if (maxSteps == 0)
            break;
    }

    return str;
}
