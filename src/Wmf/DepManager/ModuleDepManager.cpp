#include "pch.h"
#include "Wmf/DepManager/ModuleDepManager.h"
#include "Core/Timer.h"
#include "FetchModuleFileSystemJob.h"
#include "Jobs/ModuleBuildJob.h"
#include "Main/Statistics.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Syntax/SyntaxJob.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Workspace.h"

ModuleDepManager* g_ModuleCfgMgr = nullptr;

Module* ModuleDepManager::getCfgModule(const Utf8& name)
{
    const auto it = allModules.find(name);
    if (it != allModules.end())
        return it->second;
    return nullptr;
}

void ModuleDepManager::parseCfgFile(Module* cfgModule)
{
    const auto buildJob = Allocator::alloc<ModuleBuildJob>();
    buildJob->module    = cfgModule;

    const auto syntaxJob  = Allocator::alloc<SyntaxJob>();
    syntaxJob->sourceFile = cfgModule->files.front();
    syntaxJob->addDependentJob(buildJob);

    g_ThreadMgr.addJob(syntaxJob);
}

void ModuleDepManager::registerCfgFile(SourceFile* file)
{
    Utf8 moduleName;
    Path moduleFolder;
    auto kind = ModuleKind::Module;

    const auto parentFolder = file->path.parent_path();
    if (file->hasFlag(FILE_SCRIPT))
    {
        moduleName   = file->path.filename().replace_extension();
        moduleFolder = parentFolder;
    }
    else
        Workspace::computeModuleName(parentFolder, moduleName, moduleFolder, kind);

    const auto cfgModule    = Allocator::alloc<Module>();
    cfgModule->kind         = ModuleKind::Config;
    cfgModule->isScriptFile = file->hasFlag(FILE_SCRIPT);
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
        Report::error(formErr(Fat0012, moduleName.c_str(), moduleFolder.c_str()));
        OS::exit(-1);
    }

    allModules[moduleName] = cfgModule;

    // Ask to parse/resolved the config file
    parseCfgFile(cfgModule);
}

void ModuleDepManager::newCfgFile(Vector<SourceFile*>& allFiles, const Utf8& dirName, const Utf8& fileName)
{
    const auto file = Allocator::alloc<SourceFile>();
    file->name      = fileName;
    file->addFlag(FILE_CFG);
    Path pathFile = dirName;
    pathFile.append(fileName);
    file->path = pathFile;

    // If we have only one core, then we will sort files in alphabetical order to always
    // treat them in a reliable order. That way, --randomize and --seed can work.
    if (g_ThreadMgr.numWorkers == 1)
        allFiles.push_back(file);
    else
        registerCfgFile(file);
}

// If there is an 'alias' file in the source folder, then we redirect the source path with the content.
// That way, we can have a normal "dependencies" hierarchy, but with the source code coming from elsewhere.
// For example the 'std' workspace
Path ModuleDepManager::getAliasPath(const Path& srcPath)
{
    auto p = srcPath;
    p.append(SWAG_ALIAS_FILENAME);
    std::error_code err;
    if (std::filesystem::exists(p, err))
    {
        FILE* f = nullptr;
        if (!fopen_s(&f, p, "rt"))
        {
            char in[MAX_PATH];
            (void) fgets(in, MAX_PATH, f);
            (void) fclose(f);
            return in;
        }
    }

    return srcPath;
}

void ModuleDepManager::enumerateCfgFiles(const Path& path)
{
    Vector<SourceFile*> allFiles;

    OS::visitFolders(path, [&](const char* cFileName) {
        auto cfgPath = path;
        cfgPath.append(cFileName);
        cfgPath = getAliasPath(cfgPath);

        auto cfgName = cfgPath;
        cfgName.append(SWAG_CFG_FILE);

        // Each module must have a SWAG_CFG_FILE at its root, otherwise this is not a valid module
        std::error_code err;
        if (std::filesystem::exists(cfgName, err))
            newCfgFile(allFiles, cfgPath, SWAG_CFG_FILE);
    });

    // Sort files, and register them in a constant order
    if (!allFiles.empty())
    {
        std::ranges::sort(allFiles, [](const SourceFile* a, const SourceFile* b) { return strcmp(a->name, b->name) < 0; });
        for (const auto file : allFiles)
            registerCfgFile(file);
    }
}

