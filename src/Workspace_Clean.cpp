#include "pch.h"
#include "CommandLine.h"
#include "Diagnostic.h"
#include "Os.h"
#include "Report.h"
#include "Workspace.h"

void Workspace::cleanFolderContent(const Path& path)
{
    if (g_CommandLine.cleanLog)
        return;

    OS::visitFilesRec(path.string().c_str(),
                      [&](const char* cFileName)
                      {
                          std::error_code err;
                          if (filesystem::remove_all(cFileName, err) == static_cast<std::uintmax_t>(-1))
                          {
                              Report::errorOS(Fmt(Err(Fat0023), cFileName));
                              OS::exit(-1);
                          }
                      });

    std::error_code err;
    if (filesystem::remove_all(path, err) == static_cast<std::uintmax_t>(-1))
    {
        Report::errorOS(Fmt(Err(Fat0022), path.string().c_str()));
        OS::exit(-1);
    }
}

void Workspace::cleanPublic(const Path& basePath)
{
    error_code err;
    if (filesystem::exists(basePath, err))
    {
        OS::visitFolders(basePath.string().c_str(),
                         [&, this](const char* folder)
                         {
                             auto path = basePath;
                             path.append(folder);
                             path.append(SWAG_PUBLIC_FOLDER);
                             error_code err;
                             if (filesystem::exists(path, err))
                             {
                                 // Clean all targets
                                 OS::visitFolders(path.string().c_str(),
                                                  [&, this](const char* folder)
                                                  {
                                                      auto cfgpath = path;
                                                      cfgpath.append(folder);
                                                      error_code err;
                                                      if (filesystem::exists(path, err))
                                                      {
                                                          g_Log.messageHeaderCentered("Cleaning", cfgpath.string().c_str());
                                                          cleanFolderContent(cfgpath);
                                                      }
                                                  });

                                 // Clean public folder itself
                                 g_Log.messageHeaderCentered("Cleaning", path.string().c_str());
                                 cleanFolderContent(path);
                             }
                         });
    }
}

void Workspace::cleanScript(bool all)
{
    setupCachePath();
    error_code err;
    if (!filesystem::exists(g_Workspace->cachePath, err))
        return;
    auto cacheFolder = g_Workspace->cachePath;
    cacheFolder.append(SWAG_CACHE_FOLDER);
    if (!filesystem::exists(cacheFolder, err))
        return;

    Utf8 pathToClean;
    if (all)
        pathToClean = SWAG_SCRIPT_WORKSPACE;
    else
        pathToClean = workspacePath.filename().string();

    // Clean all folders of the form 'SWAG_SCRIPT_WORKSPACE-??'
    OS::visitFolders(
        cacheFolder.string().c_str(), [&](const char* folder)
        {
            auto path = cacheFolder;
            path.append(folder);
            g_Log.messageHeaderCentered("Cleaning", path.string().c_str());
            cleanFolderContent(path); },
        Fmt("%s-*", SWAG_SCRIPT_WORKSPACE).c_str());
}

void Workspace::cleanCommand()
{
    // Clear scripts cache
    if (g_CommandLine.scriptMode)
    {
        cleanScript(true);
        g_Log.messageHeaderCentered("Done", "");
        return;
    }

    // Clear a specific workspace cache
    setup();

    // Clean all output folders
    targetPath = workspacePath;
    targetPath.append(SWAG_OUTPUT_FOLDER);
    error_code err;
    if (filesystem::exists(targetPath, err))
    {
        OS::visitFolders(targetPath.string().c_str(),
                         [this](const char* folder)
                         {
                             auto path = targetPath;
                             path.append(folder);
                             g_Log.messageHeaderCentered("Cleaning", path.string().c_str());
                             cleanFolderContent(path);
                         });
    }

    // Clean all cache folders for the given workspace
    setupCachePath();
    cachePath.append(SWAG_CACHE_FOLDER);
    if (filesystem::exists(cachePath, err))
    {
        OS::visitFolders(cachePath.string().c_str(),
                         [this](const char* folder)
                         {
                             auto wkPath = workspacePath.filename().string() + "-";

                             // We sure to only clean for the given workspace
                             if (strstr(folder, wkPath.c_str()) == folder)
                             {
                                 auto path = cachePath;
                                 path.append(folder);
                                 g_Log.messageHeaderCentered("Cleaning", path.string().c_str());
                                 cleanFolderContent(path);
                             }
                         });
    }

    // Clean all public folders of the workspace modules
    cleanPublic(modulesPath);
    cleanPublic(testsPath);

    // Clean the full content of the dependency path
    if (g_CommandLine.cleanDep)
    {
        if (filesystem::exists(dependenciesPath, err))
        {
            auto path = dependenciesPath;
            g_Log.messageHeaderCentered("Cleaning", path.string().c_str());
            if (!g_CommandLine.cleanLog)
                cleanFolderContent(dependenciesPath);
        }
    }

    // Otherwise just clean tmp files in it
    else
        cleanPublic(dependenciesPath);

    g_Log.messageHeaderCentered("Done", "");
}
