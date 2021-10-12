#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "EnumerateModuleJob.h"
#include "ModuleBuildJob.h"
#include "Module.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Backend.h"
#include "ByteCodeOptimizer.h"
#include "ModuleCfgManager.h"
#include "ErrorIds.h"
#include "ModuleManager.h"

void Workspace::computeModuleName(const fs::path& path, Utf8& moduleName, Utf8& moduleFolder, ModuleKind& kind)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Be sure module name is valid
    Utf8 errorStr;
    if (!Module::isValidName(cFileName, errorStr))
    {
        errorStr = "fatal error: " + errorStr;
        errorStr += Utf8::format(" (path is `%s`)", path.string().c_str());
        g_Log.error(errorStr);
        OS::exit(-1);
    }

    // Module name is equivalent to the folder name, except for the tests folder where
    // we prepend SWAG_TESTS_FOLDER
    if (parent == SWAG_TESTS_FOLDER)
    {
        moduleName = SWAG_TESTS_FOLDER;
        moduleName += "_";
    }

    moduleName += cFileName;

    // Kind
    kind = ModuleKind::Module;
    if (parent == SWAG_TESTS_FOLDER)
        kind = ModuleKind::Test;
    else if (parent == SWAG_EXAMPLES_FOLDER)
        kind = ModuleKind::Example;
    else if (parent == SWAG_DEPENDENCIES_FOLDER)
        kind = ModuleKind::Dependency;
    else if (parent == SWAG_MODULES_FOLDER)
        kind = ModuleKind::Module;
    else
        SWAG_ASSERT(false);

    moduleFolder = path.string();
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

Module* Workspace::createOrUseModule(const Utf8& moduleName, const Utf8& modulePath, ModuleKind kind, bool errorModule)
{
    Module* module = g_Allocator.alloc<Module>();

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
        module->buildCfg                    = cfgModule->buildCfg;
        module->buildParameters.foreignLibs = cfgModule->buildParameters.foreignLibs;
        module->moduleDependencies          = cfgModule->moduleDependencies;
    }

    // Is this the module we want to build ?
    if (g_CommandLine->moduleName == moduleName)
    {
        filteredModule = module;
        if (g_CommandLine->run)
            runModule = filteredModule;
    }

    if (g_CommandLine->stats)
        g_Stats.numModules++;

    return module;
}

void Workspace::addBootstrap()
{
    // Bootstrap will be compiled in the workspace scope, in order to be defined once
    // for all modules
    bootstrapModule       = g_Allocator.alloc<Module>();
    bootstrapModule->kind = ModuleKind::BootStrap;
    bootstrapModule->setup("bootstrap", "");
    modules.push_back(bootstrapModule);

    auto     file         = g_Allocator.alloc<SourceFile>();
    fs::path p            = g_CommandLine->exePath;
    file->name            = "swag.bootstrap.swg";
    file->path            = p.parent_path().string() + "/runtime/bootstrap.swg";
    file->module          = bootstrapModule;
    file->isBootstrapFile = true;
    bootstrapModule->addFile(file);
}

void Workspace::addRuntimeFile(const char* fileName)
{
    auto     file       = g_Allocator.alloc<SourceFile>();
    fs::path p          = g_CommandLine->exePath;
    file->name          = fileName;
    file->path          = p.parent_path().string() + "/runtime/" + fileName;
    file->module        = runtimeModule;
    file->isRuntimeFile = true;
    runtimeModule->addFile(file);
}

void Workspace::addRuntime()
{
    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    runtimeModule       = g_Allocator.alloc<Module>();
    runtimeModule->kind = ModuleKind::Runtime;
    runtimeModule->setup("runtime", "");
    modules.push_back(runtimeModule);

    addRuntimeFile("runtime.swg");
    addRuntimeFile("runtime_win32.swg");
    addRuntimeFile("runtime_msvc.swg");
    addRuntimeFile("systemallocator.swg");
    addRuntimeFile("scratchallocator.swg");
}

OneTag* Workspace::hasTag(const Utf8& name)
{
    for (auto& tag : tags)
    {
        if (tag.name == name)
            return &tag;
    }

    return nullptr;
}

