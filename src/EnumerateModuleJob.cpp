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
#include "ModuleCfgManager.h"

SourceFile* EnumerateModuleJob::addFileToModule(Module* theModule, vector<SourceFile*>& allFiles, string dirName, string fileName, uint64_t writeTime)
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
        theModule->addFile(file);

    return file;
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

    // Add the config file, second pass
    auto cfgModule = g_ModuleCfgMgr.getCfgModule(theModule->name);
    if (cfgModule)
    {
        auto cfgFile    = theModule->path + "/" + SWAG_CFG_FILE;
        auto writeTime  = OS::getFileWriteTime(cfgFile.c_str());
        auto file       = addFileToModule(theModule, allFiles, fs::path(cfgFile).parent_path().string(), SWAG_CFG_FILE, writeTime);
        file->isCfgFile = true;
    }

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b) { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
            theModule->addFile(file);
    }
}

Module* EnumerateModuleJob::addModule(const fs::path& path)
{
    Utf8       moduleName, moduleFolder;
    ModuleKind kind;

    // Get infos about module, depending on where it is located
    g_Workspace.computeModuleName(path, moduleName, moduleFolder, kind);

    // Create theModule
    auto theModule = g_Workspace.createOrUseModule(moduleName, moduleFolder, kind);

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
    // Modules in the workspace
    if (!g_CommandLine.scriptCommand)
    {
        enumerateModules(g_Workspace.dependenciesPath);
        enumerateModules(g_Workspace.modulesPath);
        enumerateModules(g_Workspace.examplesPath);
        if (g_CommandLine.test)
            enumerateModules(g_Workspace.testsPath);
    }
    else
    {
        // If we are in script mode, then we add one single module with the script file
        auto parentFolder  = fs::path(g_CommandLine.scriptName.c_str()).parent_path().string();
        auto file          = g_Allocator.alloc<SourceFile>();
        file->name         = fs::path(g_CommandLine.scriptName).filename().string();
        auto scriptModule  = g_Workspace.createOrUseModule(file->name, parentFolder, ModuleKind::Script);
        file->path         = g_CommandLine.scriptName;
        file->module       = scriptModule;
        file->isScriptFile = true;
        scriptModule->addFile(file);
        g_Workspace.runModule = scriptModule;
    }

    // Add all external dependencies
    for (int i = 0; i < g_Workspace.modules.size(); i++)
    {
        auto m = g_Workspace.modules[i];
        for (auto d : m->moduleDependencies)
        {
            if (d->fetchKind != DependencyFetchKind::Swag)
                continue;

            Utf8       moduleName, moduleFolder;
            ModuleKind kind;
            fs::path   modulePath = fs::path(d->resolvedLocation.c_str());
            g_Workspace.computeModuleName(modulePath, moduleName, moduleFolder, kind);
            if (!g_Workspace.getModuleByName(moduleName))
                addModule(modulePath);
        }
    }

    return JobResult::ReleaseJob;
}
