#include "pch.h"
#include "ModuleCfgManager.h"
#include "Workspace.h"
#include "Module.h"
#include "ThreadManager.h"
#include "ModuleBuildJob.h"
#include "FetchModuleFileSystemJob.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "File.h"
#include "LanguageSpec.h"

ModuleCfgManager* g_ModuleCfgMgr = nullptr;

Module* ModuleCfgManager::getCfgModule(const Utf8& name)
{
    auto it = allModules.find(name);
    if (it != allModules.end())
        return it->second;
    return nullptr;
}

void ModuleCfgManager::parseCfgFile(Module* cfgModule)
{
    auto buildJob    = g_Allocator.alloc<ModuleBuildJob>();
    buildJob->module = cfgModule;

    auto syntaxJob        = g_Allocator.alloc<SyntaxJob>();
    syntaxJob->sourceFile = cfgModule->files.front();
    syntaxJob->addDependentJob(buildJob);

    g_ThreadMgr.addJob(syntaxJob);
}

void ModuleCfgManager::registerCfgFile(SourceFile* file)
{
    Utf8       moduleName, moduleFolder;
    ModuleKind kind = ModuleKind::Module;

    auto parentFolder = fs::path(file->path).parent_path();
    if (file->isScriptFile)
    {
        moduleName   = file->name;
        moduleFolder = parentFolder.string();
    }
    else
        g_Workspace->computeModuleName(parentFolder, moduleName, moduleFolder, kind);

    auto cfgModule  = g_Allocator.alloc<Module>();
    cfgModule->kind = ModuleKind::Config;
    cfgModule->setup(moduleName, moduleFolder);
    cfgModule->addFile(file);

    if (kind != ModuleKind::Dependency)
    {
        cfgModule->isLocalToWorkspace = true;
        cfgModule->remoteLocationDep  = moduleFolder;
    }

    // Register it
    if (getCfgModule(moduleName))
    {
        g_Log.error(Utf8::format(g_E[Err0169], moduleName.c_str(), moduleFolder.c_str()));
        OS::exit(-1);
    }

    allModules[moduleName] = cfgModule;

    // Ask to parse/resolved the config file
    parseCfgFile(cfgModule);
}

void ModuleCfgManager::newCfgFile(vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName)
{
    auto file         = g_Allocator.alloc<SourceFile>();
    file->name        = fileName;
    file->isCfgFile   = true;
    fs::path pathFile = dirName.c_str();
    pathFile.append(fileName.c_str());
    file->path = Utf8::normalizePath(pathFile);

    // If we have only one core, then we will sort files in alphabetical order to always
    // treat them in a reliable order. That way, --randomize and --seed can work.
    if (g_CommandLine->numCores == 1)
        allFiles.push_back(file);
    else
        registerCfgFile(file);
}

void ModuleCfgManager::enumerateCfgFiles(const fs::path& path)
{
    vector<SourceFile*> allFiles;

    OS::visitFolders(path.string().c_str(), [&](const char* cFileName)
                     {
                         auto cfgPath = path;
                         cfgPath.append(cFileName);

                         auto cfgName = cfgPath;
                         cfgName.append(SWAG_CFG_FILE);

                         // Each module must have a SWAG_CFG_FILE at its root, otherwise this is not a valid module
                         if (!fs::exists(cfgName))
                         {
                             g_Log.error(Utf8::format(g_E[Err0507], cfgPath.string().c_str(), SWAG_CFG_FILE));
                             g_Workspace->numErrors++;
                             return;
                         }

                         newCfgFile(allFiles, cfgPath.string(), SWAG_CFG_FILE);
                     });

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        sort(allFiles.begin(), allFiles.end(), [](SourceFile* a, SourceFile* b)
             { return strcmp(a->name.c_str(), b->name.c_str()) == -1; });
        for (auto file : allFiles)
            registerCfgFile(file);
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
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0508], SWAG_CFG_FILE, remotePath.c_str())});

    // Otherwise we copy the config file to the cache path, with a unique name.
    // Then later we will parse that file to get informations from the module
    FILE* fsrc = nullptr;
    if (!openFile(&fsrc, remotePath.c_str(), "rbN"))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0509], remotePath.c_str())});

    // Remove source configuration file
    FILE* fdest    = nullptr;
    auto  destPath = g_Workspace->cachePath.string();
    destPath += "/";
    cfgFilePath = destPath;

    // Generate a unique name for the configuration file
    static int cacheNum = 0;
    cfgFileName         = Utf8::format("module%u.swg", cacheNum++).c_str();
    destPath += cfgFileName;
    if (!openFile(&fdest, destPath.c_str(), "wbN"))
    {
        closeFile(&fsrc);
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0510], SWAG_CFG_FILE, dep->name.c_str())});
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

    closeFile(&fsrc);
    closeFile(&fdest);
    return true;
}

