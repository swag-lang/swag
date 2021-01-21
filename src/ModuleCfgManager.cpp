#include "pch.h"
#include "ModuleCfgManager.h"
#include "Workspace.h"
#include "Context.h"
#include "Ast.h"
#include "Profile.h"
#include "Timer.h"
#include "Module.h"
#include "ThreadManager.h"
#include "ModuleBuildJob.h"

ModuleCfgManager g_ModuleCfgMgr;

void ModuleCfgManager::registerCfgFile(SourceFile* file)
{
    Utf8       moduleName, moduleFolder;
    ModuleKind kind;
    g_Workspace.computeModuleName(fs::path(file->path).parent_path(), moduleName, moduleFolder, kind);

    auto cfgModule  = g_Allocator.alloc<Module>();
    cfgModule->kind = ModuleKind::Config;
    cfgModule->setup(moduleName, moduleFolder);
    cfgModule->addFile(file);

    auto buildJob    = g_Pool_moduleBuildJob.alloc();
    buildJob->module = cfgModule;

    auto syntaxJob        = g_Pool_syntaxJob.alloc();
    syntaxJob->sourceFile = file;
    syntaxJob->addDependentJob(buildJob);

    g_ThreadMgr.addJob(syntaxJob);
}

void ModuleCfgManager::newCfgFile(vector<SourceFile*>& allFiles, string dirName, string fileName)
{
    auto file         = g_Allocator.alloc<SourceFile>();
    file->name        = fileName;
    file->cfgFile     = true;
    fs::path pathFile = dirName.c_str();
    pathFile.append(fileName);
    file->path = normalizePath(pathFile);

    // If we have only one core, then we will sort files in alphabetical order to always
    // treat them in a reliable order. That way, --randomize and --seed can work.
    if (g_CommandLine.numCores == 1)
        allFiles.push_back(file);
    else
    {
        registerCfgFile(file);
    }
}

void ModuleCfgManager::enumerateCfgFiles(const fs::path& path)
{
    vector<SourceFile*> allFiles;

    OS::visitFolders(path.string().c_str(), [&](const char* cFileName) {
        auto cfgPath = path;
        cfgPath.append(cFileName);
        auto cfgName = cfgPath;
        cfgName.append("module.swg");
        if (fs::exists(cfgName))
        {
            newCfgFile(allFiles, cfgPath.string(), "module.swg");
        }
    });

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b) { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
        {
            registerCfgFile(file);
        }
    }
}

bool ModuleCfgManager::execute()
{
    Timer timer(&g_Stats.cfgTime);
    timer.start();

    if (g_CommandLine.verbose)
        g_Log.verbosePass(LogPassType::PassBegin, "Config Manager", "");

    // Enumerate existing module.swag files, and do syntax for all of them
    enumerateCfgFiles(g_Workspace.dependenciesPath);
    enumerateCfgFiles(g_Workspace.modulesPath);
    enumerateCfgFiles(g_Workspace.examplesPath);
    if (g_CommandLine.test)
        enumerateCfgFiles(g_Workspace.testsPath);
    g_ThreadMgr.waitEndJobs();
    g_Workspace.checkPendingJobs();

    if (g_CommandLine.verbose)
        g_Log.verbosePass(LogPassType::PassEnd, "Config Manager", "", g_Stats.cfgTime.load());

    return true;
}