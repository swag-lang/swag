#include "pch.h"
#include "CommandLine.h"
#include "Backend.h"
#include "ErrorIds.h"

CommandLine g_CommandLine;

bool CommandLine::check()
{
    // Stack
    stackSize = (uint32_t) Allocator::alignSize(stackSize);
    if (stackSize < SWAG_LIMIT_MIN_STACK || stackSize > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(Utf8::format(Msg0519, Utf8::toNiceSize(stackSize).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
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
            g_Log.error(Utf8::format(Msg0520, Backend::GetAbiName()));
            return false;
        }

        if (vendor != BackendVendor::Pc)
        {
            g_Log.error(Utf8::format(Msg0521, Backend::GetVendorName()));
            return false;
        }

        if (os != BackendOs::Windows)
        {
            g_Log.error(Utf8::format(Msg0522, Backend::GetOsName()));
            return false;
        }

        if (arch != BackendArch::X86_64)
        {
            g_Log.error(Utf8::format(Msg0523, Backend::GetArchName()));
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
    if (!g_CommandLine.scriptName.empty())
    {
        fs::path p(g_CommandLine.scriptName);
        if (p.extension().string().empty())
        {
            g_CommandLine.scriptName += ".swgs";
        }
        else if (p.extension().string() != ".swgs")
        {
            g_Log.error(Utf8::format(Msg0319, p.extension().string().c_str()));
            return false;
        }
    }

    return true;
}