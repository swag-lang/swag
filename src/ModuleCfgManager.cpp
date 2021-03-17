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

    if (kind != ModuleKind::Dependency)
        cfgModule->remoteLocationDep = moduleFolder;

    // Register it
    if (getCfgModule(moduleName))
    {
        auto errorStr = format("more than one module with name '%s' is present in the workspace (path is '%s')", moduleName.c_str(), moduleFolder.c_str());
        g_Log.error(errorStr);
        exit(-1);
    }

    allModules[moduleName] = cfgModule;

    // Ask to parse/resolved the config file
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

        // Each module must have a SWAG_CFG_FILE at its root, otherwise this is not a valid module
        if (!fs::exists(cfgName))
        {
            g_Log.error(format("fatal error: invalid module '%s', configuration file '%s' is missing", cfgPath.string().c_str(), SWAG_CFG_FILE));
            g_Workspace.numErrors++;
            return;
        }

        newCfgFile(allFiles, cfgPath.string(), SWAG_CFG_FILE);
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

bool ModuleCfgManager::fetchModuleCfgLocal(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    string remotePath = dep->resolvedLocation.c_str();
    remotePath += "/";
    remotePath.append(SWAG_CFG_FILE);

    // No cfg file, we are done, and this is ok, we have found a module without
    // a specific configuration file. This is legit.
    if (!fs::exists(remotePath))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("cannot find '%s' in module folder '%s'", SWAG_CFG_FILE, remotePath.c_str())});

    // Otherwise we copy the config file to the cache path, with a unique name.
    // Then later we will parse that file to get informations from the module
    FILE* fsrc = nullptr;
    File::openFile(&fsrc, remotePath.c_str(), "rbN");
    if (!fsrc)
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("cannot access file '%s'", remotePath.c_str())});

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
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("cannot fetch file '%s' for module dependency '%s'", SWAG_CFG_FILE, dep->name.c_str())});
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

bool ModuleCfgManager::fetchModuleCfgSwag(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    string remotePath = g_CommandLine.exePath.parent_path().string();
    remotePath += "/";
    remotePath += dep->locationParam;
    remotePath += "/";
    remotePath += SWAG_MODULES_FOLDER;
    remotePath += "/";
    remotePath += dep->name;
    remotePath = fs::absolute(remotePath.c_str()).string();
    remotePath = fs::canonical(remotePath).string();
    remotePath = normalizePath(fs::path(remotePath.c_str()));
    if (!fs::exists(remotePath))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("dependency module folder '%s' does not exist", remotePath.c_str())});
    dep->resolvedLocation = remotePath;

    return fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName);
}

bool ModuleCfgManager::fetchModuleCfgDisk(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    auto remotePath = string(dep->locationParam);
    remotePath += "/";
    remotePath += SWAG_MODULES_FOLDER;
    remotePath += "/";
    remotePath += dep->name;
    remotePath = fs::absolute(remotePath.c_str()).string();
    remotePath = fs::canonical(remotePath).string();
    remotePath = normalizePath(fs::path(remotePath.c_str()));
    if (!fs::exists(remotePath))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("dependency module folder '%s' does not exist", remotePath.c_str())});
    dep->resolvedLocation = remotePath;

    return fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName);
}

bool ModuleCfgManager::fetchModuleCfg(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    if (dep->location.empty())
        return dep->node->sourceFile->report({dep->node, dep->node->token, format("cannot resolve module dependency '%s' ('location' is empty)", dep->name.c_str())});

    vector<Utf8> tokens;
    tokenize(dep->location.c_str(), '@', tokens);
    if (tokens.size() != 2)
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, "invalid 'location' format; should have the form 'location=\"mode@accesspath\"'"});

    // Check mode
    if (tokens[0] != "swag" && tokens[0] != "disk")
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, format("invalid 'location' mode; should be 'swag' or 'disk', not '%s'", tokens[0].c_str())});
    dep->locationParam = tokens[1];

    cfgFilePath.clear();
    cfgFileName.clear();

    // Local module, as a filepath
    if (tokens[0] == "disk")
    {
        dep->fetchKind = DependencyFetchKind::Disk;
        return fetchModuleCfgDisk(dep, cfgFilePath, cfgFileName);
    }

    // Direct access to compiler std workspace
    if (tokens[0] == "swag")
    {
        dep->fetchKind = DependencyFetchKind::Swag;
        return fetchModuleCfgSwag(dep, cfgFilePath, cfgFileName);
    }

    SWAG_ASSERT(false);
    return false;
}

