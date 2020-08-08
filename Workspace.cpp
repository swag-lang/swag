#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "EnumerateModuleJob.h"
#include "ModuleBuildJob.h"
#include "Os.h"
#include "Timer.h"
#include "CommandLineParser.h"
#include "Module.h"

Workspace g_Workspace;

Module* Workspace::getModuleByName(const Utf8& moduleName)
{
    shared_lock lk(mutexModules);
    auto        it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const Utf8& moduleName)
{
    Module* module;

    {
        unique_lock lk(mutexModules);

        auto it = mapModulesNames.find(moduleName);
        if (it != mapModulesNames.end())
            return it->second;

        module = g_Allocator.alloc<Module>();
        module->setup(moduleName);
        modules.push_back(module);
        mapModulesNames[moduleName] = module;

        // Is this the module we want to build ?
        if (g_CommandLine.moduleFilter == moduleName)
            filteredModule = module;
    }

    if (g_CommandLine.stats)
        g_Stats.numModules++;

    return module;
}

void Workspace::addBootstrap()
{
    // Get bootstrap.swg file
    void*    ptr;
    uint32_t size;
    if (!OS::getEmbeddedTextFile(OS::ResourceFile::SwagBootstrap, &ptr, &size))
    {
        g_Log.error("internal fatal error: unable to load internal 'bootstrap.swg' file");
        exit(-1);
    }

    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    bootstrapModule = g_Allocator.alloc<Module>();
    if (!bootstrapModule->setup(""))
        exit(-1);
    modules.push_back(bootstrapModule);

    auto     file         = g_Allocator.alloc<SourceFile>();
    auto     job          = g_Pool_syntaxJob.alloc();
    fs::path p            = g_CommandLine.exePath;
    file->path            = p.parent_path().string() + "/bootstrap.swg";
    file->module          = bootstrapModule;
    file->isBootstrapFile = true;
    file->externalBuffer  = (uint8_t*) ptr;
    file->externalSize    = size;
    bootstrapModule->addFile(file);
    bootstrapModule->isBootStrap = true;
    job->sourceFile              = file;
    g_ThreadMgr.addJob(job);
}

void Workspace::setupPaths()
{
    workspacePath = fs::absolute(g_CommandLine.workspacePath);
    testsPath     = workspacePath;
    testsPath.append("tests/");
    modulesPath = workspacePath;
    modulesPath.append("modules/");
    dependenciesPath = workspacePath;
    dependenciesPath.append("dependencies/");
}

