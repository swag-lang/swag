#include "pch.h"
#include "CommandLine.h"
#include "Backend.h"
#include "ErrorIds.h"
#include "Log.h"
#include "Diagnostic.h"

CommandLine* g_CommandLine = nullptr;

bool CommandLine::check()
{
    // Stack
    stackSizeRT = (uint32_t) Allocator::alignSize(stackSizeRT);
    if (stackSizeRT < SWAG_LIMIT_MIN_STACK || stackSizeRT > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(Fmt(Err(Err0519), Utf8::toNiceSize(stackSizeRT).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    stackSizeBC = (uint32_t) Allocator::alignSize(stackSizeBC);
    if (stackSizeBC < SWAG_LIMIT_MIN_STACK || stackSizeBC > SWAG_LIMIT_MAX_STACK)
    {
        g_Log.error(Fmt(Err(Err0519), Utf8::toNiceSize(stackSizeBC).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    if (rebuildAll)
        rebuild = true;

    // Force verbose
    if (verboseCmdLine || verbosePath || verboseLink || verboseTestErrors || verboseConcreteTypes || verboseStages)
        verbose = true;

    // Stats
    if (statsWhat != StatsWhat::None)
        stats = true;

    // Check special backend X64
    if (backendGenType == BackendGenType::X64)
    {
        if (target.os != SwagTargetOs::Windows)
        {
            g_Log.error(Fmt(Err(Err0522), Backend::GetOsName(target)));
            return false;
        }

        if (target.arch != SwagTargetArch::X86_64)
        {
            g_Log.error(Fmt(Err(Err0523), Backend::GetArchName(target)));
            return false;
        }
    }

    // Make some paths canonical
    if (!workspacePath.empty())
    {
        workspacePath = fs::absolute(workspacePath).string();
        error_code errorCode;
        auto       workspacePath1 = fs::canonical(workspacePath, errorCode).string();
        if (!errorCode)
            workspacePath = workspacePath1;
    }

    if (!cachePath.empty())
    {
        cachePath = fs::absolute(cachePath).string();
        error_code errorCode;
        auto       cachePath1 = fs::canonical(cachePath).string();
        if (!errorCode)
            cachePath = cachePath1;
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
            g_Log.error(Fmt(Err(Err0319), p.extension().string().c_str()));
            return false;
        }
    }

    return true;
}