bool ModuleDepManager::fetchModuleCfgLocal(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName)
{
    auto remotePath = dep->resolvedLocation;
    remotePath.append(SWAG_CFG_FILE);

    // No cfg file, we are done, we need one !
    std::error_code err;
    if (!std::filesystem::exists(remotePath, err))
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0082, SWAG_CFG_FILE, remotePath.c_str())});

    // Otherwise we copy the config file to the cache path, with a unique name.
    // Then later we will parse that file to get information from the module
    FILE* fsrc = nullptr;
    if (fopen_s(&fsrc, remotePath, "rbN"))
    {
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0083, remotePath.c_str())});
    }

    // Remove source configuration file
    FILE* fdest    = nullptr;
    auto  destPath = g_Workspace->cachePath;
    cfgFilePath.append(destPath);

    // Generate a unique name for the configuration file
    static int cacheNum = 0;
    cfgFileName         = form("module%u.swg", cacheNum++).c_str();
    destPath.append(cfgFileName);
    if (fopen_s(&fdest, destPath, "wbN"))
    {
        (void) fclose(fsrc);
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0078, SWAG_CFG_FILE, dep->name.c_str())});
    }

    // Copy content
    uint8_t buffer[4096];
    while (true)
    {
        const auto numRead = fread(buffer, 1, sizeof(buffer), fsrc);
        if (numRead)
            (void) fwrite(buffer, 1, numRead, fdest);
        if (numRead != sizeof(buffer))
            break;
    }

    (void) fclose(fsrc);
    (void) fflush(fdest);
    (void) fclose(fdest);

    return true;
}

bool ModuleDepManager::fetchModuleCfgSwag(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch)
{
    Path remotePath = g_CommandLine.exePath.parent_path();
    remotePath.append(dep->locationParam);
    remotePath.append(SWAG_MODULES_FOLDER);
    remotePath.append(dep->name);

    remotePath = std::filesystem::absolute(remotePath);
    std::error_code err;
    const auto      remotePath1 = std::filesystem::canonical(remotePath, err);
    if (!err)
        remotePath = remotePath1;
    if (!std::filesystem::exists(remotePath, err))
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0090, remotePath.c_str())});
    if (!fetch)
        return true;
    dep->resolvedLocation = remotePath;
    return fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName);
}

bool ModuleDepManager::fetchModuleCfgDisk(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch)
{
    Path remotePath = dep->locationParam;
    remotePath.append(SWAG_MODULES_FOLDER);
    remotePath.append(dep->name);

    remotePath = std::filesystem::absolute(remotePath);
    std::error_code err;
    const auto      remotePath1 = std::filesystem::canonical(remotePath, err);
    if (!err)
        remotePath = remotePath1;
    if (!std::filesystem::exists(remotePath, err))
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0090, remotePath.c_str())});
    if (!fetch)
        return true;

    dep->resolvedLocation = remotePath;
    return fetchModuleCfgLocal(dep, cfgFilePath, cfgFileName);
}

