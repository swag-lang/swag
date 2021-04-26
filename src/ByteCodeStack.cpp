#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeRunContext.h"
#include "Ast.h"
#include "ErrorIds.h"

thread_local ByteCodeStack g_byteCodeStack;

uint32_t ByteCodeStack::maxLevel(ByteCodeRunContext* context)
{
    if (!context)
        return (uint32_t) steps.size() - 1;

    // The last stack step can be the same as the current context. If it's not the case,
    // then it's like we have one more step.
    auto& back = steps.back();
    if (back.bc == context->bc && back.ip == context->ip)
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
        Diagnostic diag{Msg0259, DiagnosticLevel::CallStack};
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
    if (fct)
    {
        Diagnostic diag{sourceFile, *location, fct->getNameForMessage().c_str(), DiagnosticLevel::CallStack};
        diag.stackLevel        = level;
        diag.currentStackLevel = current;
        diag.report();
    }
    else
    {
        Diagnostic diag{sourceFile, *location, bc->name, DiagnosticLevel::CallStack};
        diag.stackLevel        = level;
        diag.currentStackLevel = current;
        diag.report();
    }

    // #mixin
    if (ip->node->flags & AST_IN_MIXIN)
    {
        auto owner = ip->node->parent;
        while (owner && owner->kind != AstNodeKind::CompilerMixin)
            owner = owner->parent;
        if (owner)
        {
            fct = owner->ownerInline && owner->ownerInline->ownerFct == ip->node->ownerFct ? owner->ownerInline->func : owner->ownerFct;
            Diagnostic diag{owner->sourceFile, owner->token.startLocation, fct->getNameForMessage().c_str(), DiagnosticLevel::CallStackInlined};
            diag.report();
        }
    }

    // Inline chain
    auto parent = ip->node->ownerInline;
    while (parent && parent->ownerFct == ip->node->ownerFct)
    {
        fct = parent->ownerInline && parent->ownerInline->ownerFct == ip->node->ownerFct ? parent->ownerInline->func : parent->ownerFct;
        Diagnostic diag{parent->sourceFile, parent->token.startLocation, fct->getNameForMessage().c_str(), DiagnosticLevel::CallStackInlined};
        diag.report();
        parent = parent->ownerInline;
    }
}

void ByteCodeStack::getSteps(vector<ByteCodeStackStep>& copySteps)
{
    // Add one step for the current context if necessary
    copySteps = steps;
    if (currentContext)
    {
        if (copySteps.empty() || copySteps.back().bc != currentContext->bc || copySteps.back().ip != currentContext->ip)
        {
            ByteCodeStackStep step;
            step.bc = currentContext->bc;
            step.ip = currentContext->ip;
            step.bp = currentContext->bp;
            copySteps.push_back(step);
        }
    }
}

void ByteCodeStack::log()
{
    // Add one step for the current context if necessary
    vector<ByteCodeStackStep> copySteps;
    getSteps(copySteps);
    if (copySteps.empty())
        return;

    int maxSteps = 20;
    for (int i = (int) copySteps.size() - 1; i >= 0; i--)
    {
        maxSteps--;
        if (maxSteps == 0)
            break;

        SWAG_ASSERT(currentContext);
        bool current = false;
        if (currentContext->debugOn)
            current = i == (copySteps.size() - 1) - currentContext->debugStackFrameOffset;
        logStep(i, current, copySteps[i]);
    }
}

void ByteCodeStack::reportError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(msg);
    g_Log.eol();
    log();
    g_Log.setDefaultColor();
    g_Log.unlock();
}
