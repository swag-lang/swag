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

void EnumerateModuleJob::addFileToModule(Module* theModule, vector<SourceFile*>& allFiles, string dirName, string fileName, uint64_t writeTime)
{
    auto file         = g_Allocator.alloc<SourceFile>();
    file->fromTests   = theModule->kind == ModuleKind::Test;
    file->name        = fileName;
    fs::path pathFile = dirName.c_str();
    pathFile.append(fileName);
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

void EnumerateModuleJob::enumerateFilesInModule(const fs::path& basePath, Module* theModule)
{
    vector<SourceFile*> allFiles;

    auto path = basePath;
    path += "/";
    path += SWAG_SRC_FOLDER;
    path += "/";

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
                        addFileToModule(theModule, allFiles, tmp, cFileName, writeTime);
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

    // Module name is equivalent to the folder name, except for the tests folder where
    // we prepend SWAG_TESTS_FOLDER
    string moduleName;
    if (parent == SWAG_TESTS_FOLDER)
    {
        moduleName = SWAG_TESTS_FOLDER;
        moduleName += "_";
    }

    moduleName += cFileName;

    // Kind
    ModuleKind kind = ModuleKind::Module;
    if (parent == SWAG_TESTS_FOLDER)
        kind = ModuleKind::Test;
    else if (parent == SWAG_EXAMPLES_FOLDER)
        kind = ModuleKind::Example;
    else if (parent == SWAG_DEPENDENCIES_FOLDER)
        kind = ModuleKind::Dependency;
    else if (parent == SWAG_MODULES_FOLDER)
        kind = ModuleKind::Module;
    else
    {
        SWAG_ASSERT(false);
    }

    // Create theModule
    auto theModule = g_Workspace.createOrUseModule(moduleName, path.string(), kind);

    // Parse all files in the source tree
    enumerateFilesInModule(path, theModule);
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