bool ModuleCfgManager::fetchModuleCfgSwag(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    string remotePath = g_CommandLine->exePath.parent_path().string();
    remotePath += "/";
    remotePath += dep->locationParam;
    remotePath += "/";
    remotePath += SWAG_MODULES_FOLDER;
    remotePath += "/";
    remotePath += dep->name;
    remotePath = fs::absolute(remotePath.c_str()).string();
    remotePath = fs::canonical(remotePath).string();
    remotePath = Utf8::normalizePath(fs::path(remotePath.c_str()));
    if (!fs::exists(remotePath))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0511], remotePath.c_str())});
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
    remotePath = Utf8::normalizePath(fs::path(remotePath.c_str()));
    if (!fs::exists(remotePath))
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0511], remotePath.c_str())});
    dep->resolvedLocation = remotePath;

    return fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName);
}

bool ModuleCfgManager::fetchModuleCfg(ModuleDependency* dep, Utf8& cfgFilePath, Utf8& cfgFileName)
{
    if (dep->location.empty())
        return dep->node->sourceFile->report({dep->node, Utf8::format(g_E[Err0513], dep->name.c_str())});

    vector<Utf8> tokens;
    Utf8::tokenize(dep->location.c_str(), '@', tokens);
    if (tokens.size() != 2)
    {
        if (dep->isLocalToWorkspace)
            return dep->node->sourceFile->report({dep->node, g_E[Err0514]});
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, g_E[Err0514]});
    }

    // Check mode
    if (tokens[0] != g_LangSpec->name_swag && tokens[0] != g_LangSpec->name_disk)
        return dep->node->sourceFile->report({dep->node, dep->tokenLocation, Utf8::format(g_E[Err0515], tokens[0].c_str())});
    dep->locationParam = tokens[1];

    cfgFilePath.clear();
    cfgFileName.clear();

    // Local module, as a filepath
    if (tokens[0] == g_LangSpec->name_disk)
    {
        dep->fetchKind = DependencyFetchKind::Disk;
        return fetchModuleCfgDisk(dep, cfgFilePath, cfgFileName);
    }

    // Direct access to compiler std workspace
    if (tokens[0] == g_LangSpec->name_swag)
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
    {
        dep->isLocalToWorkspace = srcModule->isLocalToWorkspace;
        dep->location           = srcModule->remoteLocationDep;
    }

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
            if (cfgModule->wasAddedDep || g_CommandLine->computeDep)
            {
                cfgModule->mustFetchDep = true;
                pendingCfgModules.insert(cfgModule);
            }

            break;

        // If the dependency does not specify something, that means that we don't know if we are up to date.
        // In that case, if g_CommandLine->computeDep is true, we will have to fetch dependency configuration file
        // to get the one that corresponds to the dependency (UINT32_MAX means 'latest')
        case CompareVersionResult::EQUAL:
            if (g_CommandLine->computeDep)
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
            Diagnostic diag{dep->node, Utf8::format(g_E[Err0516], dep->name.c_str(), dep->verNum, cfgModule->fetchDep->verNum)};
            Diagnostic note{cfgModule->fetchDep->node, g_E[Note035], DiagnosticLevel::Note};
            dep->node->sourceFile->report(diag, &note);
            return false;
        }

        case CompareVersionResult::REVISION_GREATER:
        case CompareVersionResult::BUILDNUM_GREATER:
            cfgModule->fetchDep = dep;
            if (cfgModule->wasAddedDep || g_CommandLine->computeDep)
            {
                cfgModule->mustFetchDep = true;
                pendingCfgModules.insert(cfgModule);
            }
            break;

        case CompareVersionResult::EQUAL:
            // If version is more specific, then take it
            if ((cfgModule->fetchDep->verNum == UINT32_MAX && dep->verNum != UINT32_MAX) ||
                (cfgModule->fetchDep->revNum == UINT32_MAX && dep->revNum != UINT32_MAX) ||
                (cfgModule->fetchDep->buildNum == UINT32_MAX && dep->buildNum != UINT32_MAX))
            {
                cfgModule->fetchDep = dep;
                pendingCfgModules.insert(cfgModule);
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

    // Enumerate existing configuration files, and do
    // syntax/semantic for all of them.
    // In this pass, only the #dependencies block will
    // be evaluated
    //////////////////////////////////////////////////
    if (!g_CommandLine->scriptCommand)
    {
        enumerateCfgFiles(g_Workspace->dependenciesPath);
        enumerateCfgFiles(g_Workspace->modulesPath);
        enumerateCfgFiles(g_Workspace->examplesPath);
        if (g_CommandLine->test || g_CommandLine->listDepCmd || g_CommandLine->fetchDep)
            enumerateCfgFiles(g_Workspace->testsPath);
    }

    // When this is a simple script, then register the script file as the configuration file
    else
    {
        auto file          = g_Allocator.alloc<SourceFile>();
        file->name         = fs::path(g_CommandLine->scriptName.c_str()).filename().string().c_str();
        file->isCfgFile    = true;
        file->isScriptFile = true;
        file->path         = Utf8::normalizePath(g_CommandLine->scriptName);
        registerCfgFile(file);
    }

    g_ThreadMgr.waitEndJobs();
    g_Workspace->checkPendingJobs();
    if (g_Workspace->numErrors)
        return false;

    // Remember the configuration of the local modules
    //////////////////////////////////////////////////
    for (auto m : allModules)
    {
        auto module         = m.second;
        module->localCfgDep = module->buildCfg;
    }

    // Populate the list of all modules dependencies,
    // until everything is done
    //////////////////////////////////////////////////
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
            // Nothing to fetch if this is an internal workspace module
            if (cfgModule->isLocalToWorkspace)
                continue;

            // Get the remote config file in cache (if it exists), that depends on the dependency
            Utf8 cfgFilePath, cfgFileName;
            SWAG_CHECK(fetchModuleCfg(cfgModule->fetchDep, cfgFilePath, cfgFileName));

            cfgModule->files.clear(); // memleak

            auto file = g_Allocator.alloc<SourceFile>();
            cfgModule->files.push_back(file);

            file->name        = cfgFileName;
            file->isCfgFile   = true;
            file->module      = cfgModule;
            fs::path pathFile = cfgFilePath.c_str();
            pathFile.append(cfgFileName.c_str());
            file->path = Utf8::normalizePath(pathFile);

            parseCfgFile(cfgModule);
        }

        g_ThreadMgr.waitEndJobs();
        g_Workspace->checkPendingJobs();
        ok = g_Workspace->numErrors.load() == 0;
    }

    if (!ok)
        return false;

    // Compare versions
    //////////////////////////////////////////////////
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
            Diagnostic diag{dep->node, Utf8::format(g_E[Err0518], dep->name.c_str(), dep->version.c_str(), dep->resolvedLocation.c_str())};
            dep->node->sourceFile->report(diag);
            ok = false;
        }

        // Compare the fetch version with the original local one. If they do not match, then we must fetch the module content
        if (!module->wasAddedDep && !module->mustFetchDep)
        {
            cmp = compareVersions(module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum, module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (cmp != CompareVersionResult::EQUAL)
                module->mustFetchDep = true;
            else if (g_CommandLine->getDepCmd && g_CommandLine->getDepForce)
                module->mustFetchDep = true;
        }
    }

    if (!ok)
        return false;

    // List all dependencies
    //////////////////////////////////////////////////
    if (g_CommandLine->listDepCmd)
    {
        for (auto m : allModules)
        {
            auto module = m.second;
            Utf8 msg;
            if (module->fetchDep)
                msg += Utf8::format("%d.%d.%d", module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum);
            else
                msg += Utf8::format("%d.%d.%d", module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (module->fetchDep && module->fetchDep->fetchKind == DependencyFetchKind::Swag)
                msg += " [swag]";
            else if (module->mustFetchDep)
                msg += Utf8::format(" => version %d.%d.%d is available", module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            g_Log.messageHeaderDot(module->name, msg);
        }
    }

    // Fetch all modules
    //////////////////////////////////////////////////
    if (g_CommandLine->fetchDep)
    {
        for (auto m : allModules)
        {
            if (!m.second->fetchDep)
                continue;
            if (!m.second->mustFetchDep)
                continue;

            Job* fetchJob = nullptr;
            switch (m.second->fetchDep->fetchKind)
            {
            case DependencyFetchKind::Disk:
            case DependencyFetchKind::Swag:
                fetchJob = g_Allocator.alloc<FetchModuleFileSystemJob>();
                break;
            }

            SWAG_ASSERT(fetchJob);
            fetchJob->module = m.second;
            g_ThreadMgr.addJob(fetchJob);
        }

        g_ThreadMgr.waitEndJobs();
        ok = g_Workspace->numErrors.load() == 0;
    }

    return ok;
}