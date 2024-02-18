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
    error_code err;
    if (!exists(cacheWorkspace, err) && !create_directories(cacheWorkspace, err))
    {
        Report::errorOS(FMT(Err(Fat0016), cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    cacheWorkspace.append(name);
    if (!exists(cacheWorkspace, err) && !create_directories(cacheWorkspace, err))
    {
        Report::errorOS(FMT(Err(Fat0016), cacheWorkspace.c_str()));
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
        Report::error(FMT(Err(Fat0031), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    // Script filename
    Path pathF               = g_CommandLine.scriptName;
    pathF                    = absolute(pathF);
    g_CommandLine.scriptName = pathF.string();
    error_code err;
    if (!filesystem::exists(g_CommandLine.scriptName.c_str(), err))
    {
        Report::error(FMT(Err(Fat0030), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    g_Workspace->setupCachePath();
    if (!exists(g_Workspace->cachePath, err))
    {
        Report::error(FMT(Err(Fat0010), g_Workspace->cachePath.string().c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_Workspace->build();
}
