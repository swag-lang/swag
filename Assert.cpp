#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Os.h"

#ifdef SWAG_HAS_ASSERT
thread_local DiagnosticInfos g_diagnosticInfos;

void DiagnosticInfos::log()
{
    if (steps.empty())
        return;

    g_Log.setColor(LogColor::DarkYellow);
    for (int i = (int) steps.size() - 1; i >= 0; i--)
    {
        const auto& step = steps[i];

        g_Log.print("----------------\n");
        if (!step.message.empty())
            g_Log.print(format("note:        %s\n", step.message.c_str()));
        if (step.sourceFile)
            g_Log.print(format("source file: %s\n", step.sourceFile->path.c_str()));
        if (step.node)
            g_Log.print(format("source line: %d\n", step.node->token.startLocation.line + 1));
    }

    g_Log.print("----------------\n");
    g_Log.setDefaultColor();
}

void DiagnosticInfos::reportError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(msg);
    log();
    g_Log.setDefaultColor();
    g_Log.unlock();
}

void swag_assert(const char* expr, const char* file, int line)
{
    g_Log.lock();

    g_Log.setColor(LogColor::Red);
    g_Log.print(format("assertion failed: %s:%d: %s\n", file, line, expr));
    g_diagnosticInfos.log();
    g_Log.setDefaultColor();

#ifndef _DEBUG
    OS::assertBox(expr, file, line);
#endif
    assert(false);
}
#endif