void Workspace::setupInternalTags()
{
    OneTag oneTag;

    // Swag.Endian = "little" or "big" depending on the architecture
    switch (g_CommandLine->target.arch)
    {
    case TargetArch::X86_64:
        oneTag.type       = g_TypeMgr->typeInfoString;
        oneTag.value.text = "little";
        oneTag.name       = "Swag.Endian";
        tags.push_back(oneTag);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

Utf8 Workspace::getTargetFullName()
{
    return g_CommandLine->buildCfg + "-" + Backend::GetOsName(g_CommandLine->target) + "-" + Backend::GetArchName(g_CommandLine->target);
}

void Workspace::setupTarget()
{
    targetPath = workspacePath;

    targetPath.append(SWAG_OUTPUT_FOLDER);
    targetPath.append("/");
    targetPath.append(getTargetFullName().c_str());

    if (g_CommandLine->verbosePath)
        g_Log.verbose(Utf8::format("target path is `%s`", targetPath.string().c_str()));

    // Be sure folders exists
    error_code errorCode;
    if (!fs::exists(targetPath) && !fs::create_directories(targetPath, errorCode))
    {
        g_Log.errorOS(Utf8::format(g_E[Err0545], targetPath.string().c_str()));
        OS::exit(-1);
    }

    // Cache directory
    setupCachePath();
    if (!fs::exists(cachePath))
    {
        g_Log.errorOS(Utf8::format(g_E[Err0546], cachePath.string().c_str()));
        OS::exit(-1);
    }

    cachePath.append(SWAG_CACHE_FOLDER);
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.errorOS(Utf8::format(g_E[Err0547], cachePath.string().c_str()));
        OS::exit(-1);
    }

    cachePath.append(workspacePath.filename().string() + "-" + g_Workspace->getTargetFullName().c_str());
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.errorOS(Utf8::format(g_E[Err0547], cachePath.string().c_str()));
        OS::exit(-1);
    }

    if (g_CommandLine->verbosePath)
        g_Log.verbose(Utf8::format("cache path is `%s`", cachePath.string().c_str()));

    targetPath += "/";
    cachePath += "/";
}

void Workspace::errorPendingJobs(vector<PendingJob>& pendingJobs)
{
    for (auto& it : pendingJobs)
    {
        auto pendingJob = it.pendingJob;
        if (pendingJob->flags & JOB_NO_PENDING_REPORT)
            continue;

        auto node       = it.node;
        auto sourceFile = pendingJob->sourceFile;

        if (node->kind == AstNodeKind::FuncDeclType)
            node = node->parent;

        // Is there a dependency cycle ?
        bool      isCycle = false;
        auto      depJob  = pendingJob->waitingJob;
        set<Job*> done;
        while (depJob)
        {
            if (depJob == pendingJob)
            {
                isCycle = true;
                break;
            }

            if (done.find(depJob) != done.end())
                break;

            done.insert(depJob);
            depJob = depJob->waitingJob;
        }

        if (isCycle)
        {
            vector<const Diagnostic*> notes;
            depJob = pendingJob->waitingJob;

            auto prevJob = pendingJob;
            while (depJob != pendingJob)
            {
                Utf8 msg = Utf8::format(g_E[Nte0046], AstNode::getKindName(prevJob->originalNode).c_str(), prevJob->originalNode->token.text.c_str());
                msg += Utf8::format(g_E[Nte0045], AstNode::getKindName(depJob->originalNode).c_str(), depJob->originalNode->token.text.c_str());
                auto note = new Diagnostic{prevJob->nodes.back(), msg, DiagnosticLevel::Note};
                notes.push_back(note);

                prevJob = depJob;
                depJob->flags |= JOB_NO_PENDING_REPORT;
                depJob = depJob->waitingJob;
            }

            Utf8 msg = Utf8::format(g_E[Nte0046], AstNode::getKindName(prevJob->originalNode).c_str(), prevJob->originalNode->token.text.c_str());
            msg += Utf8::format(g_E[Nte0045], AstNode::getKindName(pendingJob->originalNode).c_str(), pendingJob->originalNode->token.text.c_str());
            auto note = new Diagnostic{prevJob->nodes.back(), msg, DiagnosticLevel::Note};
            notes.push_back(note);

            Diagnostic diag{pendingJob->originalNode, Utf8::format(g_E[Err0419], AstNode::getKindName(pendingJob->originalNode).c_str(), pendingJob->originalNode->token.text.c_str())};
            sourceFile->report(diag, notes);
            continue;
        }

        // If this job is waiting for another job, then we will raise an error for the waiting job only
        if (pendingJob->waitingJob)
        {
            pendingJob->flags |= JOB_NO_PENDING_REPORT;
            continue;
        }

        // Job is not done, and we do not wait for a specific identifier
        auto toSolve = pendingJob->waitingSymbolSolved;
        if (!toSolve)
        {
            Diagnostic diag{node, Utf8::format(g_E[Err0549], pendingJob->module->name.c_str(), AstNode::getKindName(node).c_str(), node->token.text.c_str())};
#ifdef SWAG_DEV_MODE
            diag.remarks.push_back(it.id);
#endif
            sourceFile->report(diag);
            continue;
        }

        // We have an identifier
        SWAG_ASSERT(!toSolve->nodes.empty());
        auto declNode = toSolve->nodes.front();

        Utf8 msg;
        if (toSolve->kind == SymbolKind::PlaceHolder)
            msg = Utf8::format(g_E[Err0892], toSolve->name.c_str());
        else
            msg = Utf8::format(g_E[Err0893], toSolve->name.c_str());

        // a := func(a) for example
        if (toSolve->kind == SymbolKind::Variable &&
            declNode->kind == AstNodeKind::VarDecl &&
            declNode->sourceFile == node->sourceFile &&
            declNode->token.startLocation.line == node->token.startLocation.line)
        {
            msg = Utf8::format(g_E[Err0894], toSolve->name.c_str());
        }

        Diagnostic diag{node, msg};
#ifdef SWAG_DEV_MODE
        diag.remarks.push_back(it.id);
#endif

        Diagnostic note{declNode, g_E[Nte0028], DiagnosticLevel::Note};
        sourceFile->report(diag, &note);
    }
}

