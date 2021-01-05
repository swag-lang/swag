#include "pch.h"
#include "CommandLine.h"
#include "Allocator.h"
#include "Log.h"
#include "Global.h"

CommandLine g_CommandLine;

bool CommandLine::check()
{
    // Stack
    stackSize = (uint32_t) Allocator::alignSize(stackSize);
    if (stackSize < SWAG_LIMIT_MIN_STACK || stackSize > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(format("command line error: invalid --stack-size value (%s), valid range is [%s, %s]", toNiceSize(stackSize).c_str(), toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    // Force verbose
    if (verboseCmdLine || verbosePath || verboseLink || verboseTestErrors || verbosePass || verboseConcreteTypes)
        verbose = true;

    return true;
}