#include "pch.h"
#include "Module.h"
#include "FetchModuleJobFileSystem.h"
#include "OS.h"
#include "Workspace.h"
#include "CopyFileJob.h"
#include "ThreadManager.h"
#include "ErrorIds.h"

thread_local Pool<FetchModuleJobFileSystem> g_Pool_moduleFetchJobFileSystem;

JobResult FetchModuleJobFileSystem::execute()
{
    auto dep = module->fetchDep;

    auto depName = format("%s %u.%d.%d", dep->name.c_str(), dep->module->buildCfg.moduleVersion, dep->module->buildCfg.moduleRevision, dep->module->buildCfg.moduleBuildNum);
    g_Log.messageHeaderCentered("Copying", depName.c_str());

    // Collect list of source files
    set<string> srcFiles;
    OS::visitFilesRec(dep->resolvedLocation, [&](const char* fileName) {
        auto n = normalizePath(fileName + dep->resolvedLocation.length());

        // Do not collect public folder
        if (strstr(n.c_str(), SWAG_PUBLIC_FOLDER) == n.c_str() + 1)
            return;

        srcFiles.insert(n);
    });

    auto destPath = g_Workspace.dependenciesPath.string();
    destPath += dep->name.c_str();

    // Collect list of dest files if they exist, in order to remove old ones
    vector<string> dstFiles;
    OS::visitFilesRec(destPath.c_str(), [&](const char* fileName) {
        auto n = normalizePath(fileName + destPath.length());

        // Do not collect public folder
        if (strstr(n.c_str(), SWAG_PUBLIC_FOLDER) == n.c_str() + 1)
            return;

        dstFiles.push_back(n);
    });

    // Remove all files in dest folder that are no more in source folder
    for (auto f : dstFiles)
    {
        if (srcFiles.find(f) == srcFiles.end())
        {
            auto n = destPath + f;
            if (!fs::remove(n))
            {
                g_Log.errorOS(format(Msg0603, n.c_str()));
                return JobResult::ReleaseJob;
            }
        }
    }

    // Copy all files
    for (auto& f : srcFiles)
    {
        fs::path srcFileName  = string(dep->resolvedLocation) + f;
        fs::path destFileName = destPath + f;

        auto folder = destFileName.parent_path();
        if (!fs::exists(folder) && !fs::create_directories(folder))
        {
            g_Log.errorOS(format(Msg0604, folder.c_str()));
            return JobResult::ReleaseJob;
        }

        auto job        = g_Pool_copyFileJob.alloc();
        job->module     = module;
        job->sourcePath = srcFileName.string();
        job->destPath   = destFileName.string();
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}