#include "pch.h"
#include "Workspace.h"
#include "OS.h"
#include "ErrorIds.h"

void Workspace::scriptCommand()
{
    if (g_CommandLine.scriptName.empty())
    {
        g_Log.error(Utf8::format(Msg0318, g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    // Script filename
    fs::path pathF           = fs::absolute(g_CommandLine.scriptName).string();
    g_CommandLine.scriptName = Utf8::normalizePath(pathF.string());
    if (!fs::exists(g_CommandLine.scriptName))
    {
        g_Log.error(Utf8::format(Msg0166, g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_CommandLine.run           = true;
    g_CommandLine.scriptMode    = true;
    g_CommandLine.scriptCommand = true;

    // Compute workspace folder and name
    // Will be shared between all scripts, in the cache folder
    g_Workspace.setupCachePath();
    if (!fs::exists(g_Workspace.cachePath))
    {
        g_Log.error(Utf8::format(Msg0546, g_Workspace.cachePath.string().c_str()));
        OS::exit(-1);
    }

    auto cacheWorkspace = g_Workspace.cachePath.string();
    cacheWorkspace.append(SWAG_CACHE_FOLDER);
    error_code errorCode;
    if (!fs::exists(cacheWorkspace) && !fs::create_directories(cacheWorkspace, errorCode))
    {
        g_Log.error(Utf8::format(Msg0547, cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    cacheWorkspace.append("/");
    cacheWorkspace.append(SWAG_SCRIPT_WORKSPACE);
    if (!fs::exists(cacheWorkspace) && !fs::create_directories(cacheWorkspace, errorCode))
    {
        g_Log.error(Utf8::format(Msg0547, cacheWorkspace.c_str()));
        OS::exit(-1);
    }

    // This is it. Build and run !
    g_CommandLine.workspacePath = cacheWorkspace;
    g_Workspace.build();
}
