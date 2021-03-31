#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ByteCodeRunContext.h"
#include "Ast.h"

thread_local ByteCodeStack g_byteCodeStack;

void ByteCodeStack::log()
{
    if (steps.empty())
        return;

    auto stackLevel = 0;
    int  maxSteps   = min((int) steps.size() - 1, 20);
    for (int i = maxSteps + 1; i >= 0; i--)
    {
        ByteCodeInstruction* ip;
        ByteCode*            bc;
        if (i == maxSteps + 1)
        {
            if (!currentContext)
                continue;
            bc = currentContext->bc;
            ip = currentContext->ip;
        }
        else
        {
            bc = steps[i].bc;
            ip = steps[i].ip;

            // Remove that step if it's the same as the current context
            if (i == maxSteps)
            {
                if (currentContext && bc == currentContext->bc && ip == currentContext->ip)
                {
                    steps.pop_back();
                    continue;
                }
            }

            stackLevel++;
        }

        if (!ip)
        {
            Diagnostic diag{"<foreign code>", DiagnosticLevel::CallStack};
            diag.stackLevel = stackLevel;
            diag.report();
            continue;
        }

        if (!ip->node)
            continue;

        // Current ip
        auto sourceFile = ip->node->sourceFile;
        auto location   = ip->location;
        auto fct        = ip->node->ownerInline && ip->node->ownerInline->ownerFct == ip->node->ownerFct ? ip->node->ownerInline->func : ip->node->ownerFct;
        if (fct)
        {
            Diagnostic diag{sourceFile, *location, fct->getNameForMessage().c_str(), DiagnosticLevel::CallStack};
            diag.stackLevel = stackLevel;
            diag.report();
        }
        else
        {
            Diagnostic diag{sourceFile, *location, bc->name, DiagnosticLevel::CallStack};
            diag.stackLevel = stackLevel;
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