void Workspace::createNew()
{
    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error("fatal error: missing workspace folder '--workspace'");
        exit(-1);
    }

    if (fs::exists(workspacePath))
    {
        g_Log.error(format("fatal error: workspace folder '%s' already exists", workspacePath.string().c_str()));
        exit(-1);
    }

    // Create workspace folders
    error_code errorCode;
    if (!fs::create_directories(workspacePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(testsPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", testsPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(modulesPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulesPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(dependenciesPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", dependenciesPath.string().c_str()));
        exit(-1);
    }

    // Create one module folder
    auto modulePath = modulesPath;
    modulePath.append(workspacePath.filename());
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    modulePath.append("src");
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    // Create an empty file
    modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        g_Log.error(format("fatal error: cannot create file '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    const char* oneMain = "#main\n{\n\t@print(\"Hello world!\\n\")\n}";
    file << oneMain;

    g_Log.message(format("workspace '%s' has been created", workspacePath.string().c_str()));
    exit(0);
}

void Workspace::setup()
{
    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error("fatal error: missing workspace folder '--workspace'");
        exit(-1);
    }

    if (!fs::exists(workspacePath))
    {
        g_Log.error(format("fatal error: workspace folder '%s' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::exists(testsPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'tests/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::exists(modulesPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'modules/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::exists(dependenciesPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'dependencies/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    g_ThreadMgr.init();
}

void Workspace::deleteFolderContent(const fs::path& path)
{
    OS::visitFiles(path.string().c_str(), [&](const char* cFileName) {
        auto folder = path.string() + "/";
        folder += cFileName;
        try
        {
            fs::remove_all(folder);
        }
        catch (...)
        {
            g_Log.error(format("fatal error: cannot delete file '%s'", folder.c_str()));
            exit(-1);
        }
    });
}

Utf8 Workspace::GetArchName()
{
    switch (g_CommandLine.target)
    {
    case BackendTarget::Win64:
        return "win64";
    default:
        return "?";
    }
}

void Workspace::setupTarget()
{
    targetPath = workspacePath;
    targetPath.append("output/");
    targetPath.append(g_CommandLine.buildCfg + "-" + GetArchName().c_str());

    if (g_CommandLine.verbose)
        g_Log.verbose(format("-- target directory is '%s'", targetPath.string().c_str()));

    // Clean target
    if (g_CommandLine.clean && g_CommandLine.backendOutput)
    {
        if (fs::exists(targetPath))
            deleteFolderContent(targetPath);
    }

    // Be sure folders exists
    error_code errorCode;
    if (!fs::exists(targetPath) && !fs::create_directories(targetPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create target directory '%s'", targetPath.string().c_str()));
        exit(-1);
    }

    // Cache directory
    cachePath = g_CommandLine.cachePath;
    if (cachePath.empty())
    {
        cachePath = OS::getTemporaryFolder();
        if (cachePath.empty())
            cachePath = targetPath;
    }
    else
    {
        if (!fs::exists(cachePath))
        {
            g_Log.error(format("fatal error: cache directory '%s' does not exist", cachePath.string().c_str()));
            exit(-1);
        }
    }

    cachePath.append("swag_cache");
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot cache target directory '%s'", cachePath.string().c_str()));
        exit(-1);
    }

    cachePath.append(workspacePath.filename().string() + "-" + g_CommandLine.buildCfg + "-" + g_Workspace.GetArchName().c_str());
    if (!fs::exists(cachePath) && !fs::create_directories(cachePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot cache target directory '%s'", cachePath.string().c_str()));
        exit(-1);
    }

    auto testCachePath = cachePath;
    testCachePath.append("test");
    if (!fs::exists(testCachePath) && !fs::create_directories(testCachePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot cache target directory '%s'", testCachePath.string().c_str()));
        exit(-1);
    }

    if (g_CommandLine.verbose)
    {
        g_Log.verbose(format("-- cache directory is '%s'", cachePath.string().c_str()));
    }

    // Clean target
    if (g_CommandLine.clean && g_CommandLine.backendOutput)
    {
        if (fs::exists(cachePath))
            deleteFolderContent(cachePath);
        if (fs::exists(testCachePath))
            deleteFolderContent(testCachePath);
    }

    targetPath += "/";
    cachePath += "/";
}


void Workspace::checkPendingJobs()
{
    if (g_ThreadMgr.waitingJobs.empty())
        return;

    for (auto pendingJob : g_ThreadMgr.waitingJobs)
    {
        auto firstNode = pendingJob->originalNode;
        if (firstNode)
        {
            if (!(firstNode->flags & AST_GENERATED))
            {
                AstNode* node = nullptr;
                if (!pendingJob->nodes.empty())
                    node = pendingJob->nodes.back();
                else if (!pendingJob->dependentNodes.empty())
                    node = pendingJob->dependentNodes.back();
                if (node)
                {
                    if (!pendingJob->sourceFile->numErrors)
                    {
                        auto name = firstNode->name;

                        Utf8 typeName;
                        if (firstNode->typeInfo)
                            typeName = format("(type is %s)", firstNode->typeInfo->name.c_str());

                        if (pendingJob->waitingSymbolSolved && !name.empty())
                            pendingJob->sourceFile->report({node, node->token, format("cannot resolve %s '%s' %s because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), name.c_str(), typeName.c_str(), pendingJob->waitingSymbolSolved->fullName.c_str())});
                        else if (pendingJob->waitingSymbolSolved)
                            pendingJob->sourceFile->report({node, node->token, format("cannot resolve %s because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), pendingJob->waitingSymbolSolved->fullName.c_str())});
                        else
                            pendingJob->sourceFile->report({firstNode, firstNode->token, format("cannot resolve %s '%s'", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str())});
                        pendingJob->sourceFile->numErrors = 0;
                    }
                }
            }
        }
    }
}

bool Workspace::buildTarget()
{
    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    if (g_CommandLine.verbose)
        g_Log.verbose("## syntax pass begin");

    {
        Timer timer(g_Stats.syntaxTime);
        auto  enumJob = new EnumerateModuleJob;
        g_ThreadMgr.addJob(enumJob);
        g_ThreadMgr.waitEndJobs();
    }

    if (g_CommandLine.verbose)
        g_Log.verbose(format(" # syntax pass end in %.3fs (%d files %d modules)", g_Stats.syntaxTime.load(), g_Stats.numFiles.load(), modules.size()));

    // Runtime swag module semantic pass
    //////////////////////////////////////////////////
    if (bootstrapModule)
    {
        // Errors in swag.swg !!!
        if (bootstrapModule->numErrors)
        {
            g_Log.error("some errors have been found in 'swag.bootstrap.swg' ! exiting...");
            return false;
        }

        auto job    = g_Pool_moduleSemanticJob.alloc();
        job->module = bootstrapModule;
        g_ThreadMgr.addJob(job);
        g_ThreadMgr.waitEndJobs();

        // Errors in swag.swg !!!
        if (bootstrapModule->numErrors)
        {
            g_Log.error("some errors have been found in 'swag.bootstrap.swg' ! exiting...");
            return false;
        }
    }

    vector<Module*>  toBuild;
    vector<Module*>* modulesToBuild = &modules;

    // Filter modules to build
    //////////////////////////////////////////////////
    if (!g_CommandLine.moduleFilter.empty())
    {
        if (!filteredModule)
        {
            g_Log.error(format("module '%s' cannot be found in that workspace", g_CommandLine.moduleFilter.c_str()));
            return false;
        }

        modulesToBuild = &toBuild;
        toBuild.push_back(filteredModule);
        filteredModule->addedToBuild = true;
        for (int i = 0; i < toBuild.size(); i++)
        {
            for (auto& dep : toBuild[i]->moduleDependencies)
            {
                auto it = g_Workspace.mapModulesNames.find(dep.first);
                SWAG_ASSERT(it != g_Workspace.mapModulesNames.end());

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
        auto job    = g_Pool_moduleBuildJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();
    return true;
}

bool Workspace::build()
{
    {
        Timer timer(g_Stats.totalTime, true);

        setup();

        if (g_CommandLine.devMode)
            g_Log.messageHeaderCentered("Developer", "Mode", LogColor::Blue, LogColor::Blue);
        if (g_CommandLine.verbose)
            g_Log.verbose(format("-- workspace path is '%s'", workspacePath.string().c_str()));

        g_Log.messageHeaderCentered("Workspace", format("%s [%s-%s]", workspacePath.filename().string().c_str(), g_CommandLine.buildCfg.c_str(), g_Workspace.GetArchName().c_str()));
        addBootstrap();
        setupTarget();
        SWAG_CHECK(buildTarget());
    }

    if (g_Workspace.numErrors)
        g_Log.messageHeaderCentered("Done", format("%d error(s)", g_Workspace.numErrors.load()), LogColor::Green, LogColor::Red);
    else
        g_Log.messageHeaderCentered("Done", format("%.3fs", g_Stats.totalTime.load()));

    return true;
}

bool Workspace::watch()
{
    setup();

    if (g_CommandLine.verbose)
        g_Log.verbose(format("=> watching workspace '%s'", workspacePath.string().c_str()));

    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);
    auto cmdLine = cmdParser.buildString(false);

    OS::watch([&](const string& moduleName) {
        uint32_t errors = 0;
        OS::doProcess(format("swag.exe test %s", cmdLine.c_str()), g_Workspace.workspacePath.string(), g_CommandLine.verbose, errors);
    });

    return true;
}