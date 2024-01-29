#include "pch.h"
#include "FetchModuleFileSystemJob.h"
#include "ErrorIds.h"
#include "FileJob.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"
#include "ThreadManager.h"
#include "Workspace.h"

JobResult FetchModuleFileSystemJob::execute()
{
    auto dep = module->fetchDep;

    auto depName = Fmt("%s %u.%d.%d",
                       dep->name.c_str(),
                       dep->module->buildCfg.moduleVersion,
                       dep->module->buildCfg.moduleRevision,
                       dep->module->buildCfg.moduleBuildNum);
    if (collectSourceFiles)
        g_Log.messageHeaderCentered("Copying", depName.c_str());
    else
        g_Log.messageHeaderCentered("Using", depName.c_str());

    // Collect list of source files
    // Collect the module config file, and everything in src/ and publish/
    SetUtf8 srcFiles;
    OS::visitFilesRec(dep->resolvedLocation.string().c_str(),
                      [&](const char* fileName)
                      {
                          Path n    = fileName;
                          Utf8 subN = fileName + dep->resolvedLocation.string().length() + 1;

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
                              if (n.string().find(srcFolder.string()) == 0)
                              {
                                  srcFiles.insert(subN);
                                  return;
                              }
                          }

                          Path publishFolder = dep->resolvedLocation;
                          publishFolder.append(SWAG_PUBLISH_FOLDER);
                          if (n.string().find(publishFolder.string()) == 0)
                          {
                              srcFiles.insert(subN);
                              return;
                          }
                      });

    auto destPath = g_Workspace->dependenciesPath;
    destPath.append(dep->name.c_str());

    // Collect list of already existing files in the dependency folder, in order to remove old ones if necessary
    Vector<Utf8> dstFiles;
    OS::visitFilesRec(destPath.string().c_str(),
                      [&](const char* fileName)
                      {
                          Path n    = fileName;
                          Utf8 subN = fileName + destPath.string().length() + 1;

                          // Do not collect public folder
                          Path publicFolder = dep->resolvedLocation;
                          publicFolder.append(SWAG_PUBLIC_FOLDER);
                          if (n.string().find(publicFolder.string()) == 0)
                              return;

                          dstFiles.push_back(subN);
                      });

    // Remove all files in the dependency folder that are no more in the module source folder
    for (const auto& f : dstFiles)
    {
        if (srcFiles.find(f) == srcFiles.end())
        {
            if (f == SWAG_ALIAS_FILENAME)
                continue;

            auto n = destPath;
            n.append(f.c_str());

            if (!filesystem::remove(n))
            {
                Report::errorOS(Fmt(Err(Err0090), n.string().c_str()));
                return JobResult::ReleaseJob;
            }
        }
    }

    // Copy all collected files
    for (auto& f : srcFiles)
    {
        auto srcFileName = dep->resolvedLocation;
        srcFileName.append(f.c_str());
        auto destFileName = destPath;
        destFileName.append(f.c_str());

        auto       folder = destFileName.parent_path();
        error_code err;
        if (!filesystem::exists(folder, err) && !filesystem::create_directories(folder, err))
        {
            Report::errorOS(Fmt(Err(Err0100), folder.string().c_str()));
            return JobResult::ReleaseJob;
        }

        auto job        = Allocator::alloc<CopyFileJob>();
        job->module     = module;
        job->sourcePath = srcFileName;
        job->destPath   = destFileName;
        g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}