#include "pch.h"
#include "Os/Os.h"

void swagAssert(const char* expr, const char* file, int line)
{
    OS::assertBox(expr, file, line);
}
