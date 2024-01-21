#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleSemanticJob.h"
#include "EnumerateModuleJob.h"
#include "ModuleBuildJob.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Backend.h"
#include "ByteCodeOptimizer.h"
#include "ModuleDepManager.h"
#include "Report.h"
#include "TypeGenStructJob.h"
#include "Naming.h"
#include "SyntaxJob.h"
#include "ModuleManager.h"

void Workspace::computeModuleName(const Path& path, Utf8& moduleName, Path& moduleFolder, ModuleKind& kind)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Be sure module name is valid
    Utf8 errorStr;
    if (!Module::isValidName(cFileName, errorStr))
    {
        errorStr = "fatal error: " + errorStr;
        errorStr += Fmt(" (path is [[%s]])", path.string().c_str());
        Report::error(errorStr);
        OS::exit(-1);
    }

    // Module name is equivalent to the folder name, except for specific folders
    if (parent == SWAG_TESTS_FOLDER)
        moduleName = "test_";

    moduleName += cFileName;

    // Kind
    kind = ModuleKind::Module;
    if (parent == SWAG_TESTS_FOLDER)
        kind = ModuleKind::Test;
    else if (parent == SWAG_DEPENDENCIES_FOLDER)
        kind = ModuleKind::Dependency;
    else if (parent == SWAG_MODULES_FOLDER)
        kind = ModuleKind::Module;
    else
        SWAG_ASSERT(false);

    moduleFolder = path;
}

SourceFile* Workspace::findFile(const char* fileName)
{
    SourceFile* sourceFile = nullptr;
    for (auto m : mapModulesNames)
    {
        sourceFile = m.second->findFile(fileName);
        if (sourceFile)
            return sourceFile;
    }

    if (!sourceFile)
        sourceFile = g_Workspace->bootstrapModule->findFile(fileName);
    if (!sourceFile)
        sourceFile = g_Workspace->runtimeModule->findFile(fileName);
    return sourceFile;
}

Module* Workspace::getModuleByName(const Utf8& moduleName)
{
    SharedLock lk(mutexModules);
    auto       it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const Utf8& moduleName, const Path& modulePath, ModuleKind kind, bool errorModule)
{
    Module* module = Allocator::alloc<Module>();

    {
        ScopedLock lk(mutexModules);

#ifdef SWAG_DEV_MODE
        auto it = mapModulesNames.find(moduleName);
        SWAG_ASSERT(it == mapModulesNames.end());
#endif

        modules.push_back(module);
        mapModulesNames[moduleName] = module;
        module->isErrorModule       = errorModule;
    }

    module->kind = kind;
    module->setup(moduleName, modulePath);

    // Setup from the config module, if it exists
    auto cfgModule = g_ModuleCfgMgr->getCfgModule(moduleName);
    if (cfgModule)
    {
        // :GetCfgFileParams
        module->buildCfg                     = cfgModule->buildCfg;
        module->buildParameters.foreignLibs  = cfgModule->buildParameters.foreignLibs;
        module->buildParameters.globalUsings = cfgModule->buildParameters.globalUsings;
        module->moduleDependencies           = cfgModule->moduleDependencies;
        module->compilerLoads                = cfgModule->compilerLoads;
        module->docComment                   = std::move(cfgModule->docComment);
    }

    // Is this the module we want to build ?
    if (g_CommandLine.moduleName == moduleName)
    {
        filteredModule = module;
        if (g_CommandLine.run)
            runModule = filteredModule;
    }

#ifdef SWAG_STATS
    g_Stats.numModules++;
#endif

    return module;
}

void Workspace::addBootstrap()
{
    // Bootstrap will be compiled in the workspace scope, in order to be defined once
    // for all modules
    bootstrapModule       = Allocator::alloc<Module>();
    bootstrapModule->kind = ModuleKind::BootStrap;
    bootstrapModule->setup("bootstrap", "");
    modules.push_back(bootstrapModule);

    auto file  = Allocator::alloc<SourceFile>();
    file->name = "bootstrap.swg";
    file->path = g_CommandLine.exePath.parent_path();
    file->path.append("runtime");
    file->path.append("bootstrap.swg");
    file->module          = bootstrapModule;
    file->isBootstrapFile = true;
    bootstrapModule->addFile(file);
}

