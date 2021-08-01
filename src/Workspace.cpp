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
#include "ByteCodeOptimizerJob.h"
#include "ModuleCfgManager.h"
#include "ErrorIds.h"

Workspace g_Workspace;

void Workspace::computeModuleName(const fs::path& path, Utf8& moduleName, Utf8& moduleFolder, ModuleKind& kind)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Be sure module name is valid
    Utf8 errorStr;
    if (!Module::isValidName(cFileName, errorStr))
    {
        errorStr = "fatal error: " + errorStr;
        errorStr += Utf8::format(" (path is '%s')", path.string().c_str());
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
        sourceFile = g_Workspace.bootstrapModule->findFile(fileName);
    if (!sourceFile)
        sourceFile = g_Workspace.runtimeModule->findFile(fileName);
    return sourceFile;
}

Module* Workspace::getModuleByName(const Utf8& moduleName)
{
    shared_lock lk(mutexModules);
    auto        it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const Utf8& moduleName, const Utf8& modulePath, ModuleKind kind)
{
    Module* module = nullptr;

    {
        scoped_lock lk(mutexModules);

        auto it = mapModulesNames.find(moduleName);
        if (it != mapModulesNames.end())
        {
            it->second->setup(moduleName, modulePath);
            return it->second;
        }

        module = g_Allocator.alloc<Module>();
        modules.push_back(module);
        mapModulesNames[moduleName] = module;
    }

    module->kind = kind;
    module->setup(moduleName, modulePath);

    // Setup from the config module, if it exists
    auto cfgModule = g_ModuleCfgMgr.getCfgModule(moduleName);
    if (cfgModule)
    {
        module->buildCfg                    = cfgModule->buildCfg;
        module->buildParameters.foreignLibs = cfgModule->buildParameters.foreignLibs;
        module->moduleDependencies          = cfgModule->moduleDependencies;
    }

    // Is this the module we want to build ?
    if (g_CommandLine.moduleName == moduleName)
    {
        filteredModule = module;
        if (g_CommandLine.run)
            runModule = filteredModule;
    }

    if (g_CommandLine.stats)
        g_Stats.numModules++;

    return module;
}

void Workspace::addBootstrap()
{
    // Bootstrap will be compiled in the workspace scope, in order to be defined once
    // for all modules
    bootstrapModule       = g_Allocator.alloc<Module>();
    bootstrapModule->kind = ModuleKind::BootStrap;
    bootstrapModule->setup("", "");
    modules.push_back(bootstrapModule);

    auto     file         = g_Allocator.alloc<SourceFile>();
    fs::path p            = g_CommandLine.exePath;
    file->name            = "swag.bootstrap.swg";
    file->path            = p.parent_path().string() + "/runtime/bootstrap.swg";
    file->module          = bootstrapModule;
    file->isBootstrapFile = true;
    bootstrapModule->addFile(file);
}

void Workspace::addRuntimeFile(const char* fileName)
{
    auto     file       = g_Allocator.alloc<SourceFile>();
    fs::path p          = g_CommandLine.exePath;
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
    runtimeModule->setup("", "");
    modules.push_back(runtimeModule);

    addRuntimeFile("runtime.swg");
    addRuntimeFile("runtime_win32.swg");
    addRuntimeFile("runtime_msvc.swg");
    addRuntimeFile("systemallocator.swg");
    addRuntimeFile("scratchallocator.swg");
}