bool ModuleCfgManager::resolveModuleDependency(Module* srcModule, ModuleDependency* dep)
{
    // If location dependency is not defined, then we take the remote location of the module
    // with that dependency
    if (dep->location.empty())
        dep->location = srcModule->remoteLocationDep;

    // Module not here : add it.
    if (!dep->module)
    {
        dep->module       = g_Allocator.alloc<Module>();
        dep->module->kind = ModuleKind::Config;
        dep->module->setup(dep->name, "");
        allModules[dep->name] = dep->module;

        dep->module->mustFetchDep      = true;
        dep->module->wasAddedDep       = true;
        dep->module->remoteLocationDep = dep->location;
        dep->module->fetchDep          = dep;
        pendingCfgModules.insert(dep->module);
        return true;
    }

    // Now we can set the location of the module with the location of the dependency
    dep->module->remoteLocationDep = dep->location;

    auto cfgModule = dep->module;

    // If this is the first time, that means that we compare the requested dependency with the local version of the module.
    if (!cfgModule->fetchDep)
    {
        cfgModule->fetchDep = dep;

        auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, cfgModule->buildCfg.moduleVersion, cfgModule->buildCfg.moduleRevision, cfgModule->buildCfg.moduleBuildNum);
        switch (cmp)
        {
        // If the dependency requests a bigger version, then we will have to fetch the dependency configuration from the
        // remote path.
        case CompareVersionResult::VERSION_GREATER:
        case CompareVersionResult::REVISION_GREATER:
        case CompareVersionResult::BUILDNUM_GREATER:
            if (cfgModule->wasAddedDep || g_CommandLine.computeDep)
            {
                cfgModule->mustFetchDep = true;
                pendingCfgModules.insert(cfgModule);
            }

            break;

        // If the dependency does not specify something, that means that we don't know if we are up to date.
        // In that case, if g_CommandLine.computeDep is true, we will have to fetch dependency configuration file
        // to compare the local version with the remote one.
        case CompareVersionResult::EQUAL:
            if (g_CommandLine.computeDep)
            {
                if (dep->verNum == UINT32_MAX || dep->revNum == UINT32_MAX || dep->buildNum == UINT32_MAX)
                {
                    pendingCfgModules.insert(cfgModule);
                }
            }
            break;
        }
    }

    // Here, we have two dependencies requests in different modules. So we compare both to keep the 'more recent' one.
    // If a dependency wants version 1.? and another wants 2.?, then this is an error as it is invalid to depend on
    // two different major versions.
    else if (cfgModule->fetchDep != dep)
    {
        auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, cfgModule->fetchDep->verNum, cfgModule->fetchDep->revNum, cfgModule->fetchDep->buildNum);
        switch (cmp)
        {
        case CompareVersionResult::VERSION_GREATER:
        case CompareVersionResult::VERSION_LOWER:
        {
            Diagnostic diag{dep->node, format("cannot resolve dependency to module '%s' because of two different major versions ('%d' and '%d')", dep->name.c_str(), dep->verNum, cfgModule->fetchDep->verNum)};
            Diagnostic note{cfgModule->fetchDep->node, "this is the other '#import'", DiagnosticLevel::Note};
            dep->node->sourceFile->report(diag, &note);
            return false;
        }

        case CompareVersionResult::REVISION_GREATER:
        case CompareVersionResult::BUILDNUM_GREATER:
            cfgModule->fetchDep = dep;
            if (cfgModule->wasAddedDep || g_CommandLine.computeDep)
            {
                cfgModule->mustFetchDep = true;
                pendingCfgModules.insert(cfgModule);
            }
            break;

        case CompareVersionResult::EQUAL:
            if (g_CommandLine.computeDep)
            {
                if (dep->verNum == UINT32_MAX || dep->revNum == UINT32_MAX || dep->buildNum == UINT32_MAX)
                {
                    cfgModule->fetchDep = dep;
                    pendingCfgModules.insert(cfgModule);
                }
            }
            break;
        }
    }

    return true;
}

CompareVersionResult ModuleCfgManager::compareVersions(uint32_t depVer, uint32_t depRev, uint32_t devBuildNum, uint32_t modVer, uint32_t modRev, uint32_t modBuildNum)
{
    if (depVer != UINT32_MAX && modVer != UINT32_MAX && depVer > modVer)
        return CompareVersionResult::VERSION_GREATER;
    if (depVer != UINT32_MAX && modVer != UINT32_MAX && depVer < modVer)
        return CompareVersionResult::VERSION_LOWER;

    if (depRev != UINT32_MAX && modRev != UINT32_MAX && depRev < modRev)
        return CompareVersionResult::REVISION_LOWER;
    if (depRev != UINT32_MAX && modRev != UINT32_MAX && depRev > modRev)
        return CompareVersionResult::REVISION_GREATER;

    if (devBuildNum != UINT32_MAX && modBuildNum != UINT32_MAX && devBuildNum > modBuildNum)
        return CompareVersionResult::BUILDNUM_GREATER;
    if (devBuildNum != UINT32_MAX && modBuildNum != UINT32_MAX && devBuildNum < modBuildNum)
        return CompareVersionResult::BUILDNUM_LOWER;

    return CompareVersionResult::EQUAL;
}