bool ModuleDepManager::fetchModuleCfg(ModuleDependency* dep, Path& cfgFilePath, Utf8& cfgFileName, bool fetch)
{
    if (dep->location.empty())
        return Report::report({dep->node, formErr(Err0728, dep->name.c_str())});

    Vector<Utf8> tokens;
    Utf8::tokenize(dep->location, '@', tokens);
    if (tokens.size() != 2)
    {
        if (dep->isLocalToWorkspace)
            return Report::report({dep->node, toErr(Err0227)});
        return Report::report({dep->node, dep->tokenLocation, toErr(Err0227)});
    }

    // Check mode
    if (tokens[0] != g_LangSpec->name_swag && tokens[0] != g_LangSpec->name_disk)
        return Report::report({dep->node, dep->tokenLocation, formErr(Err0158, tokens[0].c_str())});
    dep->locationParam = tokens[1];

    cfgFilePath.clear();
    cfgFileName.clear();

    // Local module, as a filepath
    if (tokens[0] == g_LangSpec->name_disk)
    {
        dep->fetchKind = DependencyFetchKind::Disk;
        return fetchModuleCfgDisk(dep, cfgFilePath, cfgFileName, fetch);
    }

    // Direct access to compiler std workspace
    if (tokens[0] == g_LangSpec->name_swag)
    {
        dep->fetchKind = DependencyFetchKind::Swag;
        return fetchModuleCfgSwag(dep, cfgFilePath, cfgFileName, fetch);
    }

    SWAG_ASSERT(false);
    return false;
}

bool ModuleDepManager::resolveModuleDependency(const Module* srcModule, ModuleDependency* dep)
{
    // If location dependency is not defined, then we take the remote location of the module
    // with that dependency
    if (dep->location.empty())
    {
        dep->isLocalToWorkspace   = srcModule->isLocalToWorkspace;
        dep->location             = srcModule->remoteLocationDep;
        dep->locationAutoResolved = true;
    }

    // Module not here : add it.
    if (!dep->module)
    {
        dep->module       = Allocator::alloc<Module>();
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

    const auto cfgModule = dep->module;

    // If this is the first time, that means that we compare the requested dependency with the local version of the
    // module.
    if (!cfgModule->fetchDep)
    {
        cfgModule->fetchDep = dep;

        const auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, cfgModule->buildCfg.moduleVersion, cfgModule->buildCfg.moduleRevision,
                                         cfgModule->buildCfg.moduleBuildNum);
        switch (cmp)
        {
            // If the dependency requests a bigger version, then we will have to fetch the dependency configuration from the
            // remote path.
            case CompareVersionResult::VersionGreater:
            case CompareVersionResult::RevisionGreater:
            case CompareVersionResult::BuildNumGreater:
                if (cfgModule->wasAddedDep || g_CommandLine.computeDep)
                {
                    cfgModule->mustFetchDep = true;
                    pendingCfgModules.insert(cfgModule);
                }

                break;

            // If the dependency does not specify something, that means that we don't know if we are up to date.
            // In that case, if g_CommandLine.computeDep is true, we will have to fetch dependency configuration file
            // to get the one that corresponds to the dependency (UINT32_MAX means 'latest')
            case CompareVersionResult::Equal:
                if (g_CommandLine.computeDep)
                {
                    if (dep->verNum == UINT32_MAX || dep->revNum == UINT32_MAX || dep->buildNum == UINT32_MAX)
                    {
                        pendingCfgModules.insert(cfgModule);
                    }
                }
                break;

            default:
                break;
        }
    }

    // Here, we have two dependencies requests in different modules. So we compare both to keep the 'more recent' one.
    // If a dependency wants version 1.? and another wants 2.?, then this is an error as it is invalid to depend on
    // two different major versions.
    else if (cfgModule->fetchDep != dep)
    {
        const auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, cfgModule->fetchDep->verNum, cfgModule->fetchDep->revNum, cfgModule->fetchDep->buildNum);
        switch (cmp)
        {
            case CompareVersionResult::VersionGreater:
            case CompareVersionResult::VersionLower:
            {
                Diagnostic err{dep->node, formErr(Err0725, dep->name.c_str(), dep->verNum, cfgModule->fetchDep->verNum)};
                err.addNote(cfgModule->fetchDep->node, toNte(Nte0075));
                Report::report(err);
                return false;
            }

            case CompareVersionResult::RevisionGreater:
            case CompareVersionResult::BuildNumGreater:
                cfgModule->fetchDep = dep;
                if (cfgModule->wasAddedDep || g_CommandLine.computeDep)
                {
                    cfgModule->mustFetchDep = true;
                    pendingCfgModules.insert(cfgModule);
                }
                break;

            case CompareVersionResult::Equal:
                // If version is more specific, then take it
                if ((cfgModule->fetchDep->verNum == UINT32_MAX && dep->verNum != UINT32_MAX) ||
                    (cfgModule->fetchDep->revNum == UINT32_MAX && dep->revNum != UINT32_MAX) ||
                    (cfgModule->fetchDep->buildNum == UINT32_MAX && dep->buildNum != UINT32_MAX))
                {
                    cfgModule->fetchDep = dep;
                    pendingCfgModules.insert(cfgModule);
                }

                break;

            default:
                break;
        }
    }

    return true;
}

