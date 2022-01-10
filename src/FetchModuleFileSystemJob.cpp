#include "pch.h"
#include "Module.h"
#include "FetchModuleFileSystemJob.h"
#include "Workspace.h"
#include "CopyFileJob.h"
#include "ThreadManager.h"
#include "ErrorIds.h"
#include "Os.h"
#include "Diagnostic.h"

JobResult FetchModuleFileSystemJob::execute()
{
    auto dep = module->fetchDep;

    auto depName = Fmt("%s %u.%d.%d", dep->name.c_str(), dep->module->buildCfg.moduleVersion, dep->module->buildCfg.moduleRevision, dep->module->buildCfg.moduleBuildNum);
    if (collectSourceFiles)
        g_Log.messageHeaderCentered("Copying", depName.c_str());
    else
        g_Log.messageHeaderCentered("Linking", depName.c_str());

    // Collect list of source files
    set<string> srcFiles;
    OS::visitFilesRec(dep->resolvedLocation, [&](const char* fileName)
                      {
                          auto n = Utf8::normalizePath(fileName + dep->resolvedLocation.length());

                          // Collect the module config file, and everything in src/ and publish/
                          Utf8 cfgFile = "/";
                          cfgFile += SWAG_CFG_FILE;

                          if (collectSourceFiles && n == cfgFile)
                              srcFiles.insert(n);
                          else if (strstr(n.c_str(), SWAG_PUBLISH_FOLDER) == n.c_str() + 1)
                              srcFiles.insert(n);
                          else if (collectSourceFiles && strstr(n.c_str(), SWAG_SRC_FOLDER) == n.c_str() + 1)
                              srcFiles.insert(n);
                      });

    auto destPath = g_Workspace->dependenciesPath.string();
    destPath += dep->name.c_str();

    // Collect list of dest files if they exist, in order to remove old ones
    vector<string> dstFiles;
    OS::visitFilesRec(destPath.c_str(), [&](const char* fileName)
                      {
                          auto n = Utf8::normalizePath(fileName + destPath.length());

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
            if (strstr(f.c_str(), SWAG_ALIAS_FILENAME) == f.c_str() + 1)
                continue;
            if (!fs::remove(n))
            {
                g_Log.errorOS(Fmt(Err(Err0603), n.c_str()));
                return JobResult::ReleaseJob;
            }
        }
    }

    // Copy all files
    error_code errorCode;
    for (auto& f : srcFiles)
    {
        fs::path srcFileName  = string(dep->resolvedLocation) + f;
        fs::path destFileName = destPath + f;

        auto folder = destFileName.parent_path();
        if (!fs::exists(folder) && !fs::create_directories(folder, errorCode))
        {
            g_Log.errorOS(Fmt(Err(Err0604), folder.c_str()));
            return JobResult::ReleaseJob;
        }

        auto job        = g_Allocator.alloc<CopyFileJob>();
        job->module     = module;
        job->sourcePath = srcFileName.string();
        job->destPath   = destFileName.string();
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}