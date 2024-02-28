#include "pch.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "Os.h"
#include "Report.h"
#include "Workspace.h"

void Workspace::setScriptWorkspace(const Utf8& name)
{
    // Cache directory
    setupCachePath();

    // Compute workspace folder and name
    // Will be shared between all scripts, in the cache folder
    auto cacheWorkspace = g_Workspace->cachePath;
    cacheWorkspace.append(SWAG_CACHE_FOLDER);
    std::error_code err;
    if (!std::filesystem::exists(cacheWorkspace, err) && !std::filesystem::create_directories(cacheWorkspace, err))
    {
        Report::errorOS(formErr(Fat0016, cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    cacheWorkspace.append(name);
    if (!std::filesystem::exists(cacheWorkspace, err) && !std::filesystem::create_directories(cacheWorkspace, err))
    {
        Report::errorOS(formErr(Fat0016, cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_CommandLine.workspacePath = cacheWorkspace;
    setupPaths();

    // Now that we have the workspace name, we can clean dependencies and temp
    if (g_CommandLine.cleanDep)
        cleanScript();
}

void Workspace::scriptCommand()
{
    if (g_CommandLine.scriptName.empty())
    {
        Report::error(formErr(Fat0031, g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    // Script filename
    Path pathF               = g_CommandLine.scriptName;
    pathF                    = std::filesystem::absolute(pathF);
    g_CommandLine.scriptName = pathF;
    std::error_code err;
    if (!std::filesystem::exists(g_CommandLine.scriptName.c_str(), err))
    {
        Report::error(formErr(Fat0030, g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    g_Workspace->setupCachePath();
    if (!std::filesystem::exists(g_Workspace->cachePath, err))
    {
        Report::error(formErr(Fat0010, g_Workspace->cachePath.c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_Workspace->build();
}