void Workspace::addRuntimeFile(const char* fileName)
{
    auto file  = Allocator::alloc<SourceFile>();
    file->name = fileName;
    file->path = g_CommandLine.exePath.parent_path();
    file->path.append("runtime");
    file->path.append(fileName);
    file->module        = runtimeModule;
    file->isRuntimeFile = true;
    runtimeModule->addFile(file);
}

void Workspace::addRuntime()
{
    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    runtimeModule       = Allocator::alloc<Module>();
    runtimeModule->kind = ModuleKind::Runtime;
    runtimeModule->setup("runtime", "");
    modules.push_back(runtimeModule);

    addRuntimeFile("runtime.swg");
    addRuntimeFile("runtime_err.swg");
    addRuntimeFile("runtime_str.swg");
    addRuntimeFile("runtime_windows.swg");
    addRuntimeFile("systemallocator.swg");
    addRuntimeFile("scratchallocator.swg");
    addRuntimeFile("debugallocator.swg");
}

Utf8 Workspace::getTargetFullName(const Utf8& buildCfg, const BackendTarget& target)
{
    return buildCfg + "-" + Backend::getOsName(target) + "-" + Backend::getArchName(target);
}

Path Workspace::getTargetPath(const Utf8& buildCfg, const BackendTarget& target)
{
    Path p = workspacePath;
    p.append(SWAG_OUTPUT_FOLDER);
    p.append(getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target).c_str());
    return p;
}

void Workspace::setupTarget()
{
    // Target directory
    targetPath = getTargetPath(g_CommandLine.buildCfg, g_CommandLine.target);
    if (g_CommandLine.verbosePath)
        g_Log.messageVerbose(Fmt("target path is [[%s]]", targetPath.string().c_str()));

    error_code err;
    if (!filesystem::exists(targetPath, err) && !filesystem::create_directories(targetPath, err))
    {
        Report::errorOS(Fmt(Err(Fat0021), targetPath.string().c_str()));
        OS::exit(-1);
    }

    // Cache directory
    setupCachePath();
    if (!filesystem::exists(cachePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0010), cachePath.string().c_str()));
        OS::exit(-1);
    }

    cachePath.append(SWAG_CACHE_FOLDER);
    if (!filesystem::exists(cachePath, err) && !filesystem::create_directories(cachePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0016), cachePath.string().c_str()));
        OS::exit(-1);
    }

    auto targetFullName = getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
    cachePath.append(workspacePath.filename().string() + "-" + targetFullName.c_str());
    if (!filesystem::exists(cachePath, err) && !filesystem::create_directories(cachePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0016), cachePath.string().c_str()));
        OS::exit(-1);
    }

    if (g_CommandLine.verbosePath)
        g_Log.messageVerbose(Fmt("cache path is [[%s]]", cachePath.string().c_str()));
}

