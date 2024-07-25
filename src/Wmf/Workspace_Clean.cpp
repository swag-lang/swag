#include "pch.h"
#include "Main/CommandLine.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Wmf/Workspace.h"

void Workspace::cleanFolderContent(const Path& path)
{
    if (g_CommandLine.cleanLog)
        return;

    OS::visitFilesRec(path.c_str(), [&](const char* cFileName) {
        std::error_code err;
        if (std::filesystem::remove_all(cFileName, err) == static_cast<std::uintmax_t>(-1))
        {
            Report::errorOS(formErr(Fat0023, cFileName));
            OS::exit(-1);
        }
    });

    std::error_code err;
    if (std::filesystem::remove_all(path, err) == static_cast<std::uintmax_t>(-1))
    {
        Report::errorOS(formErr(Fat0022, path.c_str()));
        OS::exit(-1);
    }
}

void Workspace::cleanPublic(const Path& basePath)
{
    std::error_code err;
    if (std::filesystem::exists(basePath, err))
    {
        OS::visitFolders(basePath, [&basePath](const char* folder) {
            auto path = basePath;
            path.append(folder);
            path.append(SWAG_PUBLIC_FOLDER);
            std::error_code inErr;
            if (std::filesystem::exists(path, inErr))
            {
                // Clean all targets
                OS::visitFolders(path.c_str(), [&path](const char* inFolder) {
                    auto cfgpath = path;
                    cfgpath.append(inFolder);
                    std::error_code inErr1;
                    if (std::filesystem::exists(path, inErr1))
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

void Workspace::cleanScript()
{
    setupCachePath();
    std::error_code err;
    if (!std::filesystem::exists(g_Workspace->cachePath, err))
        return;
    auto cacheFolder = g_Workspace->cachePath;
    cacheFolder.append(SWAG_CACHE_FOLDER);
    if (!std::filesystem::exists(cacheFolder, err))
        return;

    // Clean all folders of the form 'SWAG_SCRIPT_WORKSPACE-??'
    OS::visitFolders(cacheFolder, [&](const char* folder) {
                         auto path = cacheFolder;
                         path.append(folder);
                         g_Log.messageHeaderCentered("Cleaning", path);
                         cleanFolderContent(path); }, form("%s-*", SWAG_SCRIPT_WORKSPACE).c_str());
}

void Workspace::cleanCommand()
{
    // Clear scripts cache
    if (g_CommandLine.scriptMode)
    {
        cleanScript();
        g_Log.messageHeaderCentered("Done", "");
        return;
    }

    // Clear a specific workspace cache
    setup();

    // Clean all output folders
    targetPath = workspacePath;
    targetPath.append(SWAG_OUTPUT_FOLDER);
    std::error_code err;
    if (std::filesystem::exists(targetPath, err))
    {
        OS::visitFolders(targetPath, [this](const char* folder) {
            auto path = targetPath;
            path.append(folder);
            g_Log.messageHeaderCentered("Cleaning", path);
            cleanFolderContent(path);
        });
    }

    // Clean all cache folders for the given workspace
    setupCachePath();
    cachePath.append(SWAG_CACHE_FOLDER);
    if (std::filesystem::exists(cachePath, err))
    {
        OS::visitFolders(cachePath, [this](const char* folder) {
            const auto wkPath = workspacePath.filename() + "-";

            // We sure to only clean for the given workspace
            if (strstr(folder, wkPath.c_str()) == folder)
            {
                auto path = cachePath;
                path.append(folder);
                g_Log.messageHeaderCentered("Cleaning", path);
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
        if (std::filesystem::exists(dependenciesPath, err))
        {
            const auto path = dependenciesPath;
            g_Log.messageHeaderCentered("Cleaning", path);
            if (!g_CommandLine.cleanLog)
                cleanFolderContent(dependenciesPath);
        }
    }

    // Otherwise just clean tmp files in it
    else
        cleanPublic(dependenciesPath);

    g_Log.messageHeaderCentered("Done", "");
}
