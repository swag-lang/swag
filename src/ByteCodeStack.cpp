#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeRunContext.h"
#include "Ast.h"
#include "ErrorIds.h"

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

void ByteCodeStack::logStep(int level, bool current, ByteCodeStackStep& step)
{
    auto bc = step.bc;
    auto ip = step.ip;

    if (!ip)
    {
        Diagnostic diag{"<foreign code>", DiagnosticLevel::CallStack};
        diag.stackLevel        = level;
        diag.currentStackLevel = current;
        diag.report();
        return;
    }

    if (!ip->node)
        return;

    // Current ip
    auto sourceFile = ip->node->sourceFile;
    auto location   = ip->location;
    auto fct        = ip->node->ownerInline && ip->node->ownerInline->ownerFct == ip->node->ownerFct ? ip->node->ownerInline->func : ip->node->ownerFct;
    auto name       = fct ? fct->getDisplayNameC() : bc->name.c_str();

    Diagnostic diag{sourceFile, *location, name, DiagnosticLevel::CallStack};
    diag.raisedOnNode      = ip->node;
    diag.stackLevel        = level;
    diag.currentStackLevel = current;
    diag.report();

    // #mixin
    if (ip->node->flags & AST_IN_MIXIN)
    {
        auto owner = ip->node->parent;
        while (owner && owner->kind != AstNodeKind::CompilerMixin)
            owner = owner->parent;
        if (owner)
        {
            fct = owner->ownerInline && owner->ownerInline->ownerFct == ip->node->ownerFct ? owner->ownerInline->func : owner->ownerFct;
            Diagnostic diagMixin{owner->sourceFile, owner->token.startLocation, fct->getDisplayNameC(), DiagnosticLevel::CallStack};
            diagMixin.raisedOnNode = owner;
            diagMixin.report();
        }
    }

    // Inline chain
    auto parent = ip->node->ownerInline;
    while (parent && parent->ownerFct == ip->node->ownerFct && parent->ownerInline)
    {
        fct = parent->ownerInline && parent->ownerInline->ownerFct == ip->node->ownerFct ? parent->ownerInline->func : parent->ownerFct;
        Diagnostic diagInline{parent->sourceFile, parent->token.startLocation, fct->getDisplayNameC(), DiagnosticLevel::CallStack};
        diagInline.raisedOnNode = parent;
        diagInline.report();
        parent = parent->ownerInline;
    }
}

void ByteCodeStack::getSteps(VectorNative<ByteCodeStackStep>& copySteps, ByteCodeRunContext* runContext)
{
    // Add one step for the current context if necessary
    auto copy = steps;
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
            copy.push_back(step);
        }
    }

    for (auto& it : copy)
    {
        // If we are running bytecode, and a panic, error or warning was raised, we filter the stack so that only
        // relevent informations are displayed.
        // No need to display runtime functions.
        if (runContext->fromException666 && it.bc && it.ip)
        {
            auto loc = ByteCode::getLocation(it.bc, it.ip);
            if (loc.file && loc.file->isRuntimeFile)
                continue;
            if (loc.file && loc.file->isBootstrapFile)
                continue;
        }

        copySteps.push_back(it);
    }
}

void ByteCodeStack::log(ByteCodeRunContext* runContext)
{
    // Add one step for the current context if necessary
    VectorNative<ByteCodeStackStep> copySteps;
    getSteps(copySteps, runContext);
    if (copySteps.empty())
        return;

    size_t maxSteps = 20;
    size_t nb       = copySteps.size() - 1;
    for (size_t i = nb; i >= 0; i--)
    {
        bool current = false;
        if (runContext && runContext->debugOn)
            current = i == (copySteps.size() - 1) - runContext->debugStackFrameOffset;
        logStep((int) i, current, copySteps[i]);

        maxSteps--;
        if (maxSteps == 0)
            break;
    }
}
