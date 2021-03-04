#include "pch.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "Ast.h"

thread_local ByteCodeStack g_byteCodeStack;

void ByteCodeStack::log()
{
    if (steps.empty())
        return;

    int maxSteps = min((int) steps.size() - 1, 20);
    for (int i = maxSteps; i >= 0; i--)
    {
        const auto& step = steps[i];
        auto        ip   = step.ip;

        // Current ip
        auto sourceFile = ip->node && ip->node->sourceFile ? ip->node->sourceFile : step.bc->sourceFile;
        auto location   = ip->location;
        if (step.bc->node && step.bc->node->kind == AstNodeKind::FuncDecl)
        {
            auto       funcDecl = CastAst<AstFuncDecl>(step.bc->node, AstNodeKind::FuncDecl);
            Diagnostic diag{sourceFile, *location, funcDecl->getNameForMessage().c_str(), DiagnosticLevel::CallStack};
            diag.report();
        }
        else
        {
            Diagnostic diag{sourceFile, *location, step.bc->name, DiagnosticLevel::CallStack};
            diag.report();
        }

        // #mixin
        if (ip->node && ip->node->flags & AST_IN_MIXIN)
        {
            auto owner = ip->node->parent;
            while (owner->kind != AstNodeKind::CompilerMixin)
                owner = owner->parent;
            auto       fct = owner->ownerInline && owner->ownerInline->ownerFct == ip->node->ownerFct ? owner->ownerInline->func : owner->ownerFct;
            Diagnostic diag{owner->sourceFile, owner->token.startLocation, fct->getNameForMessage().c_str(), DiagnosticLevel::CallStackInlined};
            diag.report();
        }

        // Inline chain
        if (ip->node)
        {
            auto parent = ip->node->ownerInline;
            while (parent && parent->ownerFct == ip->node->ownerFct)
            {
                Diagnostic diag{parent->sourceFile, parent->token.startLocation, parent->ownerFct->getNameForMessage().c_str(), DiagnosticLevel::CallStackInlined};
                diag.report();
                parent = parent->ownerInline;
            }
        }
    }

    g_byteCodeStack.clear();
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
