#include "pch.h"
#include "Backend/Backend.h"
#include "Backend/ByteCode/Optimize/ByteCodeOptimizer.h"
#include "Core/Timer.h"
#include "Jobs/EnumerateModuleJob.h"
#include "Jobs/ModuleBuildJob.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/ModuleSemanticJob.h"
#include "Semantic/Type/TypeGenStructJob.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Naming.h"
#include "Syntax/SyntaxJob.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/DepManager/ModuleDepManager.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"
#ifdef SWAG_STATS
#include "Main/Statistics.h"
#endif

void Workspace::computeModuleName(const Path& path, Utf8& moduleName, Path& moduleFolder, ModuleKind& kind)
{
    const auto parent    = path.parent_path().filename();
    const auto cFileName = path.filename();

    // Be sure module name is valid
    Utf8 errorStr;
    if (!Module::isValidName(cFileName, errorStr))
    {
        errorStr = "fatal error: " + errorStr;
        errorStr += form(" (path is [[%s]])", path.c_str());
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

Module* Workspace::getModuleByName(const Utf8& moduleName)
{
    SharedLock lk(mutexModules);
    const auto it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

SourceFile* Workspace::getFileByName(const char* fileName) const
{
    SourceFile* sourceFile = nullptr;
    for (const auto& val : mapModulesNames | std::views::values)
    {
        sourceFile = val->findFile(fileName);
        if (sourceFile)
            return sourceFile;
    }

    if (!sourceFile)
        sourceFile = g_Workspace->bootstrapModule->findFile(fileName);
    if (!sourceFile)
        sourceFile = g_Workspace->runtimeModule->findFile(fileName);
    return sourceFile;
}

void Workspace::getAllByteCodes(VectorNative<ByteCode*>& result) const
{
    for (const auto m : modules)
    {
        for (const auto bc : m->byteCodeFunc)
        {
            if (!bc->out)
                continue;
            result.push_back(bc);
        }
    }
}

Module* Workspace::createModule(const Utf8& moduleName, const Path& modulePath, ModuleKind kind, bool errorModule)
{
    Module* module = Allocator::alloc<Module>();

    {
        ScopedLock lk(mutexModules);

#ifdef SWAG_DEV_MODE
        const auto it = mapModulesNames.find(moduleName);
        SWAG_ASSERT(it == mapModulesNames.end());
#endif

        modules.push_back(module);
        mapModulesNames[moduleName] = module;
        module->isErrorModule       = errorModule;
    }

    module->kind = kind;
    module->setup(moduleName, modulePath);

    // Setup from the config module, if it exists
    const auto cfgModule = g_ModuleCfgMgr->getCfgModule(moduleName);
    if (cfgModule)
    {
        // :GetCfgFileParams
        module->buildCfg                    = cfgModule->buildCfg;
        module->buildParameters.foreignLibs = cfgModule->buildParameters.foreignLibs;
        module->buildParameters.globalUsing = cfgModule->buildParameters.globalUsing;
        module->moduleDependencies          = cfgModule->moduleDependencies;
        module->compilerLoads               = cfgModule->compilerLoads;
        module->docComment                  = std::move(cfgModule->docComment);
    }

    // Is this the module we want to build ?
    if (g_CommandLine.moduleName == moduleName)
    {
        filteredModule = module;
        if (g_CommandLine.run)
            runModule = filteredModule;
    }

#ifdef SWAG_STATS
    ++g_Stats.numModules;
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

    const auto file = Allocator::alloc<SourceFile>();
    file->name      = "bootstrap.swg";
    file->path      = g_CommandLine.exePath.parent_path();
    file->path.append("runtime");
    file->path.append("bootstrap.swg");
    file->module = bootstrapModule;
    file->addFlag(FILE_BOOTSTRAP);
    bootstrapModule->addFile(file);
}

void Workspace::addRuntimeFile(const char* fileName) const
{
    const auto file = Allocator::alloc<SourceFile>();
    file->name      = fileName;
    file->path      = g_CommandLine.exePath.parent_path();
    file->path.append("runtime");
    file->path.append(fileName);
    file->module = runtimeModule;
    file->addFlag(FILE_RUNTIME);
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

    addRuntimeFile(R"(runtime.swg)");
    addRuntimeFile(R"(runtime_err.swg)");
    addRuntimeFile(R"(runtime_str.swg)");
    addRuntimeFile(R"(runtime_windows.swg)");
    addRuntimeFile(R"(systemallocator.swg)");
    addRuntimeFile(R"(scratchallocator.swg)");
    addRuntimeFile(R"(debugallocator.swg)");
}

Utf8 Workspace::getTargetFullName(const Utf8& buildCfg, const BackendTarget& target)
{
    return buildCfg + "-" + Backend::getOsName(target) + "-" + Backend::getArchName(target);
}

Path Workspace::getTargetPath() const
{
    Path p = workspacePath;
    p.append(SWAG_OUTPUT_FOLDER);
    p.append(getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target).c_str());
    return p;
}

void Workspace::setupTarget()
{
    // Target directory
    targetPath = getTargetPath();
    if (g_CommandLine.verbosePath)
        g_Log.messageVerbose(form("target path is [[%s]]", targetPath.c_str()));

    std::error_code err;
    if (!std::filesystem::exists(targetPath, err) && !std::filesystem::create_directories(targetPath, err))
    {
        Report::errorOS(formErr(Fat0021, targetPath.c_str()));
        OS::exit(-1);
    }

    // Cache directory
    setupCachePath();
    if (!std::filesystem::exists(cachePath, err))
    {
        Report::errorOS(formErr(Fat0010, cachePath.c_str()));
        OS::exit(-1);
    }

    cachePath.append(SWAG_CACHE_FOLDER);
    if (!std::filesystem::exists(cachePath, err) && !std::filesystem::create_directories(cachePath, err))
    {
        Report::errorOS(formErr(Fat0016, cachePath.c_str()));
        OS::exit(-1);
    }

    const auto targetFullName = getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
    cachePath.append(workspacePath.filename() + "-" + targetFullName.c_str());
    if (!std::filesystem::exists(cachePath, err) && !std::filesystem::create_directories(cachePath, err))
    {
        Report::errorOS(formErr(Fat0016, cachePath.c_str()));
        OS::exit(-1);
    }

    if (g_CommandLine.verbosePath)
        g_Log.messageVerbose(form("cache path is [[%s]]", cachePath.c_str()));
}

Diagnostic* Workspace::errorPendingJob(Job* prevJob, const Job* depJob)
{
    AstNode* prevNodeLocal = prevJob->nodes.empty() ? prevJob->originalNode : prevJob->nodes.back();
    SWAG_ASSERT(prevNodeLocal);

    // :JobNodeIsFile
    if (prevNodeLocal && prevNodeLocal->is(AstNodeKind::File))
        return nullptr;

    const AstNode* prevNode = nullptr;
    if (!prevJob->originalNode)
        prevNode = prevNodeLocal;
    else if (prevJob->originalNode->is(AstNodeKind::VarDecl) ||
             prevJob->originalNode->is(AstNodeKind::ConstDecl) ||
             prevJob->originalNode->is(AstNodeKind::TypeAlias) ||
             prevJob->originalNode->is(AstNodeKind::NameAlias) ||
             prevJob->originalNode->is(AstNodeKind::StructDecl) ||
             prevJob->originalNode->is(AstNodeKind::EnumDecl))
        prevNode = prevJob->originalNode;
    else
        prevNode = prevNodeLocal;

    const AstNode* depNode = nullptr;
    if (depJob)
        depNode = depJob->originalNode;

    SWAG_ASSERT(prevNode);

    Utf8 msg;
    Utf8 hint;

    if (depNode)
    {
        msg = formNte(Nte0110, Naming::kindName(prevNode).c_str(), prevNode->token.c_str(), Naming::kindName(depNode).c_str(), depNode->token.c_str());
    }
    else if (prevNode && prevJob->waitingType)
    {
        msg  = formNte(Nte0055, Naming::kindName(prevNode).c_str(), prevNode->token.c_str(), prevJob->waitingType->getDisplayNameC());
        hint = Diagnostic::isType(prevNode->typeInfo);
    }
    else if (prevJob->waitingType && dynamic_cast<TypeGenStructJob*>(prevJob))
    {
        msg = formNte(Nte0187, prevJob->waitingType->getDisplayNameC());
    }
    else if (prevJob->waitingType)
    {
        msg = formNte(Nte0187, prevJob->waitingType->getDisplayNameC());
    }
    else
    {
        msg  = formNte(Nte0186, Naming::kindName(prevNode).c_str(), prevNode->token.c_str());
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
                msg += form("because of %s [[%s]]", Naming::kindName(prevJob->waitingHintNode).c_str(), prevJob->waitingHintNode->token.c_str());
                break;
            default:
                break;
        }
    }

    const auto note   = Diagnostic::note(prevNodeLocal, prevNodeLocal->token, msg);
    note->canBeMerged = false;
    note->hint        = hint;

    Utf8 remark, sym;
    if (prevJob->waitingSymbolSolved)
        sym = form("%s [[%s]]", Naming::kindName(prevJob->waitingSymbolSolved->kind).c_str(), prevJob->waitingSymbolSolved->getFullName().c_str());

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
            remark = form("waiting for %s to be fully solved", sym.c_str());
            break;
        case JobWaitKind::WaitSymbol:
            remark = form("waiting for %s to be solved", sym.c_str());
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

        if (done.contains(depJob))
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

void Workspace::errorPendingJobs(const Vector<PendingJob>& pendingJobs)
{
    for (const auto& it : pendingJobs)
    {
        const auto pendingJob = it.pendingJob;
        const auto node       = it.node;

        if (node->token.sourceFile->module->hasCycleError)
            continue;

        // Is there a dependency cycle ?
        Set<Job*>          done;
        VectorNative<Job*> cycle;
        const bool         isCycle = errorPendingCycle(pendingJob, pendingJob->waitingJobs, done, cycle);
        if (isCycle)
        {
            Vector<const Diagnostic*> notes;
            auto                      prevJob = pendingJob;

            for (const auto depJob : cycle)
            {
                if (prevJob->nodes.size() > 1 && prevJob->originalNode->is(AstNodeKind::FuncDecl))
                {
                    const auto front  = prevJob->nodes.front();
                    const auto back   = prevJob->nodes.back();
                    auto       msg    = formNte(Nte0110, Naming::kindName(front).c_str(), front->token.c_str(), Naming::kindName(back).c_str(), back->token.c_str());
                    const auto note   = Diagnostic::note(back, back->token, msg);
                    note->canBeMerged = false;
                    note->hint        = Diagnostic::isType(back->typeInfo);
                    notes.push_back(note);
                }

                const auto note = errorPendingJob(prevJob, depJob);
                if (note)
                    notes.push_back(note);
                prevJob = depJob;
            }

            const auto note = errorPendingJob(prevJob, pendingJob);
            if (note)
                notes.push_back(note);

            const auto prevNodeLocal = pendingJob->originalNode ? pendingJob->originalNode : pendingJob->nodes.front();
            Diagnostic err{prevNodeLocal, prevNodeLocal->token, formErr(Err0629, Naming::kindName(prevNodeLocal).c_str(), prevNodeLocal->token.c_str())};
            Report::report(err, notes);
            const auto sourceFile             = Report::getDiagFile(err);
            sourceFile->module->hasCycleError = true;
            continue;
        }

        // Job is not done, and we do not wait for a specific other job...
        if (pendingJob->waitingJobs.empty())
        {
            std::ranges::sort(g_ThreadMgr.waitingJobs, [](const Job* n1, const Job* n2) {
                if (!n1->sourceFile && n2->sourceFile)
                    return true;
                if (!n1->sourceFile || !n2->sourceFile)
                    return false;
                return strcmp(n1->sourceFile->name, n2->sourceFile->name) < 0;
            });

            uint32_t idx = 0;
            for (const auto j : g_ThreadMgr.waitingJobs)
                j->waitingJobIndex = idx++;

            const auto note = errorPendingJob(pendingJob, nullptr);
            if (!note)
                continue;
            Diagnostic err{note->sourceFile, note->startLocation, note->endLocation, toErr(Err0085)};
            err.addNote(note);
            Report::report(err);
        }
    }
}

void Workspace::computeWaitingJobs()
{
    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        for (const auto dep : pendingJob->dependentJobs.list)
        {
            dep->waitingJobs.push_back_once(pendingJob);
        }

        for (const auto dep : pendingJob->jobsToAdd)
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
                for (auto it : g_ThreadMgr.waitingJobs)
                {
                    for (const auto it1 : pendingJob->waitingSymbolSolved->nodes)
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
                for (const auto& waitingJob : g_ThreadMgr.waitingJobs)
                {
                    const auto it = dynamic_cast<TypeGenStructJob*>(waitingJob);
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
                    for (auto it : g_ThreadMgr.waitingJobs)
                    {
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
                    for (auto it : g_ThreadMgr.waitingJobs)
                    {
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
    for (const auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        const auto sourceFile = pendingJob->sourceFile;
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
        if (node->is(AstNodeKind::FuncDeclType))
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
    for (const auto f : module->files)
    {
        const auto job  = Allocator::alloc<SyntaxJob>();
        job->sourceFile = f;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    if (module->numErrors)
    {
        Report::error(module->is(ModuleKind::BootStrap) ? toErr(Fat0014) : toErr(Fat0015));
        return false;
    }

    const auto job = Allocator::alloc<ModuleSemanticJob>();
    job->module    = module;
    g_ThreadMgr.addJob(job);
    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();

    bool done;
    ByteCodeOptimizer::optimize(nullptr, module, done);
    g_ThreadMgr.waitEndJobs();

    // Errors !!!
    if (module->numErrors)
    {
        Report::error(module->is(ModuleKind::BootStrap) ? toErr(Fat0014) : toErr(Fat0015));
        return false;
    }

    return module->filterFunctionsToEmit();
}

bool Workspace::buildTarget()
{
    // Load as much files as we can during setup stage
    // to void wasting some cores
    //////////////////////////////////////////////////

    if (g_ThreadMgr.numWorkers != 1 && !g_CommandLine.scriptCommand)
    {
        const auto enumJob0    = Allocator::alloc<EnumerateModuleJob>();
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

    const auto enumJob1    = Allocator::alloc<EnumerateModuleJob>();
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
            Report::error(formErr(Fat0029, g_CommandLine.moduleName.c_str()));
            return false;
        }

        toBuild.clear();
        toBuild.push_back(filteredModule);
        filteredModule->addedToBuild = true;
        for (uint32_t i = 0; i < toBuild.size(); i++)
        {
            for (const auto& dep : toBuild[i]->moduleDependencies)
            {
                ScopedLock lk(mutexModules);
                auto       it = g_Workspace->mapModulesNames.find(dep->name);
                if (it == g_Workspace->mapModulesNames.end())
                {
                    Report::error(formErr(Fat0011, dep->name.c_str()));
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
        for (const auto module : toBuild)
            module->addedToBuild = true;
        for (const auto module : toBuild)
        {
            if (module == bootstrapModule || module == runtimeModule)
                continue;
            if (module->isErrorModule)
                continue;
            const auto job = Allocator::alloc<ModuleBuildJob>();
            job->module    = module;
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
        for (uint32_t i = 0; i < waitingJobs.size(); i++)
        {
            const auto job = waitingJobs[i];
            if (job->hasFlag(JOB_PENDING_PLACE_HOLDER))
            {
                SWAG_ASSERT(!job->hasFlag(JOB_IS_IN_THREAD));
                job->addFlag(JOB_ACCEPT_PENDING_COUNT);
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
            const milliseconds ms  = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            g_CommandLine.randSeed = static_cast<int>(ms.count()) & 0x7FFFFFFF;
            srand(g_CommandLine.randSeed);
            g_CommandLine.randSeed = rand() & 0x7FFFFFFF;
        }

        srand(g_CommandLine.randSeed);
        g_Log.setColor(LogColor::DarkBlue);
        g_Log.print(form("[devmode] randomize seed is %d\n", g_CommandLine.randSeed));
        g_Log.setDefaultColor();
    }
#endif

    // User arguments that can be retrieved with '@args'
    SwagSlice oneArg;
    g_CommandLine.exePathStr = g_CommandLine.exePath;
    oneArg.buffer            = g_CommandLine.exePathStr.buffer;
    oneArg.count             = g_CommandLine.exePathStr.length();
    g_CommandLine.userArgumentsStr.push_back(oneArg);

    Utf8::tokenizeBlanks(g_CommandLine.userArguments, g_CommandLine.userArgumentsVec);
    for (auto& arg : g_CommandLine.userArgumentsVec)
    {
        oneArg.buffer = arg.buffer;
        oneArg.count  = arg.length();
        g_CommandLine.userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine.userArgumentsSlice.buffer = g_CommandLine.userArgumentsStr.data();
    g_CommandLine.userArgumentsSlice.count  = g_CommandLine.userArgumentsStr.size();

    // Build !
    auto result = true;

    {
        Timer timer(&g_Workspace->totalTime, true);

        setup();

        if (!g_CommandLine.scriptCommand)
        {
            if (g_CommandLine.verbosePath)
                g_Log.messageVerbose(form("workspace path is [[%s]]", workspacePath.c_str()));
            if (g_CommandLine.listDepCmd || g_CommandLine.getDepCmd)
                g_Log.messageHeaderCentered("Workspace", workspacePath.filename());
            else
            {
                const auto targetFullName = g_Workspace->getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
                g_Log.messageHeaderCentered("Workspace", form("%s [%s]", workspacePath.filename().c_str(), targetFullName.c_str()));
            }
        }
        else
        {
            const auto targetFullName = g_Workspace->getTargetFullName(g_CommandLine.buildCfg, g_CommandLine.target);
            const Path p              = g_CommandLine.fileName;
            g_Log.messageHeaderCentered("Script", form("%s [%s]", p.filename().c_str(), targetFullName.c_str()));
        }

        addBootstrap();
        addRuntime();
        if (!g_CommandLine.scriptCommand)
            setupTarget();
        result = buildTarget();
    }

    // Results
    if (!g_CommandLine.scriptCommand || !g_CommandLine.scriptRun)
    {
        if (g_Workspace->skippedModules.load() > 0)
            g_Log.messageHeaderCentered("Skipped modules", form("%d", g_Workspace->skippedModules.load()));

        if (g_Workspace->numErrors.load() == 1)
            g_Log.messageHeaderCentered("Done", form("%d error", g_Workspace->numErrors.load()), LogColor::Green, LogColor::Red);
        else if (g_Workspace->numErrors.load())
            g_Log.messageHeaderCentered("Done", form("%d errors", g_Workspace->numErrors.load()), LogColor::Green, LogColor::Red);
        else if (g_Workspace->numWarnings.load() == 1)
            g_Log.messageHeaderCentered("Done", form("%.3fs (%d warning)", OS::timerToSeconds(g_Workspace->totalTime.load()), g_Workspace->numWarnings.load()), LogColor::Green, LogColor::Magenta);
        else if (g_Workspace->numWarnings.load())
            g_Log.messageHeaderCentered("Done", form("%.3fs (%d warnings)", OS::timerToSeconds(g_Workspace->totalTime.load()), g_Workspace->numWarnings.load()), LogColor::Green, LogColor::Magenta);
        else
            g_Log.messageHeaderCentered("Done", form("%.3fs", OS::timerToSeconds(g_Workspace->totalTime.load())));
    }

    return result;
}
