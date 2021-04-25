#include "pch.h"
#include "ByteCodeStack.h"
#include "Os.h"

void swag_assert(const char* expr, const char* file, int line)
{
    OS::assertBox(expr, file, line);
}