void Workspace::setupPaths()
{
    workspacePath = fs::absolute(g_CommandLine.workspacePath);

    testsPath = workspacePath;
    testsPath.append(SWAG_TESTS_FOLDER);
    testsPath.append("/");

    examplesPath = workspacePath;
    examplesPath.append(SWAG_EXAMPLES_FOLDER);
    examplesPath.append("/");

    modulesPath = workspacePath;
    modulesPath.append(SWAG_MODULES_FOLDER);
    modulesPath.append("/");

    dependenciesPath = workspacePath;
    dependenciesPath.append(SWAG_DEPENDENCIES_FOLDER);
    dependenciesPath.append("/");
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
    switch (g_CommandLine.arch)
    {
    case BackendArch::X86_64:
        oneTag.type       = g_TypeMgr.typeInfoString;
        oneTag.value.text = "little";
        oneTag.name       = "Swag.Endian";
        tags.push_back(oneTag);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void Workspace::setupUserTags()
{
    // Command line tags
    // Format is --tag:"TagName : type = value"
    for (auto& tag : g_CommandLine.tags)
    {
        OneTag oneTag;
        oneTag.cmdLine = tag;

        Utf8 oneTagName = tag;
        oneTagName.trim();

        vector<Utf8> tokens;
        Utf8::tokenize(oneTagName, '=', tokens);

        if (tokens.size() == 2)
        {
            tokens[0].trim();
            tokens[1].trim();

            // Get the type
            LiteralType  literalType = LiteralType::TT_MAX;
            vector<Utf8> tokens1;
            Utf8::tokenize(tokens[0], ':', tokens1);
            if (tokens1.size() == 2)
            {
                tokens1[0].trim();
                tokens1[1].trim();
                auto it = g_LangSpec.nativeTypes.find(tokens1[1]);
                if (!it)
                {
                    g_Log.error(Utf8::format(Msg0539, tokens1[0].c_str(), tokens1[1].c_str()));
                    OS::exit(-1);
                }

                literalType = *it;
            }

            // Get value
            // Use the tokenizer
            auto tokenVal = tokens[1];
            tokenVal.trim();

            oneTag.type = nullptr;
            if (literalType != LiteralType::TT_MAX)
                oneTag.type = TypeManager::literalTypeToType(literalType);

            // If type is already specified as string, just take the value part without any conversion
            if (oneTag.type && oneTag.type->isNative(NativeTypeKind::String))
            {
                oneTag.value.text = tokenVal;
            }
            else
            {
                SourceFile fakeFile;
                Tokenizer  tokenizer;
                Token      token;
                fakeFile.setExternalBuffer(tokenVal.buffer, (uint32_t) tokenVal.count);
                tokenizer.setFile(&fakeFile);

                bool neg = false;
                tokenizer.getToken(token);
                if (token.id == TokenId::SymMinus)
                {
                    neg = true;
                    tokenizer.getToken(token);
                }
                else if (token.id == TokenId::SymPlus)
                {
                    tokenizer.getToken(token);
                }

                if (token.id != TokenId::LiteralNumber && token.id != TokenId::LiteralString)
                {
                    g_Log.error(Utf8::format(Msg0538, tokenVal.c_str(), tokens1[0].c_str()));
                    OS::exit(-1);
                }

                // Check type and value
                if (literalType == LiteralType::TT_MAX)
                    literalType = token.literalType;
                oneTag.value.reg  = token.literalValue;
                oneTag.value.text = token.text;

                auto errMsg = SemanticJob::checkLiteralType(oneTag.value, token, oneTag.type, neg);
                if (!errMsg.empty())
                {
                    auto err = Utf8::format(Msg0322, tokens1[0].c_str(), errMsg.c_str());
                    g_Log.error(err);
                    OS::exit(-1);
                }
            }

            tokens1[0].trim();
            oneTag.name = tokens1[0];
            tags.push_back(oneTag);
        }
        else
        {
            oneTag.type        = g_TypeMgr.typeInfoBool;
            oneTag.value.reg.b = true;
            oneTag.name        = oneTagName;
            tags.push_back(oneTag);
        }
    }
}

void Workspace::setup()
{
    setupPaths();
    setupInternalTags();
    setupUserTags();

    if (workspacePath.empty())
    {
        g_Log.error(Msg0540);
        OS::exit(-1);
    }

    bool invalid = false;
    if (!fs::exists(workspacePath))
    {
        g_Log.error(Utf8::format(Msg0541, workspacePath.string().c_str()));
        invalid = true;
    }
    else if (!g_CommandLine.scriptCommand && !fs::exists(modulesPath) && !fs::exists(testsPath))
    {
        g_Log.error(Utf8::format(Msg0542, workspacePath.string().c_str()));
        invalid = true;
    }

    if (invalid)
    {
        g_Log.message(Note012);
        OS::exit(-1);
    }

    g_ThreadMgr.init();
}

void Workspace::setupCachePath()
{
    // Cache directory
    cachePath = g_CommandLine.cachePath;
    if (cachePath.empty())
    {
        cachePath = OS::getTemporaryFolder();
        if (cachePath.empty())
        {
            cachePath = workspacePath;
            cachePath += SWAG_OUTPUT_FOLDER;
            cachePath += "/";
        }
    }
}

Utf8 Workspace::getTargetFolder()
{
    return g_CommandLine.buildCfg + "-" + Backend::GetOsName() + "-" + Backend::GetArchName();
}

Utf8 Workspace::getPublicPath(Module* module, bool forWrite)
{
    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
        return "";

    Utf8 publicPath = module->path + "/";
    publicPath += SWAG_PUBLIC_FOLDER;
    publicPath += "/";
    publicPath = Utf8::normalizePath(fs::path(publicPath.c_str()));

    if (!fs::exists(publicPath.c_str()))
    {
        if (!forWrite)
            return "";
        error_code errorCode;
        if (!fs::create_directories(publicPath.c_str(), errorCode))
        {
            module->error(Utf8::format(Msg0543, publicPath.c_str()));
            return "";
        }
    }

    auto cfgPublicPath = publicPath + getTargetFolder().c_str();
    cfgPublicPath += "/";

    if (!fs::exists(cfgPublicPath.c_str()))
    {
        if (!forWrite)
            return "";
        error_code errorCode;
        if (!fs::create_directories(cfgPublicPath.c_str(), errorCode))
        {
            module->error(Utf8::format(Msg0543, cfgPublicPath.c_str()));
            return "";
        }
    }

    return cfgPublicPath;
}

void Workspace::setupTarget()
{
    targetPath = workspacePath;

    targetPath.append(SWAG_OUTPUT_FOLDER);
    targetPath.append("/");
    targetPath.append(getTargetFolder().c_str());

    if (g_CommandLine.verbosePath)
        g_Log.verbose(Utf8::format("target path is '%s'", targetPath.string().c_str()));

    // Be sure folders exists
    error_code errorCode;
    if (!fs::exists(targetPath) && !fs::create_directories(targetPath, errorCode))
    {
        g_Log.error(Utf8::format(Msg0545, targetPath.string().c_str()));
        OS::exit(-1);
    }

    // Cache directory
    setupCachePath();
    if (!fs::exists(cachePath))
    {
        g_Log.error(Utf8::format(Msg0546, cachePath.string().c_str()));
        OS::exit(-1);
    }

    cachePath.append(SWAG_CACHE_FOLDER);
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.error(Utf8::format(Msg0547, cachePath.string().c_str()));
        OS::exit(-1);
    }

    cachePath.append(workspacePath.filename().string() + "-" + g_Workspace.getTargetFolder().c_str());
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.error(Utf8::format(Msg0547, cachePath.string().c_str()));
        OS::exit(-1);
    }

    if (g_CommandLine.verbosePath)
        g_Log.verbose(Utf8::format("cache path is '%s'", cachePath.string().c_str()));

    targetPath += "/";
    cachePath += "/";
}

