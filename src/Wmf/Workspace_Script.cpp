#include "pch.h"
#include "Main/CommandLine.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Wmf/Workspace.h"

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
        Report::errorOS(formErr(Fat0016, cacheWorkspace.cstr()));
        OS::exit(-1);
    }

    cacheWorkspace.append(name);
    if (!std::filesystem::exists(cacheWorkspace, err) && !std::filesystem::create_directories(cacheWorkspace, err))
    {
        Report::errorOS(formErr(Fat0016, cacheWorkspace.cstr()));
        OS::exit(-1);
    }

    // This is it. Build and run!
    g_CommandLine.workspacePath = cacheWorkspace;
    setupPaths();

    // Now that we have the workspace name, we can clean dependencies and temp
    if (g_CommandLine.cleanDep)
        cleanScript();
}

void Workspace::scriptCommand()
{
    if (g_CommandLine.fileName.empty())
    {
        Report::error(formErr(Fat0031, g_CommandLine.fileName.cstr()));
        OS::exit(-1);
    }

    // Script filename
    Path pathF             = g_CommandLine.fileName;
    pathF                  = std::filesystem::absolute(pathF);
    g_CommandLine.fileName = pathF;
    std::error_code err;
    if (!std::filesystem::exists(g_CommandLine.fileName.cstr(), err))
    {
        Report::error(formErr(Fat0030, g_CommandLine.fileName.cstr()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    g_Workspace->setupCachePath();
    if (!std::filesystem::exists(g_Workspace->cachePath, err))
    {
        Report::error(formErr(Fat0010, g_Workspace->cachePath.cstr()));
        OS::exit(-1);
    }

    // This is it. Build and run!
    g_Workspace->build();
}
