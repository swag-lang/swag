#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Os.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"
#include "Diagnostic.h"

thread_local ByteCodeStack g_byteCodeStack;

void ByteCodeStack::log()
{
    if (steps.empty())
        return;
    for (int i = (int) steps.size() - 1; i >= 0; i--)
    {
        const auto& step = steps[i];
        Diagnostic  diag{step.ip->node->sourceFile, *step.ip->getLocation(step.bc), step.bc->name, DiagnosticLevel::CallStack};
        diag.report();
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