void Workspace::computeWaitingJobs()
{
    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        for (auto p : g_ThreadMgr.waitingJobs)
        {
            if (p == pendingJob)
                continue;

            if (p->dependentJobs.list.contains(pendingJob))
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (p->jobsToAdd.contains(pendingJob))
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (p->dependentJob == pendingJob)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (!p->originalNode)
                continue;

            if (pendingJob->waitingSymbolSolved &&
                p->originalNode->kind == AstNodeKind::FuncDecl &&
                pendingJob->waitingSymbolSolved->kind == SymbolKind::Function &&
                p->originalNode->token.text == pendingJob->waitingSymbolSolved->name)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (pendingJob->waitingSymbolSolved &&
                p->originalNode->kind == AstNodeKind::StructDecl &&
                pendingJob->waitingSymbolSolved->kind == SymbolKind::Struct &&
                p->originalNode->token.text == pendingJob->waitingSymbolSolved->name)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (pendingJob->waitingSymbolSolved &&
                p->originalNode->kind == AstNodeKind::VarDecl &&
                pendingJob->waitingSymbolSolved->kind == SymbolKind::Variable &&
                p->originalNode->token.text == pendingJob->waitingSymbolSolved->name)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (pendingJob->waitingSymbolSolved &&
                p->originalNode->kind == AstNodeKind::ConstDecl &&
                pendingJob->waitingSymbolSolved->kind == SymbolKind::Variable &&
                p->originalNode->token.text == pendingJob->waitingSymbolSolved->name)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (pendingJob->waitingSymbolSolved &&
                p->originalNode->kind == AstNodeKind::EnumDecl &&
                pendingJob->waitingSymbolSolved->kind == SymbolKind::Enum &&
                p->originalNode->token.text == pendingJob->waitingSymbolSolved->name)
            {
                pendingJob->waitingJob = p;
                break;
            }

            if (p->originalNode == pendingJob->waitingIdNode)
            {
                pendingJob->waitingJob = p;
                break;
            }
        }
    }
}