Diagnostic* Workspace::errorPendingJob(Job* prevJob, Job* depJob)
{
    AstNode* prevNodeLocal = prevJob->nodes.empty() ? prevJob->originalNode : prevJob->nodes.back();
    SWAG_ASSERT(prevNodeLocal);

    // :JobNodeIsFile
    if (prevNodeLocal && prevNodeLocal->kind == AstNodeKind::File)
        return nullptr;

    AstNode* prevNode = nullptr;
    if (!prevJob->originalNode)
        prevNode = prevNodeLocal;
    else if (prevJob->originalNode->kind == AstNodeKind::VarDecl ||
             prevJob->originalNode->kind == AstNodeKind::ConstDecl ||
             prevJob->originalNode->kind == AstNodeKind::TypeAlias ||
             prevJob->originalNode->kind == AstNodeKind::NameAlias ||
             prevJob->originalNode->kind == AstNodeKind::StructDecl ||
             prevJob->originalNode->kind == AstNodeKind::EnumDecl)
        prevNode = prevJob->originalNode;
    else
        prevNode = prevNodeLocal;

    AstNode* depNode = nullptr;
    if (depJob)
        depNode = depJob->originalNode;

    SWAG_ASSERT(prevNode);

    Utf8 msg;
    Utf8 hint;

    if (depNode)
    {
        msg = Fmt(Nte(Nte0110), Naming::kindName(prevNode).c_str(), prevNode->token.ctext(), Naming::kindName(depNode).c_str(), depNode->token.ctext());
    }
    else if (prevNode && prevJob->waitingType)
    {
        msg  = Fmt(Nte(Nte0055), Naming::kindName(prevNode).c_str(), prevNode->token.ctext(), prevJob->waitingType->getDisplayNameC());
        hint = Diagnostic::isType(prevNode->typeInfo);
    }
    else if (prevJob->waitingType && dynamic_cast<TypeGenStructJob*>(prevJob))
    {
        msg      = Fmt(Nte(Nte0187), prevJob->waitingType->getDisplayNameC());
        prevNode = prevJob->waitingType->declNode;
    }
    else if (prevJob->waitingType)
    {
        msg = Fmt(Nte(Nte0187), prevJob->waitingType->getDisplayNameC());
    }
    else
    {
        msg  = Fmt(Nte(Nte0186), Naming::kindName(prevNode).c_str(), prevNode->token.ctext());
        hint = Diagnostic::isType(prevNode->typeInfo);
    }

    if (prevJob->waitingHintNode)
    {
        prevNodeLocal = prevJob->waitingHintNode;
        switch (prevJob->waitingHintNode->kind)
        {
        case AstNodeKind::ConstDecl:
        case AstNodeKind::VarDecl:
        case AstNodeKind::EnumValue:
            msg += " ";
            msg += Fmt("because of %s [[%s]]", Naming::kindName(prevJob->waitingHintNode).c_str(), prevJob->waitingHintNode->token.ctext());
            break;
        default:
            break;
        }
    }

    auto note         = Diagnostic::note(prevNodeLocal, prevNodeLocal->token, msg);
    note->canBeMerged = false;
    note->hint        = hint;

    Utf8 remark, sym;
    if (prevJob->waitingSymbolSolved)
        sym = Fmt("%s [[%s]]", Naming::kindName(prevJob->waitingSymbolSolved->kind).c_str(), prevJob->waitingSymbolSolved->getFullName().c_str());

    switch (prevJob->waitingKind)
    {
    case JobWaitKind::WaitMethods:
        remark = "waiting for all methods to be solved";
        break;
    case JobWaitKind::WaitInterfaces:
        remark = "waiting for all interfaces to be solved";
        break;
    case JobWaitKind::GenExportedType:
    case JobWaitKind::GenExportedType1:
        remark = "waiting for the type to be exported";
        break;
    case JobWaitKind::SemFullResolve:
        remark = Fmt("waiting for %s to be fully solved", sym.c_str());
        break;
    case JobWaitKind::WaitSymbol:
        remark = Fmt("waiting for %s to be solved", sym.c_str());
        break;
    default:
        break;
    }

    if (!remark.empty())
        note->remarks.push_back(remark);

    return note;
}

bool errorPendingCycle(Job* pendingJob, VectorNative<Job*>& waitingJobs, Set<Job*>& done, VectorNative<Job*>& cycle)
{
    if (waitingJobs.empty())
        return false;

    for (auto depJob : waitingJobs)
    {
        if (depJob == pendingJob)
            return true;

        if (done.find(depJob) != done.end())
            return false;
        done.insert(depJob);

        if (errorPendingCycle(pendingJob, depJob->waitingJobs, done, cycle))
        {
            cycle.push_front(depJob);
            return true;
        }
    }

    return false;
}

