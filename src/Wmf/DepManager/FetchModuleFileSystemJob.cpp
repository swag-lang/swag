#include "pch.h"
#include "FetchModuleFileSystemJob.h"
#include "Jobs/FileJob.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

FetchModuleFileSystemJob::FetchModuleFileSystemJob()
{
    addFlag(JOB_IS_IO);
}

JobResult FetchModuleFileSystemJob::execute()
{
    const auto dep = module->fetchDep;

    const auto depName = form("%s %u.%d.%d",
                              dep->name.cstr(),
                              dep->module->buildCfg.moduleVersion,
                              dep->module->buildCfg.moduleRevision,
                              dep->module->buildCfg.moduleBuildNum);
    if (collectSourceFiles)
        g_Log.messageHeaderCentered("Copying", depName.cstr());
    else
        g_Log.messageHeaderCentered("Using", depName.cstr());

    // Collect list of source files
    // Collect the module config file, and everything in src/ and publish/
    SetUtf8 srcFiles;
    OS::visitFilesRec(dep->resolvedLocation, [&](const char* fileName) {
        const Path n    = fileName;
        const Utf8 subN = fileName + dep->resolvedLocation.length() + 1;

        if (collectSourceFiles)
        {
            Path cfgFile = dep->resolvedLocation;
            cfgFile.append(SWAG_CFG_FILE);
            if (cfgFile == fileName)
            {
                srcFiles.insert(subN);
                return;
            }

            Path srcFolder = dep->resolvedLocation;
            srcFolder.append(SWAG_SRC_FOLDER);
            if (n.find(srcFolder) == 0)
            {
                srcFiles.insert(subN);
                return;
            }
        }

        Path publishFolder = dep->resolvedLocation;
        publishFolder.append(SWAG_PUBLISH_FOLDER);
        if (n.find(publishFolder) == 0)
        {
            srcFiles.insert(subN);
            return;
        }
    });

    auto destPath = g_Workspace->dependenciesPath;
    destPath.append(dep->name.cstr());

    // Collect list of already existing files in the dependency folder, to remove old ones if necessary
    Vector<Utf8> dstFiles;
    OS::visitFilesRec(destPath, [&](const char* fileName) {
        const Path n    = fileName;
        const Utf8 subN = fileName + destPath.length() + 1;

        // Do not collect public folder
        Path publicFolder = dep->resolvedLocation;
        publicFolder.append(SWAG_PUBLIC_FOLDER);
        if (n.find(publicFolder) == 0)
            return;

        dstFiles.push_back(subN);
    });

    // Remove all files in the dependency folder that are no more in the module source folder
    for (const auto& f : dstFiles)
    {
        if (!srcFiles.contains(f))
        {
            if (f == SWAG_ALIAS_FILENAME)
                continue;

            auto n = destPath;
            n.append(f.cstr());

            if (!std::filesystem::remove(n))
            {
                Report::errorOS(formErr(Err0065, n.cstr()));
                return JobResult::ReleaseJob;
            }
        }
    }

    // Copy all collected files
    for (auto& f : srcFiles)
    {
        auto srcFileName = dep->resolvedLocation;
        srcFileName.append(f.cstr());
        auto destFileName = destPath;
        destFileName.append(f.cstr());

        auto            folder = destFileName.parent_path();
        std::error_code err;
        if (!std::filesystem::exists(folder, err) && !std::filesystem::create_directories(folder, err))
        {
            Report::errorOS(formErr(Err0052, folder.cstr()));
            return JobResult::ReleaseJob;
        }

        const auto job  = Allocator::alloc<CopyFileJob>();
        job->module     = module;
        job->sourcePath = srcFileName;
        job->destPath   = destFileName;
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