void Workspace::checkPendingJobs()
{
    if (g_ThreadMgr.waitingJobs.empty())
        return;

    computeWaitingJobs();

    // Collect unsolved jobs
    set<SymbolName*>   doneSymbols;
    vector<PendingJob> pendingJobs;
    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        auto sourceFile = pendingJob->sourceFile;
        auto firstNode  = pendingJob->originalNode;
        if (!firstNode)
            continue;

        // Get the node the job was trying to resolve
        AstNode* node = nullptr;
        if (!pendingJob->nodes.empty())
            node = pendingJob->nodes.back();
        if (!node)
            continue;

        // Do not generate errors if we already have some errors
        if (sourceFile->module->numErrors)
            continue;

        // No need to raise multiple times an error for the same symbol
        if (pendingJob->waitingSymbolSolved)
        {
            if (doneSymbols.find(pendingJob->waitingSymbolSolved) != doneSymbols.end())
                continue;
            doneSymbols.insert(pendingJob->waitingSymbolSolved);
        }

        // No need to raise multiple times an error for the same id
        Utf8 id = Utf8::format("[kind: %d]", pendingJob->waitingKind);
        if (pendingJob->waitingIdNode)
        {
            id += " ";
            if (pendingJob->waitingIdNode->typeInfo)
                id += pendingJob->waitingIdNode->typeInfo->getDisplayName();
            else
                id += pendingJob->waitingIdNode->token.text;
        }

        if (pendingJob->waitingIdType)
        {
            id += " ";
            id += pendingJob->waitingIdType->getDisplayName();
        }

        PendingJob pj;
        pj.pendingJob = pendingJob;
        pj.node       = node;
        pj.id         = id;
        pendingJobs.push_back(pj);
    }

    errorPendingJobs(pendingJobs);
}

bool Workspace::buildRTModule(Module* module)
{
    for (auto f : module->files)
    {
        auto job        = g_Allocator.alloc<SyntaxJob>();
        job->sourceFile = f;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    if (module->numErrors)
    {
        g_Log.error(module->kind == ModuleKind::BootStrap ? g_E[Err0552] : g_E[Err0554]);
        return false;
    }

    auto job    = g_Allocator.alloc<ModuleSemanticJob>();
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
        g_Log.error(module->kind == ModuleKind::BootStrap ? g_E[Err0552] : g_E[Err0554]);
        return false;
    }

    return true;
}

