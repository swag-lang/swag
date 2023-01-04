#include "pch.h"
#include "Os.h"
#include "ByteCode.h"

void swagAssert(const char* expr, const char* file, int line)
{
    g_ByteCodeStackTrace->log();
    OS::assertBox(expr, file, line);
}
