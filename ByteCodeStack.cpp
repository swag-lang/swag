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
    for (int i = (int) steps.size() - 1; i >= 0; i--)
    {
        const auto& step = steps[i];

        SourceFile*     sourceFile;
        SourceLocation* location;
        ByteCode::getLocation(step.bc, step.ip, &sourceFile, &location);

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
