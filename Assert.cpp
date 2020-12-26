#include "pch.h"
#include "ByteCodeStack.h"
#include "Os.h"

void swag_assert(const char* expr, const char* file, int line)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(format("assertion failed: %s:%d: %s\n", file, line, expr));
    g_byteCodeStack.log();
    g_Log.setDefaultColor();
    g_Log.unlock();
    OS::assertBox(expr, file, line);
}