void Workspace::errorPendingJobs(Vector<PendingJob>& pendingJobs)
{
    for (auto& it : pendingJobs)
    {
        auto pendingJob = it.pendingJob;
        auto node       = it.node;

        if (node->sourceFile->module->hasCycleError)
            continue;

        // Is there a dependency cycle ?
        Set<Job*>          done;
        VectorNative<Job*> cycle;
        bool               isCycle = errorPendingCycle(pendingJob, pendingJob->waitingJobs, done, cycle);
        if (isCycle)
        {
            Vector<const Diagnostic*> notes;
            auto                      prevJob = pendingJob;

            for (size_t idxJob = 0; idxJob < cycle.size(); idxJob++)
            {
                if (prevJob->nodes.size() > 1 && prevJob->originalNode->kind == AstNodeKind::FuncDecl)
                {
                    auto front        = prevJob->nodes.front();
                    auto back         = prevJob->nodes.back();
                    auto msg          = Fmt(Nte(Nte0110), Naming::kindName(front).c_str(), front->token.ctext(), Naming::kindName(back).c_str(), back->token.ctext());
                    auto note         = Diagnostic::note(back, back->token, msg);
                    note->canBeMerged = false;
                    note->hint        = Diagnostic::isType(back->typeInfo);
                    notes.push_back(note);
                }

                auto depJob = cycle[idxJob];
                auto note   = errorPendingJob(prevJob, depJob);
                if (note)
                    notes.push_back(note);
                prevJob = depJob;
            }

            auto note = errorPendingJob(prevJob, pendingJob);
            if (note)
                notes.push_back(note);

            auto       prevNodeLocal = pendingJob->originalNode ? pendingJob->originalNode : pendingJob->nodes.front();
            Diagnostic diag{prevNodeLocal, prevNodeLocal->token, Fmt(Err(Err0624), Naming::kindName(prevNodeLocal).c_str(), prevNodeLocal->token.ctext())};
            Report::report(diag, notes);
            auto sourceFile                   = Report::getDiagFile(diag);
            sourceFile->module->hasCycleError = true;
            continue;
        }

        // Job is not done, and we do not wait for a specific other job...
        if (pendingJob->waitingJobs.empty())
        {
            auto note = errorPendingJob(pendingJob, nullptr);
            if (!note)
                continue;
            Diagnostic diag{note->sourceFile, note->startLocation, note->endLocation, Err(Err0087)};
            Report::report(diag, note);
        }
    }
}

void Workspace::computeWaitingJobs()
{
    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        for (auto dep : pendingJob->dependentJobs.list)
        {
            dep->waitingJobs.push_back_once(pendingJob);
        }

        for (auto dep : pendingJob->jobsToAdd)
        {
            dep->waitingJobs.push_back_once(pendingJob);
        }

        if (pendingJob->dependentJob)
        {
            pendingJob->dependentJob->waitingJobs.push_back_once(pendingJob);
        }

        switch (pendingJob->waitingKind)
        {
        case JobWaitKind::WaitSymbol:
            SWAG_ASSERT(pendingJob->waitingSymbolSolved);
            for (int i = 0; i < (int) g_ThreadMgr.waitingJobs.size(); i++)
            {
                auto it = g_ThreadMgr.waitingJobs[i];
                for (auto it1 : pendingJob->waitingSymbolSolved->nodes)
                {
                    if (it->originalNode == it1)
                    {
                        pendingJob->waitingJobs.push_back_once(it);
                    }
                }
            }
            break;

        case JobWaitKind::GenExportedType:
        case JobWaitKind::GenExportedType1:
            SWAG_ASSERT(pendingJob->waitingType);
            for (int i = 0; i < (int) g_ThreadMgr.waitingJobs.size(); i++)
            {
                auto it = dynamic_cast<TypeGenStructJob*>(g_ThreadMgr.waitingJobs[i]);
                if (it && it->typeInfo == pendingJob->waitingType)
                {
                    pendingJob->waitingJobs.push_back_once(it);
                    break;
                }
            }
            break;

        default:
            if (pendingJob->waitingNode)
            {
                for (int i = 0; i < (int) g_ThreadMgr.waitingJobs.size(); i++)
                {
                    auto it = g_ThreadMgr.waitingJobs[i];
                    if (it->originalNode == pendingJob->waitingNode)
                    {
                        pendingJob->waitingJobs.push_back_once(it);
                        break;
                    }
                }
                break;
            }

            if (pendingJob->waitingType)
            {
                for (int i = 0; i < (int) g_ThreadMgr.waitingJobs.size(); i++)
                {
                    auto it = g_ThreadMgr.waitingJobs[i];
                    if (it->originalNode == pendingJob->waitingType->declNode)
                    {
                        pendingJob->waitingJobs.push_back_once(it);
                        break;
                    }
                }
                break;
            }

            break;
        }
    }
}

