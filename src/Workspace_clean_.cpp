#include "pch.h"
#include "Workspace.h"
#include "OS.h"

void Workspace::cleanFolderContent(const fs::path& path)
{
    if (g_CommandLine.cleanLog)
        return;

    OS::visitFiles(path.string().c_str(), [&](const char* cFileName) {
        auto folder = path.string() + "/";
        folder += cFileName;
        try
        {
            fs::remove_all(folder);
        }
        catch (...)
        {
            g_Log.error(format("fatal error: cannot delete file '%s'", folder.c_str()));
            exit(-1);
        }
    });

    fs::remove_all(path);
}

void Workspace::cleanPublic(const fs::path& basePath)
{
    if (fs::exists(basePath))
    {
        OS::visitFolders(basePath.string().c_str(), [&, this](const char* folder) {
            auto path = basePath.string() + folder;
            path += "/";
            path += SWAG_PUBLIC_FOLDER;
            path += "/";
            path = normalizePath(path);
            if (fs::exists(path))
            {
                // Clean all targets
                OS::visitFolders(path.c_str(), [&, this](const char* folder) {
                    auto cfgpath = path + folder;
                    cfgpath      = normalizePath(cfgpath);
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
    setup();

    // Clean all output folders
    targetPath = workspacePath;
    targetPath.append(SWAG_OUTPUT_FOLDER);
    targetPath.append("/");
    if (fs::exists(targetPath))
    {
        OS::visitFolders(targetPath.string().c_str(), [this](const char* folder) {
            auto path = targetPath.string() + folder;
            path      = normalizePath(path);
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
        OS::visitFolders(cachePath.string().c_str(), [this](const char* folder) {
            auto wkPath = workspacePath.filename().string() + "-";

            // We sure to only clean for the given workspace
            if (strstr(folder, wkPath.c_str()) == folder)
            {
                auto path = cachePath.string() + folder;
                path      = normalizePath(path);
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
    if (g_CommandLine.cleanDep)
    {
        if (fs::exists(dependenciesPath))
        {
            auto path = normalizePath(dependenciesPath.string());
            g_Log.messageHeaderCentered("Cleaning", path);
            if (!g_CommandLine.cleanLog)
                fs::remove_all(dependenciesPath);
        }
    }

    // Otherwise just clean tmp files in it
    else
        cleanPublic(dependenciesPath);

    g_Log.messageHeaderCentered("Done", "");
}