bool Workspace::buildTarget()
{
    // Load as much files as we can during setup stage
    // to void wasting some cores
    //////////////////////////////////////////////////

    if (g_CommandLine->numCores != 1 && !g_CommandLine->scriptCommand)
    {
        auto enumJob0          = g_Allocator.alloc<EnumerateModuleJob>();
        enumJob0->readFileMode = true;
        g_ThreadMgr.addJob(enumJob0);
    }

    // Bootstrap module semantic pass
    //////////////////////////////////////////////////
    {
        Timer timer(&g_Stats.bootstrapTime);
        SWAG_CHECK(buildRTModule(bootstrapModule));
    }

    // Runtime module semantic pass
    //////////////////////////////////////////////////
    {
        Timer timer(&g_Stats.runtimeTime);
        runtimeModule->initFrom(bootstrapModule);
        SWAG_CHECK(buildRTModule(runtimeModule));
    }

    // Wait for optional jobs running to finish, remove
    // the rest
    //////////////////////////////////////////////////
    g_ThreadMgr.waitEndJobs();
    g_ThreadMgr.clearOptionalJobs();

    // Config pass (compute/fetch dependencies...
    //////////////////////////////////////////////////
    g_ModuleCfgMgr = g_Allocator.alloc<ModuleCfgManager>();
    SWAG_CHECK(g_ModuleCfgMgr->execute());

    // Exit now (do not really build) in case of "get", "list" commands
    if (g_CommandLine->listDepCmd || g_CommandLine->getDepCmd)
        return true;

    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    auto enumJob1          = g_Allocator.alloc<EnumerateModuleJob>();
    enumJob1->readFileMode = false;
    g_ThreadMgr.addJob(enumJob1);
    g_ThreadMgr.waitEndJobs();

    // Filter modules to build
    //////////////////////////////////////////////////
    auto toBuild = modules;
    if (!g_CommandLine->moduleName.empty())
    {
        if (!filteredModule)
        {
            g_Log.error(Utf8::format(g_E[Err0556], g_CommandLine->moduleName.c_str()));
            return false;
        }

        toBuild.clear();
        toBuild.push_back(filteredModule);
        filteredModule->addedToBuild = true;
        for (int i = 0; i < toBuild.size(); i++)
        {
            for (auto& dep : toBuild[i]->moduleDependencies)
            {
                ScopedLock lk(mutexModules);
                auto       it = g_Workspace->mapModulesNames.find(dep->name);
                if (it == g_Workspace->mapModulesNames.end())
                {
                    g_Log.error(Utf8::format(g_E[Err0557], dep->name.c_str()));
                    return false;
                }

                auto depModule = it->second;
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
        {
            if (module == bootstrapModule)
                continue;
            if (module->isErrorModule)
                continue;
            auto job    = g_Allocator.alloc<ModuleBuildJob>();
            job->module = module;
            g_ThreadMgr.addJob(job);
        }
    }

    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();
    return true;
}

bool Workspace::build()
{
    g_ModuleMgr = g_Allocator.alloc<ModuleManager>();
    g_LangSpec  = g_Allocator.alloc<LanguageSpec>();
    g_LangSpec->setup();
    Backend::setup();

    // [devmode] stuff
#if defined SWAG_DEBUG
    g_Log.messageHeaderCentered("Compiler", "[debug|devmode]", LogColor::DarkBlue, LogColor::DarkBlue);
#elif defined SWAG_DEV_MODE
    g_Log.messageHeaderCentered("Compiler", "[devmode]", LogColor::DarkBlue, LogColor::DarkBlue);
#endif

    // [devmode] randomize/seed
#ifdef SWAG_DEV_MODE
    if (g_CommandLine->randomize)
    {
        if (!g_CommandLine->randSeed)
        {
            using namespace std::chrono;
            milliseconds ms         = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            g_CommandLine->randSeed = (int) ms.count() & 0x7FFFFFFF;
            srand(g_CommandLine->randSeed);
            g_CommandLine->randSeed = rand() & 0x7FFFFFFF;
        }

        srand(g_CommandLine->randSeed);
        g_Log.setColor(LogColor::DarkBlue);
        g_Log.print(Utf8::format("[devmode] randomize seed is %d\n", g_CommandLine->randSeed));
        g_Log.setDefaultColor();
    }
#endif

    // User arguments that can be retrieved with '@args'
    pair<void*, void*> oneArg;
    g_CommandLine->exePathStr = g_CommandLine->exePath.string();
    oneArg.first              = (void*) g_CommandLine->exePathStr.c_str();
    oneArg.second             = (void*) g_CommandLine->exePathStr.size();
    g_CommandLine->userArgumentsStr.push_back(oneArg);

    Utf8::tokenizeBlanks(g_CommandLine->userArguments.c_str(), g_CommandLine->userArgumentsVec);
    for (auto& arg : g_CommandLine->userArgumentsVec)
    {
        oneArg.first  = (void*) arg.c_str();
        oneArg.second = (void*) (size_t) arg.length();
        g_CommandLine->userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine->userArgumentsSlice.first  = &g_CommandLine->userArgumentsStr[0];
    g_CommandLine->userArgumentsSlice.second = (void*) g_CommandLine->userArgumentsStr.size();

    // Build !
    auto result = true;

    {
        Timer timer(&g_Stats.totalTime, true);

        setup();

        if (!g_CommandLine->scriptCommand)
        {
            if (g_CommandLine->verbosePath)
                g_Log.verbose(Utf8::format("workspace path is `%s`", workspacePath.string().c_str()));
            if (g_CommandLine->listDepCmd || g_CommandLine->getDepCmd)
                g_Log.messageHeaderCentered("Workspace", workspacePath.filename().string().c_str());
            else
                g_Log.messageHeaderCentered("Workspace", Utf8::format("%s [%s]", workspacePath.filename().string().c_str(), g_Workspace->getTargetFullName().c_str()));
        }

        addBootstrap();
        addRuntime();
        if (!g_CommandLine->scriptCommand)
            setupTarget();
        result = buildTarget();
    }

    // Results
    if (!g_CommandLine->scriptCommand)
    {
        if (g_Stats.skippedModules.load() > 0)
            g_Log.messageHeaderCentered("Skipped modules", Utf8::format("%d", g_Stats.skippedModules.load()));
        if (g_Workspace->numErrors)
            g_Log.messageHeaderCentered("Done", Utf8::format("%d error(s)", g_Workspace->numErrors.load()), LogColor::Green, LogColor::Red);
        else
            g_Log.messageHeaderCentered("Done", Utf8::format("%.3fs", OS::timerToSeconds(g_Stats.totalTime.load())));
    }

    return result;
}
