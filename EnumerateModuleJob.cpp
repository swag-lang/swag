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
    auto timeBefore = chrono::high_resolution_clock::now();

    // If no theModule is specified, just compiled all modules in the workspace
    if (g_CommandLine.modulePath.empty())
    {
        enumerateModules(g_Workspace.dependenciesPath);
        enumerateModules(g_Workspace.modulesPath);
        if (g_CommandLine.test)
            enumerateModules(g_Workspace.testsPath);
    }

    // Else compile the theModule and its dependencies
    else
    {
        vector<fs::path> modulesToGo;
        fs::path         modulePath = g_CommandLine.workspacePath;
        modulePath.append(g_CommandLine.modulePath);
        modulesToGo.push_back(modulePath);
        for (int i = 0; i < modulesToGo.size(); i++)
        {
            auto theModule = addModule(modulesToGo[i]);
            g_ThreadMgr.waitEndJobs();
            for (auto& dep : theModule->moduleDependencies)
            {
                auto it = g_Workspace.mapModulesNames.find(dep.first);
                if (it == g_Workspace.mapModulesNames.end())
                {
                    auto depName = dep.second.name;
                    auto depPath = g_Workspace.dependenciesPath;
                    depPath.append(depName);
                    if (!fs::exists(depPath))
                    {
                        depPath = g_Workspace.modulesPath;
                        depPath.append(depName);
                        if (!fs::exists(depPath))
                        {
                            continue;
                        }
                    }

                    modulesToGo.push_back(depPath);
                }
            }
        }
    }

    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    return JobResult::ReleaseJob;
}