void Workspace::errorPendingJobs(vector<PendingJob>& pendingJobs)
{
    vector<PendingJob> newPending;

    // Filters
    for (auto& it : pendingJobs)
    {
        if (it.id == "WAIT_SYMBOL")
            newPending.push_back(it);
    }

    if (!newPending.empty())
        pendingJobs = newPending;

    // Raise errors
    for (auto& it : pendingJobs)
    {
        auto  pendingJob = it.pendingJob;
        auto  node       = it.node;
        auto& id         = it.id;
        auto  sourceFile = pendingJob->sourceFile;

        if (node->kind == AstNodeKind::FuncDeclType)
            node = node->parent;
        Utf8 name = node->token.text;

        // Job is not done, and we do not wait for a specific identifier
        auto toSolve = pendingJob->waitingSymbolSolved;
        if (!toSolve && !node->token.text.empty())
        {
            Diagnostic diag{node, node->token, Utf8::format(Msg0549, pendingJob->module->name.c_str(), AstNode::getKindName(node).c_str(), name.c_str())};
            diag.remarks.push_back(id);
            sourceFile->report(diag);
            continue;
        }

        if (!toSolve)
        {
            Diagnostic diag{node, node->token, Utf8::format(Msg0550, pendingJob->module->name.c_str(), AstNode::getKindName(node).c_str())};
            diag.remarks.push_back(id);
            sourceFile->report(diag);
            continue;
        }

        // We have an identifier
        SWAG_ASSERT(!toSolve->nodes.empty());
        auto declNode = toSolve->nodes.front();

        Utf8 msg;
        if (toSolve->kind == SymbolKind::PlaceHolder)
            msg = Utf8::format(Msg0892, toSolve->name.c_str());
        else
            msg = Utf8::format(Msg0893, toSolve->name.c_str());

        // a := func(a) for example
        if (toSolve->kind == SymbolKind::Variable &&
            declNode->kind == AstNodeKind::VarDecl &&
            declNode->sourceFile == node->sourceFile &&
            declNode->token.startLocation.line == node->token.startLocation.line)
        {
            msg = Utf8::format(Msg0894, toSolve->name.c_str());
        }

        Diagnostic diag{node, node->token, msg};
#ifdef SWAG_DEV_MODE
        diag.remarks.push_back(id);
#endif

        Diagnostic note{declNode, declNode->token, Msg0551, DiagnosticLevel::Note};
        sourceFile->report(diag, &note);
    }
}

