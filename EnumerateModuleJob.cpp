#include "pch.h"
#include "EnumerateModuleJob.h"
#include "Workspace.h"
#include "Os.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "Stats.h"

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
                        auto job        = g_Pool_syntaxJob.alloc();
                        auto file       = g_Pool_sourceFile.alloc();
                        job->sourceFile = file;
                        file->fromTests = theModule->fromTests;
                        file->path      = tmp + "\\" + cFileName;
                        file->writeTime = writeTime;
                        theModule->addFile(file);
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
    auto theModule       = g_Workspace.createOrUseModule(moduleName);
    theModule->fromTests = parent == "tests";

    // Add the build.swg file if it exists
    string tmp;
    tmp = path.string() + "/build.swg";
    if (fs::exists(tmp))
    {
        auto job             = g_Pool_syntaxJob.alloc();
        auto file            = g_Pool_sourceFile.alloc();
        job->sourceFile      = file;
        file->fromTests      = theModule->fromTests;
        file->path           = tmp;
        theModule->buildFile = file;
        theModule->addFile(file);
        g_ThreadMgr.addJob(job);
    }

    // Parse all files in the "src" sub folder, except for tests where all the source code
    // is at the root folder
    tmp             = path.string();
    theModule->path = tmp;
    if (!theModule->fromTests)
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
