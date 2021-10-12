#include "pch.h"
#include "CommandLine.h"
#include "Backend.h"
#include "ErrorIds.h"
#include "Log.h"

CommandLine* g_CommandLine = nullptr;

bool CommandLine::check()
{
    // Stack
    stackSizeRT = (uint32_t) Allocator::alignSize(stackSizeRT);
    if (stackSizeRT < SWAG_LIMIT_MIN_STACK || stackSizeRT > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(Utf8::format(g_E[Err0519], Utf8::toNiceSize(stackSizeRT).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    stackSizeBC = (uint32_t) Allocator::alignSize(stackSizeBC);
    if (stackSizeBC < SWAG_LIMIT_MIN_STACK || stackSizeBC > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(Utf8::format(g_E[Err0519], Utf8::toNiceSize(stackSizeBC).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    // Force verbose
    if (verboseCmdLine || verbosePath || verboseLink || verboseTestErrors || verboseConcreteTypes)
        verbose = true;

    // Stats
    if (statsWhat != StatsWhat::None)
        stats = true;

    // Check special backend X64
    if (backendGenType == BackendGenType::X64)
    {
        if (abi != BackendAbi::Msvc)
        {
            g_Log.error(Utf8::format(g_E[Err0520], Backend::GetAbiName()));
            return false;
        }

        if (vendor != BackendVendor::Pc)
        {
            g_Log.error(Utf8::format(g_E[Err0521], Backend::GetVendorName()));
            return false;
        }

        if (os != BackendOs::Windows)
        {
            g_Log.error(Utf8::format(g_E[Err0522], Backend::GetOsName()));
            return false;
        }

        if (arch != BackendArch::X86_64)
        {
            g_Log.error(Utf8::format(g_E[Err0523], Backend::GetArchName()));
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

    // Add/check script file extension
    if (!g_CommandLine->scriptName.empty())
    {
        fs::path p(g_CommandLine->scriptName);
        if (p.extension().string().empty())
        {
            g_CommandLine->scriptName += ".swgs";
        }
        else if (p.extension().string() != ".swgs")
        {
            g_Log.error(Utf8::format(g_E[Err0319], p.extension().string().c_str()));
            return false;
        }
    }

    return true;
}