void Workspace::checkPendingJobs()
{
    if (g_ThreadMgr.waitingJobs.empty())
        return;

    set<SymbolName*> doneSymbols;
    set<Utf8>        doneIds;

    // Collect unsolved jobs
    vector<PendingJob> pendingJobsWithSymbol;
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
        else if (!pendingJob->dependentNodes.empty())
            node = pendingJob->dependentNodes.back();
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
        Utf8 id = pendingJob->waitingId;
        if (pendingJob->waitingIdNode)
        {
            id += " ";
            if (pendingJob->waitingIdNode->typeInfo)
                id += pendingJob->waitingIdNode->typeInfo->name;
            else
                id += pendingJob->waitingIdNode->token.text;
        }

        if (pendingJob->waitingIdType)
        {
            id += " ";
            id += pendingJob->waitingIdType->name;
        }

        if (pendingJob->waitingIdNode || pendingJob->waitingIdType)
        {
            Utf8 doneId = Utf8::format("%s%llx%llx", (size_t) pendingJob->waitingIdNode, (size_t) pendingJob->waitingIdType);
            if (doneIds.find(doneId) != doneIds.end())
                continue;
            doneIds.insert(doneId);
        }

        PendingJob pj;
        pj.pendingJob = pendingJob;
        pj.node       = node;
        pj.id         = id;
        if (pendingJob->waitingSymbolSolved)
            pendingJobsWithSymbol.push_back(pj);
        else
            pendingJobs.push_back(pj);
    }

    if (!pendingJobsWithSymbol.empty())
        errorPendingJobs(pendingJobsWithSymbol);
    else
        errorPendingJobs(pendingJobs);
}

