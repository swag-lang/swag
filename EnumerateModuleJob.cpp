#include "pch.h"
#include "EnumerateModuleJob.h"
#include "Workspace.h"
#include "Os.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "Stats.h"
#include "Allocator.h"
#include "Scope.h"
#include "Module.h"

void EnumerateModuleJob::enumerateFilesInModule(const fs::path& path, Module* theModule)
{
    // Scan source folder
    vector<string> directories;
    directories.push_back(path.string());

    string   tmp, tmp1;
    fs::path modulePath;
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

        OS::visitFilesFolders(tmp.c_str(), [&](uint64_t writeTime, const char* cFileName, bool isFolder) {
            if (isFolder)
            {
                tmp1 = tmp + "/" + cFileName;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    if (g_CommandLine.fileFilter.empty() || strstr(cFileName, g_CommandLine.fileFilter.c_str()))
                    {
                        auto file         = g_Allocator.alloc<SourceFile>();
                        file->fromTests   = theModule->fromTestsFolder;
                        file->name        = cFileName;
                        fs::path pathFile = tmp.c_str();
                        pathFile.append(cFileName);
                        file->path      = normalizePath(pathFile);
                        file->writeTime = writeTime;
                        theModule->addFile(file);

                        auto job        = g_Pool_syntaxJob.alloc();
                        job->sourceFile = file;
                        g_ThreadMgr.addJob(job);
                    }
                }
            }
        });
    }
}

Module* EnumerateModuleJob::addModule(const fs::path& path)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Module name is equivalent to the folder name
    string moduleName;
    if (parent == "tests")
        moduleName = "tests.";
    moduleName += cFileName;

    // Create theModule
    auto theModule = g_Workspace.createOrUseModule(moduleName, parent == "tests");

    // Parse all files in the "src" sub folder, except for tests where all the source code
    // is at the root folder
    string tmp      = path.string();
    theModule->path = tmp;
    if (!theModule->fromTestsFolder)
        tmp += "/src";
    enumerateFilesInModule(tmp, theModule);
    return theModule;
}

void EnumerateModuleJob::enumerateModules(const fs::path& path)
{
    // Scan source folder
    OS::visitFolders(path.string().c_str(), [&](const char* cFileName) {
        addModule(path.string() + cFileName);
    });
}

JobResult EnumerateModuleJob::execute()
{
    enumerateModules(g_Workspace.dependenciesPath);
    enumerateModules(g_Workspace.modulesPath);
    if (g_CommandLine.test)
        enumerateModules(g_Workspace.testsPath);
    return JobResult::ReleaseJob;
}
