#include "pch.h"
#include "CommandLine.h"
#include "Backend.h"
#include "Log.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"

CommandLine g_CommandLine;

bool CommandLine::check()
{
    // Stack
    stackSizeRT = (uint32_t) Allocator::alignSize(stackSizeRT);
    if (stackSizeRT < SWAG_LIMIT_MIN_STACK || stackSizeRT > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(Fmt(Err(Fat0030), Utf8::toNiceSize(stackSizeRT).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    stackSizeBC = (uint32_t) Allocator::alignSize(stackSizeBC);
    if (stackSizeBC < SWAG_LIMIT_MIN_STACK || stackSizeBC > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(Fmt(Err(Fat0030), Utf8::toNiceSize(stackSizeBC).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    if (rebuildAll)
        rebuild = true;

    // Force verbose
    if (verboseCmdLine || verbosePath || verboseLink || verboseTestErrors || verboseConcreteTypes || verboseStages)
        verbose = true;

    // Check special backend SCBE
    if (backendGenType == BackendGenType::SCBE)
    {
        if (target.os != SwagTargetOs::Windows)
        {
            Report::error(Fmt(Err(Fat0038), Backend::getOsName(target)));
            return false;
        }

        if (target.arch != SwagTargetArch::X86_64)
        {
            Report::error(Fmt(Err(Fat0037), Backend::getArchName(target)));
            return false;
        }
    }

    // Make some paths canonical
    if (!workspacePath.empty())
    {
        workspacePath = filesystem::absolute(workspacePath);
        error_code err;
        auto       workspacePath1 = filesystem::canonical(workspacePath, err);
        if (!err)
            workspacePath = workspacePath1;
    }

    if (!cachePath.empty())
    {
        cachePath = filesystem::absolute(cachePath);
        error_code err;
        auto       cachePath1 = filesystem::canonical(cachePath, err);
        if (!err)
            cachePath = cachePath1;
    }

    // Add/check script file extension
    if (!g_CommandLine.scriptName.empty())
    {
        Path p = g_CommandLine.scriptName;
        if (p.extension().string().empty())
        {
            g_CommandLine.scriptName += ".swgs";
        }
        else if (p.extension().string() != ".swgs")
        {
            Report::error(Fmt(Err(Fat0013), p.extension().string().c_str()));
            return false;
        }
    }

    return true;
}