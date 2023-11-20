#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeRunContext.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "ByteCodeDebugger.h"

thread_local ByteCodeStack  g_ByteCodeStackTraceVal;
thread_local ByteCodeStack* g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

uint32_t ByteCodeStack::maxLevel(ByteCodeRunContext* runContext)
{
    if (!runContext)
        return (uint32_t) steps.size() - 1;

    // The last stack step can be the same as the current context. If it's not the case,
    // then it's like we have one more step.
    auto& back = steps.back();
    if (back.bc == runContext->bc && back.ip == runContext->ip)
        return (uint32_t) steps.size() - 1;

    // Not the case. One more step.
    return (uint32_t) steps.size();
}

Utf8 ByteCodeStack::getLogStep(int level, bool current, ByteCodeStackStep& step)
{
    auto bc = step.bc;
    auto ip = step.ip;

    Utf8 header;
    header += Log::colorToVTS(LogColor::DarkYellow);
    if (current)
        header += Fmt("[%03u] ", level);
    else
        header += Fmt("-%03u- ", level);

    Utf8 inl;
    inl += Log::colorToVTS(LogColor::DarkYellow);
    inl += "----- inline ";

    if (!ip)
    {
        auto str = header;
        str += Log::colorToVTS(LogColor::Gray);
        str += "--> <foreign code>";
        str += "\n";
        return str;
    }

    if (!ip->node)
        return "";

    // Current ip
    auto sourceFile = ip->node->sourceFile;
    auto location   = ip->location;
    auto fct        = ip->node->ownerInline && ip->node->ownerInline->ownerFct == ip->node->ownerFct ? ip->node->ownerInline->func : ip->node->ownerFct;
    auto name       = fct ? fct->getDisplayNameC() : bc->name.c_str();

    Utf8 str = ip->node->ownerInline ? inl.c_str() : header.c_str();
    str += name;
    str += ByteCodeDebugger::COLOR_VTS_LOCATION;
    if (sourceFile)
        str += Fmt(" %s:%d", sourceFile->path.string().c_str(), location->line + 1);
    str += "\n";

    // #mixin
    if (ip->node->flags & AST_IN_MIXIN)
    {
        auto owner = ip->node->parent;
        while (owner && owner->kind != AstNodeKind::CompilerMixin)
            owner = owner->parent;
        if (owner)
        {
            fct = owner->ownerInline && owner->ownerInline->ownerFct == ip->node->ownerFct ? owner->ownerInline->func : owner->ownerFct;
            str = owner->ownerInline ? inl.c_str() : header.c_str();
            str += fct->getDisplayNameC();
            str += ByteCodeDebugger::COLOR_VTS_LOCATION;
            if (owner->sourceFile)
                str += Fmt(" %s:%d", owner->sourceFile->path.string().c_str(), owner->token.startLocation.line + 1);
            str += "\n";
        }
    }

    // Inline chain
    auto parent = ip->node->ownerInline;
    while (parent && parent->ownerFct == ip->node->ownerFct)
    {
        fct = parent->ownerInline && parent->ownerInline->ownerFct == ip->node->ownerFct ? parent->ownerInline->func : parent->ownerFct;
        str += parent->ownerInline ? inl.c_str() : header.c_str();
        str += fct->getDisplayNameC();
        str += ByteCodeDebugger::COLOR_VTS_LOCATION;
        if (parent->sourceFile)
            str += Fmt(" %s:%d", parent->sourceFile->path.string().c_str(), parent->token.startLocation.line + 1);
        str += "\n";
        parent = parent->ownerInline;
    }

    return str;
}

void ByteCodeStack::getSteps(VectorNative<ByteCodeStackStep>& copySteps, ByteCodeRunContext* runContext)
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

    Utf8 str;
    int  maxSteps = 20;
    int  nb       = (int) copySteps.size() - 1;
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
