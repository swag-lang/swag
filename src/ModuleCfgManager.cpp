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
#include "FetchModuleJobFileSystem.h"
#include "Diagnostic.h"
#include "SourceFile.h"

ModuleCfgManager g_ModuleCfgMgr;

Module* ModuleCfgManager::getCfgModule(const Utf8& name)
{
    auto it = allModules.find(name);
    if (it != allModules.end())
        return it->second;
    return nullptr;
}

void ModuleCfgManager::parseCfgFile(Module* cfgModule)
{
    auto buildJob    = g_Pool_moduleBuildJob.alloc();
    buildJob->module = cfgModule;

    auto syntaxJob        = g_Pool_syntaxJob.alloc();
    syntaxJob->sourceFile = cfgModule->files.front();
    syntaxJob->addDependentJob(buildJob);

    g_ThreadMgr.addJob(syntaxJob);
}

void ModuleCfgManager::registerCfgFile(SourceFile* file)
{
    Utf8       moduleName, moduleFolder;
    ModuleKind kind;
    g_Workspace.computeModuleName(fs::path(file->path).parent_path(), moduleName, moduleFolder, kind);

    auto cfgModule  = g_Allocator.alloc<Module>();
    cfgModule->kind = ModuleKind::Config;
    cfgModule->setup(moduleName, moduleFolder);
    cfgModule->addFile(file);

    // Register it
    if (getCfgModule(moduleName))
    {
        auto errorStr = format("more than one module with name '%s' is present in the workspace (path is '%s')", moduleName.c_str(), moduleFolder.c_str());
        g_Log.error(errorStr);
        exit(-1);
    }

    allModules[moduleName] = cfgModule;
    parseCfgFile(cfgModule);
}