CompareVersionResult ModuleDepManager::compareVersions(uint32_t depVer, uint32_t depRev, uint32_t devBuildNum, uint32_t modVer, uint32_t modRev, uint32_t modBuildNum)
{
    if (depVer != UINT32_MAX && modVer != UINT32_MAX && depVer > modVer)
        return CompareVersionResult::VersionGreater;
    if (depVer != UINT32_MAX && modVer != UINT32_MAX && depVer < modVer)
        return CompareVersionResult::VersionLower;

    if (depRev != UINT32_MAX && modRev != UINT32_MAX && depRev < modRev)
        return CompareVersionResult::RevisionLower;
    if (depRev != UINT32_MAX && modRev != UINT32_MAX && depRev > modRev)
        return CompareVersionResult::RevisionGreater;

    if (devBuildNum != UINT32_MAX && modBuildNum != UINT32_MAX && devBuildNum > modBuildNum)
        return CompareVersionResult::BuildNumGreater;
    if (devBuildNum != UINT32_MAX && modBuildNum != UINT32_MAX && devBuildNum < modBuildNum)
        return CompareVersionResult::BuildNumLower;

    return CompareVersionResult::Equal;
}

bool ModuleDepManager::execute()
{
#ifdef SWAG_STATS
    Timer timer(&g_Stats.cfgTime);
#endif

    // Enumerate existing configuration files, and do
    // syntax/semantic for all of them.
    // In this pass, only the #dependencies block will
    // be evaluated
    //////////////////////////////////////////////////
    if (!g_CommandLine.scriptCommand)
    {
        enumerateCfgFiles(g_Workspace->dependenciesPath);
        enumerateCfgFiles(g_Workspace->modulesPath);
        if (g_CommandLine.test || g_CommandLine.listDepCmd || g_CommandLine.fetchDep)
            enumerateCfgFiles(g_Workspace->testsPath);
    }

    // When this is a simple script, then register the script file as the configuration file
    else
    {
        const auto file = Allocator::alloc<SourceFile>();
        file->path      = g_CommandLine.fileName;
        file->name      = file->path.filename();
        file->addFlag(FILE_CFG | FILE_SCRIPT);
        registerCfgFile(file);
    }

    g_ThreadMgr.waitEndJobs();
    Workspace::checkPendingJobs();
    if (g_Workspace->numErrors)
        return false;

    // Script mode. We need to have a workspace in that cache to store
    // the dependencies.
    if (g_CommandLine.scriptCommand)
    {
        // We want each script with the same set of dependencies to have the same
        // cache folder. So we compute a CRC that will be used in the workspace name.
        Utf8 strCrc;
        for (const auto val : allModules | std::views::values)
        {
            for (const auto dep : val->moduleDependencies)
            {
                strCrc += dep->name;
                strCrc += form("%d.%d.%d", dep->verNum, dep->revNum, dep->buildNum);
            }
        }

        const auto crc = strCrc.hash();
        g_Workspace->setScriptWorkspace(form("%s-%08x", SWAG_SCRIPT_WORKSPACE, crc));
        g_Workspace->setupTarget();

        enumerateCfgFiles(g_Workspace->dependenciesPath);
        g_ThreadMgr.waitEndJobs();
        if (g_Workspace->numErrors)
            return false;
    }

    // Remember the configuration of the local modules
    //////////////////////////////////////////////////
    for (const auto val : allModules | std::views::values)
    {
        const auto module   = val;
        module->localCfgDep = module->buildCfg;
    }

    // Populate the list of all modules dependencies,
    // until everything is done
    //////////////////////////////////////////////////
    bool ok = true;
    while (ok)
    {
        pendingCfgModules.clear();
        for (const auto val : allModules | std::views::values)
        {
            const auto parentModule = val;

            // Do not treat dependencies of a module without a remote location.
            // The remote location will be initialized when known, starting with
            // local modules
            if (parentModule->remoteLocationDep.empty())
                continue;

            for (const auto dep : parentModule->moduleDependencies)
            {
                // We need to be sure that the dependency declaration is correct
                if (!dep->location.empty() && !dep->locationAutoResolved)
                {
                    Path cfgFilePath;
                    Utf8 cfgFileName;
                    SWAG_CHECK(fetchModuleCfg(dep, cfgFilePath, cfgFileName, false));
                }

                dep->module = getCfgModule(dep->name);
                ok &= resolveModuleDependency(parentModule, dep);
            }
        }

        if (!ok)
            break;
        if (pendingCfgModules.empty())
            break;

        // We have modules to parse again
        for (const auto cfgModule : pendingCfgModules)
        {
            // Nothing to fetch if this is module already there
            if (cfgModule->isLocalToWorkspace)
                continue;

            // Get the remote config file in cache (if it exists), that depends on the dependency
            Path cfgFilePath;
            Utf8 cfgFileName;
            SWAG_CHECK(fetchModuleCfg(cfgModule->fetchDep, cfgFilePath, cfgFileName, true));

            cfgModule->files.clear(); // leak

            auto file = Allocator::alloc<SourceFile>();
            cfgModule->files.push_back(file);

            file->name = cfgFileName;
            file->addFlag(FILE_CFG);
            file->module  = cfgModule;
            Path pathFile = cfgFilePath;
            pathFile.append(cfgFileName);
            file->path = pathFile;

            parseCfgFile(cfgModule);
        }

        g_ThreadMgr.waitEndJobs();
        Workspace::checkPendingJobs();
        ok = g_Workspace->numErrors.load() == 0;
    }

    if (!ok)
        return false;

    // Compare versions
    //////////////////////////////////////////////////
    for (const auto val : allModules | std::views::values)
    {
        const auto module = val;
        const auto dep    = module->fetchDep;
        if (!dep)
            continue;

        // Verify that all fetch dep match with the corresponding module
        auto cmp = compareVersions(dep->verNum, dep->revNum, dep->buildNum, module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
        if (cmp != CompareVersionResult::Equal)
        {
            if (dep->resolvedLocation.empty())
            {
                Diagnostic err{dep->node, formErr(Err0727, dep->name.c_str(), dep->version.c_str())};
                Report::report(err);
            }
            else
            {
                Diagnostic err{dep->node, formErr(Err0726, dep->name.c_str(), dep->version.c_str(), dep->resolvedLocation.c_str())};
                Report::report(err);
            }
            ok = false;
        }

        // Compare the fetch version with the original local one. If they do not match, then we must fetch the module
        // content
        if (!module->wasAddedDep && !module->mustFetchDep)
        {
            cmp = compareVersions(module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum, module->buildCfg.moduleVersion,
                                  module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (cmp != CompareVersionResult::Equal)
                module->mustFetchDep = true;
            else if (g_CommandLine.getDepCmd && g_CommandLine.getDepForce)
                module->mustFetchDep = true;
        }
    }

    if (!ok)
        return false;

    // List all dependencies
    //////////////////////////////////////////////////
    if (g_CommandLine.listDepCmd)
    {
        for (const auto val : allModules | std::views::values)
        {
            const auto module = val;
            Utf8       msg;
            if (module->fetchDep)
                msg += form("%d.%d.%d", module->localCfgDep.moduleVersion, module->localCfgDep.moduleRevision, module->localCfgDep.moduleBuildNum);
            else
                msg += form("%d.%d.%d", module->buildCfg.moduleVersion, module->buildCfg.moduleRevision, module->buildCfg.moduleBuildNum);
            if (module->fetchDep && module->fetchDep->fetchKind == DependencyFetchKind::Swag)
                msg += " [swag]";
            else if (module->mustFetchDep)
                msg += form(" => version %d.%d.%d is available",
                            module->buildCfg.moduleVersion,
                            module->buildCfg.moduleRevision,
                            module->buildCfg.moduleBuildNum);
            g_Log.messageHeaderDot(module->name, msg);
        }
    }

    // Fetch all dependencies in the dependencies/ folder of the workspace
    // The copy will occur if
    // 1/ The module is not present (first time)
    // 2/ The module is present but is not up to date
    //////////////////////////////////////////////////
    if (g_CommandLine.fetchDep)
    {
        for (const auto val : allModules | std::views::values)
        {
            if (!val->fetchDep)
                continue;
            if (!val->mustFetchDep)
                continue;

            Job* fetchJob = nullptr;
            switch (val->fetchDep->fetchKind)
            {
                // Copy from the disk, elsewhere
                case DependencyFetchKind::Disk:
                    fetchJob = Allocator::alloc<FetchModuleFileSystemJob>();
                    break;

                // This is a dependency to the swag compiler std workspace
                // No need to make a copy, as we already have the source tree with the compiler
                // So we create a special dependency file named SWAG_ALIAS_FILENAME which will trick the compiler
                // That file will contain the path to the corresponding module location
                case DependencyFetchKind::Swag:
                {
                    std::error_code err;
                    auto            pathSrc = g_Workspace->dependenciesPath;
                    pathSrc.append(val->name.c_str());
                    if (!std::filesystem::exists(pathSrc, err) && !std::filesystem::create_directories(pathSrc, err))
                    {
                        Report::errorOS(formErr(Err0089, pathSrc.c_str()));
                        ok = false;
                        continue;
                    }

                    pathSrc.append(SWAG_ALIAS_FILENAME);

                    FILE* f = nullptr;
                    if (!fopen_s(&f, pathSrc, "wt"))
                    {
                        auto pathDst = val->fetchDep->resolvedLocation;
                        (void) fwrite(pathDst, pathDst.length(), 1, f);
                        (void) fflush(f);
                        (void) fclose(f);
                    }

                    fetchJob = Allocator::alloc<FetchModuleFileSystemJob>();

                    reinterpret_cast<FetchModuleFileSystemJob*>(fetchJob)->collectSourceFiles = false;
                    break;
                }
                default:
                    break;
            }

            SWAG_ASSERT(fetchJob);
            fetchJob->module = val;
            g_ThreadMgr.addJob(fetchJob);
        }

        g_ThreadMgr.waitEndJobs();
        ok = ok && g_Workspace->numErrors.load() == 0;
    }

    // For each modules, flatten all dependencies
    for (const auto val : allModules | std::views::values)
    {
        SetUtf8                          depNames;
        VectorNative<ModuleDependency*>& dep = val->moduleDependencies;

        for (const auto d : val->moduleDependencies)
            depNames.insert(d->module->name);

        for (uint32_t i = 0; i < dep.size(); i++)
        {
            const auto d = dep[i];
            if (!d->module)
                continue;

            for (const auto d1 : d->module->moduleDependencies)
            {
                if (depNames.contains(d1->module->name))
                    continue;
                depNames.insert(d1->module->name);

                ModuleDependency* newDep = Allocator::alloc<ModuleDependency>();
                *newDep                  = *d1;
                dep.push_back(newDep);
            }
        }
    }

    return ok;
}
