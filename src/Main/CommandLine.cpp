#include "pch.h"
#include "Backend/Backend.h"
#include "Main/CommandLine.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"

CommandLine g_CommandLine;

bool CommandLine::check()
{
#ifdef SWAG_DEV_MODE
    dbgDevMode = true;
#endif

    // Stack
    limitStackRT = Allocator::alignSize(limitStackRT);
    if (limitStackRT < SWAG_LIMIT_MIN_STACK || limitStackRT > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(formErr(Fat0006, Utf8::toNiceSize(limitStackRT).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    limitStackBC = Allocator::alignSize(limitStackBC);
    if (limitStackBC < SWAG_LIMIT_MIN_STACK || limitStackBC > SWAG_LIMIT_MAX_STACK)
    {
        Report::error(formErr(Fat0006, Utf8::toNiceSize(limitStackBC).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MIN_STACK).c_str(), Utf8::toNiceSize(SWAG_LIMIT_MAX_STACK).c_str()));
        return false;
    }

    if (rebuildAll)
        rebuild = true;

    // Force verbose
    if (verboseCmdLine || verbosePath || verboseLink || verboseErrors || verboseConcreteTypes || verboseStages)
        verbose = true;

    // Check special backend SCBE
    if (backendGenType == BackendGenType::SCBE)
    {
        if (target.os != SwagTargetOs::Windows)
        {
            Report::error(formErr(Fat0009, Backend::getOsName(target)));
            return false;
        }

        if (target.arch != SwagTargetArch::X86_64)
        {
            Report::error(formErr(Fat0008, Backend::getArchName(target)));
            return false;
        }
    }

    // Make some paths canonical
    if (!workspacePath.empty())
    {
        workspacePath = std::filesystem::absolute(workspacePath);
        std::error_code err;
        const auto      workspacePath1 = std::filesystem::canonical(workspacePath, err);
        if (!err)
            workspacePath = workspacePath1;
    }

    if (!cachePath.empty())
    {
        cachePath = std::filesystem::absolute(cachePath);
        std::error_code err;
        const auto      cachePath1 = std::filesystem::canonical(cachePath, err);
        if (!err)
            cachePath = cachePath1;
    }

    // Add/check script file extension
    if (g_CommandLine.scriptCommand && !g_CommandLine.fileName.empty())
    {
        const Path p = g_CommandLine.fileName;
        if (p.extension().empty())
        {
            g_CommandLine.fileName += ".swgs";
        }
        else if (p.extension() != ".swgs")
        {
            Report::error(formErr(Fat0025, p.extension().c_str()));
            return false;
        }
    }

    return true;
}
