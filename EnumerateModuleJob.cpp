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

    vector<SourceFile*> allFiles;
    string              tmp, tmp1;
    fs::path            modulePath;
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

                        // If we have only one core, then we will sort files in alphabetical order to always
                        // treat them in a reliable order. That way, --randomize and --seed can work.
                        if (g_CommandLine.numCores == 1)
                            allFiles.push_back(file);
                        else
                        {
                            theModule->addFile(file);
                            auto job        = g_Pool_syntaxJob.alloc();
                            job->sourceFile = file;
                            g_ThreadMgr.addJob(job);
                        }
                    }
                }
            }
        });
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b) { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
        {
            theModule->addFile(file);
            auto job        = g_Pool_syntaxJob.alloc();
            job->sourceFile = file;
            g_ThreadMgr.addJob(job);
        }
    }
}

Module* EnumerateModuleJob::addModule(const fs::path& path)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Be sure module name is valid
    Utf8 errorStr;
    if (!Module::isValidName(cFileName, errorStr))
    {
        errorStr = "fatal error: " + errorStr;
        errorStr += format(" (path is '%s')", path.string().c_str());
        g_Log.error(errorStr);
        exit(-1);
    }

    // Module name is equivalent to the folder name
    string moduleName;
    if (parent == "tests")
        moduleName = "tests_";
    moduleName += cFileName;

    // Create theModule
    auto theModule = g_Workspace.createOrUseModule(moduleName, parent == "tests", parent == "examples");

    // Parse all files in the "src" sub folder
    string tmp      = path.string();
    theModule->path = tmp;
    enumerateFilesInModule(tmp, theModule);
    return theModule;
}

void EnumerateModuleJob::enumerateModules(const fs::path& path)
{
    vector<string> allModules;

    // Scan source folder
    OS::visitFolders(path.string().c_str(), [&](const char* cFileName) {
        // If we have only one core, then we will sort modules in alphabetical order to always
        // treat them in a reliable order. That way, --randomize and --seed can work.
        if (g_CommandLine.numCores == 1)
            allModules.push_back(cFileName);
        else
            addModule(path.string() + cFileName);
    });

    // Sort modules, and register them in a constant order
    if (!allModules.empty())
    {
        sort(allModules.begin(), allModules.end());
        for (auto m : allModules)
        {
            addModule(path.string() + m);
        }
    }
}

JobResult EnumerateModuleJob::execute()
{
    enumerateModules(g_Workspace.dependenciesPath);
    enumerateModules(g_Workspace.modulesPath);
    enumerateModules(g_Workspace.examplesPath);
    if (g_CommandLine.test)
        enumerateModules(g_Workspace.testsPath);
    return JobResult::ReleaseJob;
}