void Workspace::checkPendingJobs()
{
    if (g_ThreadMgr.waitingJobs.empty())
        return;

    computeWaitingJobs();

    // Collect unsolved jobs
    Vector<PendingJob> pendingJobs;
    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        auto sourceFile = pendingJob->sourceFile;
        if (!sourceFile)
            continue;

        // Do not generate errors if we already have some errors
        if (sourceFile->module->numErrors)
            continue;

        // Get the node the job was trying to resolve
        AstNode* node = nullptr;
        if (!pendingJob->nodes.empty())
            node = pendingJob->nodes.back();
        if (!node)
            node = pendingJob->originalNode;
        if (!node)
            continue;
        if (node->kind == AstNodeKind::FuncDeclType)
            node = node->parent;

        PendingJob pj;
        pj.pendingJob = pendingJob;
        pj.node       = node;
        pendingJobs.push_back(pj);
    }

    errorPendingJobs(pendingJobs);
}

bool Workspace::buildRTModule(Module* module)
{
    for (auto f : module->files)
    {
        auto job        = Allocator::alloc<SyntaxJob>();
        job->sourceFile = f;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    if (module->numErrors)
    {
        Report::error(module->kind == ModuleKind::BootStrap ? Err(Fat0014) : Err(Fat0015));
        return false;
    }

    auto job    = Allocator::alloc<ModuleSemanticJob>();
    job->module = module;
    g_ThreadMgr.addJob(job);
    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();

    ByteCodeOptimizer opt;
    bool              done;
    opt.optimize(nullptr, module, done);
    g_ThreadMgr.waitEndJobs();

    // Errors !!!
    if (module->numErrors)
    {
        Report::error(module->kind == ModuleKind::BootStrap ? Err(Fat0014) : Err(Fat0015));
        return false;
    }

    module->filterFunctionsToEmit();
    return true;
}

bool Workspace::buildTarget()
{
    // Load as much files as we can during setup stage
    // to void wasting some cores
    //////////////////////////////////////////////////

    if (g_ThreadMgr.numWorkers != 1 && !g_CommandLine.scriptCommand)
    {
        auto enumJob0          = Allocator::alloc<EnumerateModuleJob>();
        enumJob0->readFileMode = true;
        g_ThreadMgr.addJob(enumJob0);
    }

    // Bootstrap module semantic pass
    //////////////////////////////////////////////////
    {
        bootstrapModule->logStage("buildRTModule\n");

#ifdef SWAG_STATS
        Timer timer(&g_Stats.bootstrapTime);
#endif

        SWAG_CHECK(buildRTModule(bootstrapModule));
    }

    // Runtime module semantic pass
    //////////////////////////////////////////////////
    {
        runtimeModule->logStage("buildRTModule\n");

#ifdef SWAG_STATS
        Timer timer(&g_Stats.runtimeTime);
#endif

        runtimeModule->initFrom(bootstrapModule);
        SWAG_CHECK(buildRTModule(runtimeModule));
    }

    // Wait for optional jobs running to finish, remove
    // the rest
    //////////////////////////////////////////////////
    g_ThreadMgr.waitEndJobs();
    g_ThreadMgr.clearOptionalJobs();
    bootstrapModule->printBC();
    runtimeModule->printBC();

    // Config pass (compute/fetch dependencies...
    //////////////////////////////////////////////////
    g_ModuleCfgMgr = Allocator::alloc<ModuleDepManager>();
    SWAG_CHECK(g_ModuleCfgMgr->execute());

    // Exit now (do not really build) in case of "get", "list" commands
    if (g_CommandLine.listDepCmd || g_CommandLine.getDepCmd)
        return true;

    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    auto enumJob1          = Allocator::alloc<EnumerateModuleJob>();
    enumJob1->readFileMode = false;
    g_ThreadMgr.addJob(enumJob1);
    g_ThreadMgr.waitEndJobs();
    if (g_Workspace->numErrors)
        return false;

    // Filter modules to build
    //////////////////////////////////////////////////
    auto toBuild = modules;
    if (!g_CommandLine.moduleName.empty())
    {
        if (!filteredModule)
        {
            Report::error(Fmt(Err(Fat0029), g_CommandLine.moduleName.c_str()));
            return false;
        }

        toBuild.clear();
        toBuild.push_back(filteredModule);
        filteredModule->addedToBuild = true;
        for (size_t i = 0; i < toBuild.size(); i++)
        {
            for (auto& dep : toBuild[i]->moduleDependencies)
            {
                ScopedLock lk(mutexModules);
                auto       it = g_Workspace->mapModulesNames.find(dep->name);
                if (it == g_Workspace->mapModulesNames.end())
                {
                    Report::error(Fmt(Err(Fat0011), dep->name.c_str()));
                    return false;
                }

                auto depModule = it->second;
                if (filteredModule->moduleEmbedded.contains(depModule))
                    continue;
                if (depModule->addedToBuild)
                    continue;
                toBuild.push_back(depModule);
                depModule->addedToBuild = true;
            }
        }
    }

    // Build modules
    //////////////////////////////////////////////////
    {
        SharedLock lk(mutexModules);
        for (auto module : toBuild)
            module->addedToBuild = true;
        for (auto module : toBuild)
        {
            if (module == bootstrapModule || module == runtimeModule)
                continue;
            if (module->isErrorModule)
                continue;
            auto job    = Allocator::alloc<ModuleBuildJob>();
            job->module = module;
            g_ThreadMgr.addJob(job);
        }
    }

    // Restart if necessary until it's no more possible
    //////////////////////////////////////////////////
    while (true)
    {
        g_ThreadMgr.waitEndJobs();

        bool restart     = false;
        auto waitingJobs = g_ThreadMgr.waitingJobs;
        for (size_t i = 0; i < waitingJobs.size(); i++)
        {
            auto job = waitingJobs[i];
            if (job->flags & JOB_PENDING_PLACE_HOLDER)
            {
                SWAG_ASSERT(!(job->flags & JOB_IS_IN_THREAD));
                job->flags |= JOB_ACCEPT_PENDING_COUNT;
                waitingJobs.erase_unordered(i);
                i--;
                restart = true;
                g_ThreadMgr.addJob(job);
            }
        }

        if (!restart)
        {
            checkPendingJobs();
            break;
        }
    }

    return true;
}

bool Workspace::build()
{
    if (!OS::setupBuild())
        return false;

    g_ModuleMgr = Allocator::alloc<ModuleManager>();
    g_LangSpec  = Allocator::alloc<LanguageSpec>();
    g_LangSpec->setup();
    Backend::setup();

    // [devmode] stuff
#if defined SWAG_DEBUG
    g_Log.messageHeaderCentered("Compiler", "[debug]", LogColor::DarkBlue, LogColor::DarkBlue);
#elif defined SWAG_DEV_MODE
    g_Log.messageHeaderCentered("Compiler", "[devmode]", LogColor::DarkBlue, LogColor::DarkBlue);
#elif defined SWAG_STATS
    g_Log.messageHeaderCentered("Compiler", "[stats]", LogColor::DarkBlue, LogColor::DarkBlue);
#endif

    // [devmode] randomize/seed
#ifdef SWAG_DEV_MODE
    if (g_CommandLine.randomize)
    {
        if (!g_CommandLine.randSeed)
        {
            using namespace std::chrono;
            milliseconds ms        = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            g_CommandLine.randSeed = (int) ms.count() & 0x7FFFFFFF;
            srand(g_CommandLine.randSeed);
            g_CommandLine.randSeed = rand() & 0x7FFFFFFF;
        }

        srand(g_CommandLine.randSeed);
        g_Log.setColor(LogColor::DarkBlue);
        g_Log.print(Fmt("[devmode] randomize seed is %d\n", g_CommandLine.randSeed));
        g_Log.setDefaultColor();
    }
#endif

    // User arguments that can be retrieved with '@args'
    pair<void*, void*> oneArg;
    g_CommandLine.exePathStr = g_CommandLine.exePath.string();
    oneArg.first             = (void*) g_CommandLine.exePathStr.c_str();
    oneArg.second            = (void*) (size_t) g_CommandLine.exePathStr.length();
    g_CommandLine.userArgumentsStr.push_back(oneArg);

    Utf8::tokenizeBlanks(g_CommandLine.userArguments, g_CommandLine.userArgumentsVec);
    for (auto& arg : g_CommandLine.userArgumentsVec)
    {
        oneArg.first  = (void*) arg.buffer;
        oneArg.second = (void*) (size_t) arg.length();
        g_CommandLine.userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine.userArgumentsSlice.first  = &g_CommandLine.userArgumentsStr[0];
    g_CommandLine.userArgumentsSlice.second = (void*) g_CommandLine.userArgumentsStr.size();

    // Build !
    auto result = true;

    {
        Timer timer(&g_Workspace->totalTime, true);

        setup();

        if (!g_CommandLine.scriptCommand)
        {
            if (g_CommandLine.verbosePath)
                g_Log.messageVerbose(Fmt("workspace path is [[%s]]", workspacePath.string().c_str()));
            if (g_CommandLine.listDepCmd || g_CommandLine.getDepCmd)
                g_Log.messageHeaderCentered("Workspace", workspacePath.filename().string().c_str());
            else
            {
                auto targetFullName = g_Workspace->getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
                g_Log.messageHeaderCentered("Workspace", Fmt("%s [%s]", workspacePath.filename().string().c_str(), targetFullName.c_str()));
            }
        }
        else
        {
            auto targetFullName = g_Workspace->getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
            Path p              = g_CommandLine.scriptName;
            g_Log.messageHeaderCentered("Script", Fmt("%s [%s]", p.filename().string().c_str(), targetFullName.c_str()));
        }

        addBootstrap();
        addRuntime();
        if (!g_CommandLine.scriptCommand)
            setupTarget();
        result = buildTarget();
    }

    // Results
    if (!g_CommandLine.scriptCommand)
    {
        if (g_Workspace->skippedModules.load() > 0)
            g_Log.messageHeaderCentered("Skipped modules", Fmt("%d", g_Workspace->skippedModules.load()));

        if (g_Workspace->numErrors.load() == 1)
            g_Log.messageHeaderCentered("Done", Fmt("%d error", g_Workspace->numErrors.load()), LogColor::Green, LogColor::Red);
        else if (g_Workspace->numErrors.load())
            g_Log.messageHeaderCentered("Done", Fmt("%d errors", g_Workspace->numErrors.load()), LogColor::Green, LogColor::Red);
        else if (g_Workspace->numWarnings.load() == 1)
            g_Log.messageHeaderCentered("Done", Fmt("%.3fs (%d warning)", OS::timerToSeconds(g_Workspace->totalTime.load()), g_Workspace->numWarnings.load()), LogColor::Green, LogColor::Magenta);
        else if (g_Workspace->numWarnings.load())
            g_Log.messageHeaderCentered("Done", Fmt("%.3fs (%d warnings)", OS::timerToSeconds(g_Workspace->totalTime.load()), g_Workspace->numWarnings.load()), LogColor::Green, LogColor::Magenta);
        else
            g_Log.messageHeaderCentered("Done", Fmt("%.3fs", OS::timerToSeconds(g_Workspace->totalTime.load())));
    }

    return result;
}
