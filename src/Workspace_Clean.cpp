#include "pch.h"
#include "Workspace.h"
#include "Os.h"
#include "ErrorIds.h"

void Workspace::cleanFolderContent(const fs::path& path)
{
    if (g_CommandLine->cleanLog)
        return;

    OS::visitFilesFolders(path.string().c_str(), [&](uint64_t, const char* cFileName, bool)
                          {
                              auto folder = path.string() + "/";
                              folder += cFileName;

                              std::error_code err;
                              if (fs::remove_all(folder, err) == -1)
                              {
                                  g_Log.errorOS(Utf8::format(g_E[Err0344], folder.c_str()));
                                  OS::exit(-1);
                              }
                          });

    std::error_code err;
    if (fs::remove_all(path, err) == -1)
    {
        g_Log.errorOS(Utf8::format(g_E[Err0345], path.string().c_str()));
        OS::exit(-1);
    }
}

void Workspace::cleanPublic(const fs::path& basePath)
{
    if (fs::exists(basePath))
    {
        OS::visitFolders(basePath.string().c_str(), [&, this](const char* folder)
                         {
                             auto path = basePath.string() + folder;
                             path += "/";
                             path += SWAG_PUBLIC_FOLDER;
                             path += "/";
                             path = Utf8::normalizePath(path);
                             if (fs::exists(path))
                             {
                                 // Clean all targets
                                 OS::visitFolders(path.c_str(), [&, this](const char* folder)
                                                  {
                                                      auto cfgpath = path + folder;
                                                      cfgpath      = Utf8::normalizePath(cfgpath);
                                                      if (fs::exists(path))
                                                      {
                                                          g_Log.messageHeaderCentered("Cleaning", cfgpath);
                                                          cleanFolderContent(cfgpath);
                                                      }
                                                  });

                                 // Clean public folder itself
                                 g_Log.messageHeaderCentered("Cleaning", path);
                                 cleanFolderContent(path);
                             }
                         });
    }
}

void Workspace::cleanCommand()
{
    // Clear scripts cache
    if (g_CommandLine->scriptMode)
    {
        setupCachePath();
        if (!fs::exists(g_Workspace->cachePath))
            return;
        auto cacheFolder = g_Workspace->cachePath.string();
        cacheFolder.append(SWAG_CACHE_FOLDER);
        if (!fs::exists(cacheFolder))
            return;

        // Clean all folders of the form 'SWAG_SCRIPT_WORKSPACE-??'
        OS::visitFolders(
            cacheFolder.c_str(), [&](const char* folder)
            {
                auto path = cacheFolder + "/";
                path.append(folder);
                path = Utf8::normalizePath(path);
                g_Log.messageHeaderCentered("Cleaning", path);
                cleanFolderContent(path);
            },
            Utf8::format("%s-*", SWAG_SCRIPT_WORKSPACE).c_str());
    }
    else
    {
        setup();

        // Clean all output folders
        targetPath = workspacePath;
        targetPath.append(SWAG_OUTPUT_FOLDER);
        targetPath.append("/");
        if (fs::exists(targetPath))
        {
            OS::visitFolders(targetPath.string().c_str(), [this](const char* folder)
                             {
                                 auto path = targetPath.string() + folder;
                                 path      = Utf8::normalizePath(path);
                                 g_Log.messageHeaderCentered("Cleaning", path);
                                 cleanFolderContent(path);
                             });
        }

        // Clean all cache folders for the given workspace
        setupCachePath();
        cachePath.append(SWAG_CACHE_FOLDER);
        cachePath.append("/");
        if (fs::exists(cachePath))
        {
            OS::visitFolders(cachePath.string().c_str(), [this](const char* folder)
                             {
                                 auto wkPath = workspacePath.filename().string() + "-";

                                 // We sure to only clean for the given workspace
                                 if (strstr(folder, wkPath.c_str()) == folder)
                                 {
                                     auto path = cachePath.string() + folder;
                                     path      = Utf8::normalizePath(path);
                                     g_Log.messageHeaderCentered("Cleaning", path);
                                     cleanFolderContent(path);
                                 }
                             });
        }

        // Clean all public folders of the workspace modules
        cleanPublic(modulesPath);
        cleanPublic(examplesPath);
        cleanPublic(testsPath);

        // Clean the full content of the dependency path
        if (g_CommandLine->cleanDep)
        {
            if (fs::exists(dependenciesPath))
            {
                auto path = Utf8::normalizePath(dependenciesPath.string());
                g_Log.messageHeaderCentered("Cleaning", path);
                if (!g_CommandLine->cleanLog)
                    fs::remove_all(dependenciesPath);
            }
        }

        // Otherwise just clean tmp files in it
        else
            cleanPublic(dependenciesPath);
    }

    g_Log.messageHeaderCentered("Done", "");
}