void ModuleCfgManager::newCfgFile(vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName)
{
    auto file         = g_Allocator.alloc<SourceFile>();
    file->name        = fileName;
    file->cfgFile     = true;
    fs::path pathFile = dirName.c_str();
    pathFile.append(fileName.c_str());
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
        cfgName.append(SWAG_CFG_FILE);
        if (fs::exists(cfgName))
        {
            newCfgFile(allFiles, cfgPath.string(), SWAG_CFG_FILE);
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

bool ModuleCfgManager::dependencyIsMatching(ModuleDependency* dep, Module* module)
{
    return false;
}

bool ModuleCfgManager::fetchModuleCfgLocal(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    // Determin if this is a valid folder path
    auto remotePath = normalizePath(dep->location.c_str());
    remotePath += "/";
    remotePath += SWAG_MODULES_FOLDER;
    remotePath += "/";
    remotePath += dep->name;
    remotePath = fs::absolute(remotePath.c_str()).string();
    remotePath = fs::canonical(remotePath).string();
    if (!fs::exists(remotePath))
        return true;

    dep->fetchKind = DependencyFetchKind::FileSystem;
    dep->location  = remotePath;

    remotePath += "/";
    remotePath.append(SWAG_CFG_FILE);

    // No cfg file, we are done, and this is ok, we have found a module without
    // a specific configuration file. This is legit.
    if (!fs::exists(remotePath))
        return true;

    // Otherwise we copy the config file to the cache path, with a unique name.
    // Then later we will parse that file to get informations from the module
    FILE* fsrc = nullptr;
    File::openFile(&fsrc, remotePath.c_str(), "rbN");
    if (!fsrc)
        return dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot access file '%s'", remotePath.c_str())});

    // Remove source configuration file
    FILE* fdest    = nullptr;
    auto  destPath = g_Workspace.cachePath.string();
    destPath += "/";
    cfgFilePath = destPath;

    // Generate a unique name for the configuration file
    static int cacheNum = 0;
    cfgFileName         = format("module%u.swg", cacheNum++).c_str();
    destPath += cfgFileName;
    File::openFile(&fdest, destPath.c_str(), "wbN");
    if (!fdest)
    {
        File::closeFile(&fsrc);
        return dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot fetch file '%s' for module dependency '%s'", SWAG_CFG_FILE, dep->name.c_str())});
    }

    // Copy content
    uint8_t buffer[4096];
    while (true)
    {
        auto numRead = fread(buffer, 1, sizeof(buffer), fsrc);
        if (numRead)
            fwrite(buffer, 1, numRead, fdest);
        if (numRead != sizeof(buffer))
            break;
    }

    File::closeFile(&fsrc);
    File::closeFile(&fdest);
    return true;
}

bool ModuleCfgManager::fetchModuleCfg(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    if (dep->location.empty())
        return dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot resolve module dependency '%s' ('location' is empty)", dep->name.c_str())});

    cfgFilePath.clear();
    cfgFileName.clear();

    // Local module, as a filepath
    SWAG_CHECK(fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName));
    if (dep->fetchKind != DependencyFetchKind::Invalid)
        return true;

    return dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot resolve module dependency '%s'", dep->name.c_str())});
}

bool ModuleCfgManager::resolveModuleDependency(Module* cfgModule, ModuleDependency* dep)
{
    // Get the remote config file in cache (if it exists)
    Utf8 cfgFilePath, cfgFileName;
    SWAG_CHECK(fetchModuleCfg(dep, cfgFilePath, cfgFileName));

    // Module not here : add it.
    if (!cfgModule)
    {
        cfgModule       = g_Allocator.alloc<Module>();
        cfgModule->kind = ModuleKind::Config;
        cfgModule->setup(dep->name, "");
        allModules[dep->name] = cfgModule;
    }

    cfgModule->mustFetch = true;
    cfgModule->fetchDep  = dep;

    // Module exists without a config file. This is fine. Set default values.
    if (cfgFileName.empty())
    {
        SWAG_ASSERT(false); // todo
        cfgModule->buildCfg.moduleVersion.addr  = (void*) "1.0.0";
        cfgModule->buildCfg.moduleVersion.count = 5;
    }
    else
    {
        cfgModule->files.clear(); // memleak

        auto file = g_Allocator.alloc<SourceFile>();
        cfgModule->files.push_back(file);

        file->name        = cfgFileName;
        file->cfgFile     = true;
        file->module      = cfgModule;
        fs::path pathFile = cfgFilePath.c_str();
        pathFile.append(cfgFileName.c_str());
        file->path = normalizePath(pathFile);

        pendingCfgModules.insert(cfgModule);
        return true;
    }

    dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot resolve module dependency '%s'", dep->name.c_str())});
    return false;
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

    // Populate the list of all modules dependencies, until everything is done
    bool ok    = true;
    bool dirty = true;
    while (ok && dirty)
    {
        dirty = false;

        pendingCfgModules.clear();
        for (auto m : allModules)
        {
            auto module = m.second;
            for (auto dep : module->moduleDependencies)
            {
                auto cfgModule = getCfgModule(dep->name);

                // Invalid module dependency
                if (!cfgModule || !cfgModule->mustFetch)
                {
                    if (!cfgModule || !dependencyIsMatching(dep, cfgModule))
                    {
                        dirty = true;
                        ok &= resolveModuleDependency(cfgModule, dep);
                    }
                }
            }
        }

        // We have modules to parse again
        if (!pendingCfgModules.empty())
        {
            for (auto f : pendingCfgModules)
                parseCfgFile(f);
            g_ThreadMgr.waitEndJobs();
            g_Workspace.checkPendingJobs();
            continue;
        }
    }

    // Fetch all modules
    if (ok)
    {
        for (auto m : allModules)
        {
            if (!m.second->mustFetch)
                continue;

            Job* fetchJob = nullptr;
            switch (m.second->fetchDep->fetchKind)
            {
            case DependencyFetchKind::FileSystem:
                fetchJob = g_Pool_moduleFetchJobFileSystem.alloc();
                break;
            }

            SWAG_ASSERT(fetchJob);
            fetchJob->module = m.second;
            g_ThreadMgr.addJob(fetchJob);
        }

        g_ThreadMgr.waitEndJobs();
        ok = g_Workspace.numErrors.load() == 0;
    }

    if (g_CommandLine.verbose)
        g_Log.verbosePass(LogPassType::PassEnd, "Config Manager", "", g_Stats.cfgTime.load());

    return ok;
}