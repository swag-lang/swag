#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "DiagnosticInfos.h"
#include "Os.h"

#ifdef SWAG_HAS_ASSERT
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