bool Workspace::buildTarget()
{
    // Bootstrap module semantic pass
    //////////////////////////////////////////////////
    {
        Timer timer(&g_Stats.bootstrapTime);
        timer.start();

        for (auto f : bootstrapModule->files)
        {
            auto job        = g_Allocator.alloc<SyntaxJob>();
            job->sourceFile = f;
            g_ThreadMgr.addJob(job);
        }

        g_ThreadMgr.waitEndJobs();

        if (bootstrapModule->numErrors)
        {
            g_Log.error(Msg0552);
            return false;
        }

        auto job    = g_Allocator.alloc<ModuleSemanticJob>();
        job->module = bootstrapModule;
        g_ThreadMgr.addJob(job);
        g_ThreadMgr.waitEndJobs();
        checkPendingJobs();

        ByteCodeOptimizerJob opt;
        opt.module     = bootstrapModule;
        opt.startIndex = 0;
        opt.endIndex   = (int) bootstrapModule->byteCodeFunc.size();
        opt.optimize(false);

        // Errors !!!
        if (bootstrapModule->numErrors)
        {
            g_Log.error(Msg0552);
            return false;
        }

        timer.stop();
    }

    // Runtime module semantic pass
    //////////////////////////////////////////////////
    {
        Timer timer(&g_Stats.runtimeTime);
        timer.start();

        for (auto f : runtimeModule->files)
        {
            auto job        = g_Allocator.alloc<SyntaxJob>();
            job->sourceFile = f;
            g_ThreadMgr.addJob(job);
        }

        g_ThreadMgr.waitEndJobs();

        if (runtimeModule->numErrors)
        {
            g_Log.error(Msg0554);
            return false;
        }

        runtimeModule->constantSegment.initFrom(&bootstrapModule->constantSegment);
        runtimeModule->mutableSegment.initFrom(&bootstrapModule->mutableSegment);

        auto job    = g_Allocator.alloc<ModuleSemanticJob>();
        job->module = runtimeModule;
        g_ThreadMgr.addJob(job);
        g_ThreadMgr.waitEndJobs();
        checkPendingJobs();

        ByteCodeOptimizerJob opt;
        opt.module     = runtimeModule;
        opt.startIndex = 0;
        opt.endIndex   = (int) runtimeModule->byteCodeFunc.size();
        opt.optimize(false);

        // Errors !!!
        if (runtimeModule->numErrors)
        {
            g_Log.error(Msg0554);
            return false;
        }

        timer.stop();
    }

    // Config pass (compute/fetch dependencies...
    //////////////////////////////////////////////////
    g_ThreadMgr.waitEndJobs();
    SWAG_CHECK(g_ModuleCfgMgr.execute());

    // Exit now (do not really build) in case of "get", "list" commands
    if (g_CommandLine.listDepCmd || g_CommandLine.getDepCmd)
        return true;

    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    auto enumJob = g_Allocator.alloc<EnumerateModuleJob>();
    g_ThreadMgr.addJob(enumJob);
    g_ThreadMgr.waitEndJobs();

    // Filter modules to build
    //////////////////////////////////////////////////
    VectorNative<Module*>  toBuild;
    VectorNative<Module*>* modulesToBuild = &modules;
    if (!g_CommandLine.moduleName.empty())
    {
        if (!filteredModule)
        {
            g_Log.error(Utf8::format(Msg0556, g_CommandLine.moduleName.c_str()));
            return false;
        }

        modulesToBuild = &toBuild;
        toBuild.push_back(filteredModule);
        filteredModule->addedToBuild = true;
        for (int i = 0; i < toBuild.size(); i++)
        {
            for (auto& dep : toBuild[i]->moduleDependencies)
            {
                auto it = g_Workspace.mapModulesNames.find(dep->name);
                if (it == g_Workspace.mapModulesNames.end())
                {
                    g_Log.error(Utf8::format(Msg0557, dep->name.c_str()));
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
    for (auto module : *modulesToBuild)
    {
        if (module == bootstrapModule)
            continue;
        auto job    = g_Allocator.alloc<ModuleBuildJob>();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();
    return true;
}

bool Workspace::build()
{
    g_TypeMgr.setup();
    g_LangSpec.setup();
    Backend::setup();

    // [devmode] stuff
#if defined SWAG_DEBUG
    g_Log.messageHeaderCentered("Compiler", "[debug|devmode]", LogColor::DarkBlue, LogColor::DarkBlue);
#elif defined SWAG_DEV_MODE
    g_Log.messageHeaderCentered("Compiler", "[devmode]", LogColor::DarkBlue, LogColor::DarkBlue);
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
        g_Log.print(Utf8::format("[devmode] randomize seed is %d\n", g_CommandLine.randSeed));
        g_Log.setDefaultColor();
    }
#endif

    // User arguments that can be retrieved with '@args'
    pair<void*, void*> oneArg;
    g_CommandLine.exePathStr = g_CommandLine.exePath.string();
    oneArg.first             = (void*) g_CommandLine.exePathStr.c_str();
    oneArg.second            = (void*) g_CommandLine.exePathStr.size();
    g_CommandLine.userArgumentsStr.push_back(oneArg);

    Utf8::tokenizeBlanks(g_CommandLine.userArguments.c_str(), g_CommandLine.userArgumentsVec);
    for (auto& arg : g_CommandLine.userArgumentsVec)
    {
        oneArg.first  = (void*) arg.c_str();
        oneArg.second = (void*) (size_t) arg.length();
        g_CommandLine.userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine.userArgumentsSlice.first  = &g_CommandLine.userArgumentsStr[0];
    g_CommandLine.userArgumentsSlice.second = (void*) g_CommandLine.userArgumentsStr.size();

    // Build !
    auto result = true;

    {
        Timer timer(&g_Stats.totalTime);
        timer.start(true);

        setup();

        if (!g_CommandLine.scriptCommand)
        {
            if (g_CommandLine.verbosePath)
                g_Log.verbose(Utf8::format("workspace path is '%s'", workspacePath.string().c_str()));
            if (g_CommandLine.listDepCmd || g_CommandLine.getDepCmd)
                g_Log.messageHeaderCentered("Workspace", workspacePath.filename().string().c_str());
            else
                g_Log.messageHeaderCentered("Workspace", Utf8::format("%s [%s]", workspacePath.filename().string().c_str(), g_Workspace.getTargetFolder().c_str()));
        }

        addBootstrap();
        addRuntime();
        setupTarget();
        result = buildTarget();

        timer.stop(true);
    }

    // Results
    if (!g_CommandLine.scriptCommand)
    {
        if (g_Stats.skippedModules.load() > 0)
            g_Log.messageHeaderCentered("Skipped modules", Utf8::format("%d", g_Stats.skippedModules.load()));
        if (g_Workspace.numErrors)
            g_Log.messageHeaderCentered("Done", Utf8::format("%d error(s)", g_Workspace.numErrors.load()), LogColor::Green, LogColor::Red);
        else
            g_Log.messageHeaderCentered("Done", Utf8::format("%.3fs", OS::timerToSeconds(g_Stats.totalTime.load())));
    }

    return result;
}
