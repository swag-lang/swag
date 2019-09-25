#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"

#ifdef SWAG_HAS_ASSERT
thread_local DiagnosticInfos g_diagnosticInfos;

void swag_assert(const char* expr, const char* file, int line)
{
    g_Log.lock();

    g_Log.setColor(LogColor::Red);
    g_Log.print(format("assertion failed: %s:%d: %s\n", file, line, expr));
	g_Log.setColor(LogColor::Cyan);
    if (!g_diagnosticInfos.pass.empty())
        g_Log.print(format("pass:        %s\n", g_diagnosticInfos.pass.c_str()));
    if (g_diagnosticInfos.sourceFile)
        g_Log.print(format("source file: %s\n", g_diagnosticInfos.sourceFile->path.string().c_str()));
    if (g_diagnosticInfos.node)
        g_Log.print(format("source line: %d\n", g_diagnosticInfos.node->token.startLocation.line));
    g_Log.setDefaultColor();

    assert(false);
    exit(-1);
}
#endif