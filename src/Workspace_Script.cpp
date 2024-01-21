#include "pch.h"
#include "Workspace.h"
#include "Os.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"
#include "CommandLine.h"

void Workspace::setScriptWorkspace(const Utf8& name)
{
    // Cache directory
    setupCachePath();

    // Compute workspace folder and name
    // Will be shared between all scripts, in the cache folder
    auto cacheWorkspace = g_Workspace->cachePath;
    cacheWorkspace.append(SWAG_CACHE_FOLDER);
    error_code err;
    if (!filesystem::exists(cacheWorkspace, err) && !filesystem::create_directories(cacheWorkspace, err))
    {
        Report::errorOS(Fmt(Err(Fat0016), cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    cacheWorkspace.append(name.c_str());
    if (!filesystem::exists(cacheWorkspace, err) && !filesystem::create_directories(cacheWorkspace, err))
    {
        Report::errorOS(Fmt(Err(Fat0016), cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_CommandLine.workspacePath = cacheWorkspace;
    setupPaths();

    // Now that we have the workspace name, we can clean dependencies and temp
    if (g_CommandLine.cleanDep)
        cleanScript(false);
}

void Workspace::scriptCommand()
{
    if (g_CommandLine.scriptName.empty())
    {
        Report::error(Fmt(Err(Fat0031), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    // Script filename
    Path pathF               = g_CommandLine.scriptName;
    pathF                    = filesystem::absolute(pathF);
    g_CommandLine.scriptName = pathF.string();
    error_code err;
    if (!filesystem::exists(g_CommandLine.scriptName.c_str(), err))
    {
        Report::error(Fmt(Err(Fat0030), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    g_Workspace->setupCachePath();
    if (!filesystem::exists(g_Workspace->cachePath, err))
    {
        Report::error(Fmt(Err(Fat0010), g_Workspace->cachePath.string().c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_Workspace->build();
}
