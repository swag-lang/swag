#include "pch.h"
#include "CommandLine.h"
#include "Backend.h"
#include "Diagnostic.h"
#include "Report.h"

CommandLine g_CommandLine;

bool CommandLine::check()
{
    // Stack
    limitStackRT = (uint32_t) Allocator::alignSize(limitStackRT);
    if (limitStackRT < SWAG_LIMIT_MIN_STACK || limitStackRT > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(FMT(Err(Fat0006), Utf8::toNiceSize(limitStackRT).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    limitStackBC = (uint32_t) Allocator::alignSize(limitStackBC);
    if (limitStackBC < SWAG_LIMIT_MIN_STACK || limitStackBC > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(FMT(Err(Fat0006), Utf8::toNiceSize(limitStackBC).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
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
            Report::error(FMT(Err(Fat0009), Backend::getOsName(target)));
            return false;
        }

        if (target.arch != SwagTargetArch::X86_64)
        {
            Report::error(FMT(Err(Fat0008), Backend::getArchName(target)));
            return false;
        }
    }

    // Make some paths canonical
    if (!workspacePath.empty())
    {
        workspacePath = filesystem::absolute(workspacePath);
        error_code err;
        const auto workspacePath1 = filesystem::canonical(workspacePath, err);
        if (!err)
            workspacePath = workspacePath1;
    }

    if (!cachePath.empty())
    {
        cachePath = filesystem::absolute(cachePath);
        error_code err;
        const auto cachePath1 = filesystem::canonical(cachePath, err);
        if (!err)
            cachePath = cachePath1;
    }

    // Add/check script file extension
    if (!g_CommandLine.scriptName.empty())
    {
        const Path p = g_CommandLine.scriptName;
        if (p.extension().string().empty())
        {
            g_CommandLine.scriptName += ".swgs";
        }
        else if (p.extension().string() != ".swgs")
        {
            Report::error(FMT(Err(Fat0025), p.extension().string().c_str()));
            return false;
        }
    }

    return true;
}