bool ModuleCfgManager::execute()
{
    Timer timer(&g_Stats.cfgTime);
    timer.start();

    g_Log.verbosePass(LogPassType::Title, "ConfigManager", "");
    g_Log.verbosePass(LogPassType::PassBegin, "ConfigManager", "");

    // Enumerate existing configuration files, and do syntax/semantic for all of them
    enumerateCfgFiles(g_Workspace.dependenciesPath);
    enumerateCfgFiles(g_Workspace.modulesPath);
    enumerateCfgFiles(g_Workspace.examplesPath);
    if (g_CommandLine.test || g_CommandLine.listDepCmd || g_CommandLine.fetchDep)
        enumerateCfgFiles(g_Workspace.testsPath);
    g_ThreadMgr.waitEndJobs();
    g_Workspace.checkPendingJobs();
    if (g_Workspace.numErrors)
        return false;

    // Remember the configuration of the local module
    for (auto m : allModules)
    {
        auto module         = m.second;
        module->localCfgDep = module->buildCfg;
    }

    // Populate the list of all modules dependencies, until everything is done
    bool ok = true;
    while (ok)
    {
        pendingCfgModules.clear();
        for (auto m : allModules)
        {
            auto parentModule = m.second;

            // Do not treat dependencies of a module without a remote location.
            // The remote location will be initialized when known, starting with
            // local modules
            if (parentModule->remoteLocationDep.empty())
                continue;

            for (auto dep : parentModule->moduleDependencies)
            {
                dep->module = getCfgModule(dep->name);
                ok &= resolveModuleDependency(parentModule, dep);
            }
        }

        if (!ok)
            break;
        if (pendingCfgModules.empty())
            break;

        // We have modules to parse again
        for (auto cfgModule : pendingCfgModules)
        {
            // Get the remote config file in cache (if it exists), that depends on the dependency
            Utf8 cfgFilePath, cfgFileName;
            SWAG_CHECK(fetchModuleCfg(cfgModule->fetchDep, cfgFilePath, cfgFileName));

            cfgModule->files.clear(); // memleak

            auto file = g_Allocator.alloc<SourceFile>();
            cfgModule->files.push_back(file);

            file->name        = cfgFileName;
            file->cfgFile     = true;
            file->module      = cfgModule;
            fs::path pathFile = cfgFilePath.c_str();
            pathFile.append(cfgFileName.c_str());
            file->path = normalizePath(pathFile);

            parseCfgFile(cfgModule);
        }

        g_ThreadMgr.waitEndJobs();
        g_Workspace.checkPendingJobs();
        ok = g_Workspace.numErrors.load() == 0;
    }

    if (ok)
    {
        for (auto m : allModules)
        {
            auto module = m.second;
            auto dep    = module->fetchDep;
            if (!dep)
                continue;

            // Verify that all fetch dep match with the corresponding module
            auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (cmp != CompareVersionResult::EQUAL)
            {
                Diagnostic diag{dep->node, format("cannot resolve dependency to module '%s', version '%s' cannot be found at location '%s'", dep->name.c_str(), dep->version.c_str(), dep->resolvedLocation.c_str())};
                dep->node->sourceFile->report(diag);
                ok = false;
            }

            // Compare the fetch version with the original local one. If they do not match, then we must fetch the module content
            if (!module->wasAddedDep && !module->mustFetchDep)
            {
                cmp = compareVersions(module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum, module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
                if (cmp != CompareVersionResult::EQUAL)
                {
                    module->mustFetchDep = true;
                }
            }
        }
    }

    // List all dependencies
    if (ok && g_CommandLine.listDepCmd)
    {
        for (auto m : allModules)
        {
            auto module = m.second;
            Utf8 msg;
            if (module->fetchDep)
                msg += format("%d.%d.%d", module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum);
            else
                msg += format("%d.%d.%d", module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (module->mustFetchDep)
                msg += format(" => version %d.%d.%d is available", module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            g_Log.messageHeaderDot(module->name, msg);
        }
    }

    // Fetch all modules
    if (ok && g_CommandLine.fetchDep)
    {
        for (auto m : allModules)
        {
            if (!m.second->mustFetchDep)
                continue;

            Job* fetchJob = nullptr;
            switch (m.second->fetchDep->fetchKind)
            {
            case DependencyFetchKind::Disk:
            case DependencyFetchKind::Swag:
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

    timer.stop();
    g_Log.verbosePass(LogPassType::PassEnd, "ConfigManager", "", g_Stats.cfgTime.load());

    return ok;
}