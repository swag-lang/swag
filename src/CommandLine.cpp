#include "pch.h"
#include "CommandLine.h"
#include "Allocator.h"
#include "Log.h"
#include "Global.h"
#include "Backend.h"

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

    // Check special backend X64
    if (backendType == BackendType::X64)
    {
        if (abi != BackendAbi::Msvc)
        {
            g_Log.error(format("command line error: invalid abi '%s' for x64 backend", Backend::GetAbiName()));
            return false;
        }

        if (vendor != BackendVendor::Pc)
        {
            g_Log.error(format("command line error: invalid vendor '%s' for x64 backend", Backend::GetVendorName()));
            return false;
        }

        if (os != BackendOs::Windows)
        {
            g_Log.error(format("command line error: invalid os '%s' for x64 backend", Backend::GetOsName()));
            return false;
        }

        if (arch != BackendArch::X86_64)
        {
            g_Log.error(format("command line error: invalid arch '%s' for x64 backend", Backend::GetArchName()));
            return false;
        }
    }

    // Make some paths canonical
    if (!workspacePath.empty())
    {
        workspacePath = fs::absolute(workspacePath).string();
        workspacePath = fs::canonical(workspacePath).string();
    }

    if (!cachePath.empty())
    {
        cachePath = fs::absolute(cachePath).string();
        cachePath = fs::canonical(cachePath).string();
    }

    return true;
}