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
    error_code errorCode;
    if (!fs::exists(cacheWorkspace) && !fs::create_directories(cacheWorkspace, errorCode))
    {
        Report::errorOS(Fmt(Err(Fat0003), cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    cacheWorkspace += "/";
    cacheWorkspace.append(name.c_str());
    if (!fs::exists(cacheWorkspace) && !fs::create_directories(cacheWorkspace, errorCode))
    {
        Report::errorOS(Fmt(Err(Fat0003), cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_CommandLine.workspacePath = cacheWorkspace.string();
    setupPaths();

    // Now that we have the workspace name, we can clean dependencies and temp
    if (g_CommandLine.cleanDep)
        cleanScript(false);
}

void Workspace::scriptCommand()
{
    if (g_CommandLine.scriptName.empty())
    {
        Report::error(Fmt(Err(Fat0017), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    // Script filename
    fs::path pathF            = fs::absolute(g_CommandLine.scriptName).string();
    g_CommandLine.scriptName = Utf8::normalizePath(pathF.string());
    if (!fs::exists(g_CommandLine.scriptName))
    {
        Report::error(Fmt(Err(Fat0020), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    g_Workspace->setupCachePath();
    if (!fs::exists(g_Workspace->cachePath))
    {
        Report::error(Fmt(Err(Fat0002), g_Workspace->cachePath.string().c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_Workspace->build();
}
