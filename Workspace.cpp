#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Stats.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "CopyFileJob.h"
#include "ByteCode.h"
#include "ByteCodeModuleManager.h"
#include "Os.h"

Workspace g_Workspace;

Module* Workspace::getModuleByName(const string& moduleName)
{
    scoped_lock lk(mutexModules);
    auto        it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const string& moduleName)
{
    Module* module;

    {
        scoped_lock lk(mutexModules);

        auto it = mapModulesNames.find(moduleName);
        if (it != mapModulesNames.end())
            return it->second;

        module = g_Pool_module.alloc();
        module->setup(moduleName);
        modules.push_back(module);
        mapModulesNames[moduleName] = module;
    }

    if (g_CommandLine.stats)
        g_Stats.numModules++;

    return module;
}

void Workspace::enumerateFilesInModule(const fs::path& path, Module* module)
{
    // Scan source folder
    vector<string> directories;
    directories.push_back(path.string());

    string   tmp, tmp1;
    fs::path modulePath;
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

        OS::visitFilesFolders(tmp.c_str(), [&](const char* cFileName, bool isFolder) {
            if (isFolder)
            {
                tmp1 = tmp + "/" + cFileName;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    if (g_CommandLine.fileFilter.empty() || strstr(cFileName, g_CommandLine.fileFilter.c_str()))
                    {
                        // File filtering by name
                        if (!module->fromTests || g_CommandLine.testFilter.empty() || strstr(cFileName, g_CommandLine.testFilter.c_str()))
                        {
                            auto job        = g_Pool_syntaxJob.alloc();
                            auto file       = g_Pool_sourceFile.alloc();
                            job->sourceFile = file;
                            file->fromTests = module->fromTests;
                            file->path      = tmp + "\\" + cFileName;
                            module->addFile(file);
                            g_ThreadMgr.addJob(job);
                        }
                    }
                }
            }
        });
    }
}

void Workspace::enumerateModules(const fs::path& path, ModulesTypes type)
{
    // Scan source folder
    OS::visitFolders(path.string().c_str(), [&](const char* cFileName) {
		// Module name if equivalent to the folder name
		string moduleName;
		if (type == ModulesTypes::Tests)
			moduleName = "tests.";
		moduleName += cFileName;

		// Create module
		auto module = createOrUseModule(moduleName);
		module->fromTests = type == ModulesTypes::Tests;

		// Add the build.swg file if it exists
		string tmp;
		tmp = path.string() + cFileName + "/build.swg";
		if (fs::exists(tmp))
		{
			auto job = g_Pool_syntaxJob.alloc();
			auto file = g_Pool_sourceFile.alloc();
			job->sourceFile = file;
			file->fromTests = module->fromTests;
			file->path = tmp;
			module->buildFile = file;
			module->addFile(file);
			g_ThreadMgr.addJob(job);
		}

		// Parse all files in the "src" sub folder, except for tests where all the source code
		// is at the root folder
		tmp = path.string() + cFileName;
		module->path = tmp;
		if (type != ModulesTypes::Tests)
			tmp += "/src";
		enumerateFilesInModule(tmp, module); });
}

void Workspace::publishModule(Module* module)
{
    // Scan source folder
    string publishPath = module->path + "/publish";
    string searchPath  = publishPath;
    OS::visitFiles(searchPath.c_str(), [&](const char* cFileName) {
        auto job        = g_Pool_copyFileJob.alloc();
        job->module     = module;
        job->sourcePath = publishPath + "/" + cFileName;
        job->destPath   = targetPath.string() + "/" + cFileName;
        g_ThreadMgr.addJob(job);
    });
}

void Workspace::addRuntime()
{
    if (!g_CommandLine.addRuntimeModule)
        return;

    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    runtimeModule = g_Pool_module.alloc();
    runtimeModule->setup("");
    modules.push_back(runtimeModule);

    auto     file  = g_Pool_sourceFile.alloc();
    auto     job   = g_Pool_syntaxJob.alloc();
    fs::path p     = g_CommandLine.exePath;
    file->path     = p.parent_path().string() + "/swag.swg";
    file->module   = runtimeModule;
    file->swagFile = true;
    runtimeModule->addFile(file);
    job->sourceFile = file;
    g_ThreadMgr.addJob(job);
}

void Workspace::checkPendingJobs()
{
    if (g_ThreadMgr.pendingJobs.empty())
        return;

    for (auto pendingJob : g_ThreadMgr.pendingJobs)
    {
        auto firstNode = pendingJob->nodes.front();
        if (!(firstNode->flags & AST_GENERATED))
        {
            auto sourceFile = pendingJob->sourceFile;
            auto node       = pendingJob->nodes.back();
            if (!sourceFile->numErrors)
            {
                if (pendingJob->waitingSymbolSolved && !firstNode->name.empty())
                    sourceFile->report({node, node->token, format("cannot resolve %s '%s' because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str(), pendingJob->waitingSymbolSolved->name.c_str())});
                else if (pendingJob->waitingSymbolSolved)
                    sourceFile->report({node, node->token, format("cannot resolve %s because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), pendingJob->waitingSymbolSolved->name.c_str())});
                else
                    sourceFile->report({node, node->token, format("cannot resolve %s '%s'", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str())});
                sourceFile->numErrors = 0;
            }
        }
    }

    g_ThreadMgr.pendingJobs.clear();
}

bool Workspace::buildModules(const vector<Module*>& list)
{
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("-> dependency pass");

    auto timeBefore = chrono::high_resolution_clock::now();

    // Dependency pass
    // We add the "?.swg" file corresponding to the
    // module we want to import
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        module->hasBeenBuilt = true;
        for (auto& dep : module->moduleDependencies)
        {
            auto node = dep.second.node;

            // Now the .swg export file should be in the cache
            bool generated = true;
            auto path      = targetPath.string() + "\\" + node->name + ".generated.swg";
            if (!fs::exists(path))
            {
                generated = false;
                path      = targetPath.string() + "\\" + node->name + ".swg";
            }

            if (!fs::exists(path))
            {
                auto sourceFile = node->sourceFile;
                sourceFile->report({node, format("cannot find module export file '%s'", path.c_str())});
                continue;
            }

            // Then do syntax on it
            auto job             = g_Pool_syntaxJob.alloc();
            auto file            = g_Pool_sourceFile.alloc();
            job->sourceFile      = file;
            file->path           = move(path);
            file->generated      = true;
            dep.second.generated = generated;
            module->addFile(file);
            g_ThreadMgr.addJob(job);
        }
    }

    g_ThreadMgr.waitEndJobs();

    // Load all dependencies
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        for (const auto& dep : module->moduleDependencies)
        {
            if (!g_ModuleMgr.loadModule(dep.first))
            {
                module->error(format("fail to load module '%s' => %s", dep.first.c_str(), OS::getLastErrorAsString().c_str()));
            }
        }
    }

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("-> semantic pass");

    // Semantic pass on all 'build.swg' files
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        if (module == runtimeModule)
            continue;
        if (!module->buildFile)
            continue;
        auto job           = g_Pool_moduleSemanticJob.alloc();
        job->module        = module;
        job->buildFileMode = true;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();

    // Publish the "/publish" folder of each module
    // to the cache
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        if (module == runtimeModule)
            continue;
        publishModule(module);
    }

    // Semantic pass on the rest of the files,
    // for all modules
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        if (module == runtimeModule)
            continue;
        auto job           = g_Pool_moduleSemanticJob.alloc();
        job->module        = module;
        job->buildFileMode = false;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();
    checkPendingJobs();

    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    // Call bytecode test functions
    //////////////////////////////////////////////////
    for (auto module : list)
    {
        if ((g_CommandLine.test && g_CommandLine.runByteCodeTests && !module->byteCodeTestFunc.empty()) ||
            !module->byteCodeRunFunc.empty())
        {
            // INIT
            if (!module->byteCodeInitFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #init function(s)", module->name.c_str(), module->byteCodeInitFunc.size()));

                    for (auto func : module->byteCodeInitFunc)
                    {
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // #TEST
            if (g_CommandLine.test && g_CommandLine.runByteCodeTests && !module->byteCodeTestFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #test function(s)", module->name.c_str(), module->byteCodeTestFunc.size()));

                    for (auto func : module->byteCodeTestFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // #RUN
            if (!module->byteCodeRunFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #run function(s)", module->name.c_str(), module->byteCodeRunFunc.size()));

                    for (auto func : module->byteCodeRunFunc)
                    {
                        g_Stats.testFunctions++;
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }

            // DROP
            if (!module->byteCodeDropFunc.empty())
            {
                if (!module->numErrors)
                {
                    if (g_CommandLine.verboseBuildPass)
                        g_Log.verbose(format("   module '%s', bytecode execution of %d #drop function(s)", module->name.c_str(), module->byteCodeDropFunc.size()));

                    for (auto func : module->byteCodeDropFunc)
                    {
                        module->executeNode(func->node->sourceFile, func->node);
                    }
                }
            }
        }
    }

    // During unit testing, be sure we don't have
    // remaining not raised errors
    //////////////////////////////////////////////////
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
    {
        for (auto module : list)
        {
            for (auto file : module->files)
            {
                if (file->unittestError)
                {
                    auto nb             = file->unittestError;
                    file->unittestError = 0;
                    file->report({file, format("missing unittest errors: %d (%d raised)", nb, file->numErrors)});
                }
            }
        }
    }

    // Output pass on all modules
    //////////////////////////////////////////////////
    if (g_CommandLine.backendOutput)
    {
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose("-> backend pass");

        timeBefore = chrono::high_resolution_clock::now();

        for (auto module : list)
        {
            if (module->numErrors > 0)
                continue;
            if (module->name.empty())
                continue;
            if (module->buildPass < BuildPass::Backend)
                continue;
            if (module->files.size() == 0)
                continue;

            auto outputJob    = g_Pool_moduleOutputJob.alloc();
            outputJob->module = module;
            g_ThreadMgr.addJob(outputJob);
        }

        g_ThreadMgr.waitEndJobs();

        timeAfter = chrono::high_resolution_clock::now();
        g_Stats.backendTime += timeAfter - timeBefore;
    }

    return true;
}

void Workspace::setup()
{
    workspacePath = g_CommandLine.workspacePath;
    if (!fs::exists(workspacePath))
    {
        g_Log.error(format("fatal error: workspace folder '%s' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    testsPath = workspacePath;
    testsPath.append("tests/");
    modulesPath = workspacePath;
    modulesPath.append("modules/");
    dependenciesPath = workspacePath;
    dependenciesPath.append("dep/");

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("=> building workspace '%s'", workspacePath.string().c_str()));

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

void Workspace::setupTarget()
{
    targetPath = workspacePath;
    targetPath.append("out/");
    targetPath.append(g_CommandLine.config + "-" + g_CommandLine.arch);

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("=> target is '%s'", targetPath.string().c_str()));

    // Clean target
    if (g_CommandLine.cleanTarget)
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

    targetTestPath = targetPath;
    targetTestPath.append("/");
    if (!fs::exists(targetTestPath) && !fs::create_directories(targetTestPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create target directory '%s'", targetTestPath.string().c_str()));
        exit(-1);
    }
}

bool Workspace::buildTarget()
{
    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("## starting syntax pass");

    auto timeBefore = chrono::high_resolution_clock::now();
    if (g_CommandLine.test)
        enumerateModules(testsPath, ModulesTypes::Tests);
    enumerateModules(modulesPath, ModulesTypes::Workspace);
    enumerateModules(dependenciesPath, ModulesTypes::Dependencies);
    g_ThreadMgr.waitEndJobs();
    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   syntax pass done on %d file(s) in %d module(s)", g_Stats.numFiles.load(), modules.size()));

    // Runtime swag module semantic pass
    //////////////////////////////////////////////////
    if (runtimeModule)
    {
        // Errors in swag.swg !!!
        if (runtimeModule->numErrors)
        {
            g_Log.error("some errors have been found in 'swag.swg' ! exiting...");
            return false;
        }

        auto job    = g_Pool_moduleSemanticJob.alloc();
        job->module = runtimeModule;
        g_ThreadMgr.addJob(job);
        g_ThreadMgr.waitEndJobs();

        // Errors in swag.swg !!!
        if (runtimeModule->numErrors)
        {
            g_Log.error("some errors have been found in 'swag.swg' ! exiting...");
            return false;
        }
    }

    // Build modules in dependency order
    //////////////////////////////////////////////////
    vector<Module*> order;
    vector<Module*> remain = modules;
    vector<Module*> nextRemain;
    int             pass = 1;
    while (!remain.empty())
    {
        for (auto module : remain)
        {
            if (module->numErrors > 0)
                continue;
            if (module->buildPass < BuildPass::Semantic)
                continue;

            bool canBuild  = true;
            bool hasErrors = false;
            for (const auto& dep : module->moduleDependencies)
            {
                auto it = mapModulesNames.find(dep.first);
                if (it != mapModulesNames.end() && it->second->numErrors)
                {
                    hasErrors = true;
                }
                if (it != mapModulesNames.end() && !it->second->hasBeenBuilt)
                {
                    canBuild = false;
                    break;
                }
            }

            if (hasErrors)
                module->numErrors++;
            else if (canBuild)
                order.push_back(module);
            else
                nextRemain.push_back(module);
        }

        // Nothing to build !
        if (order.empty())
        {
            for (auto module : remain)
            {
                if (!module->numErrors)
                    module->error("cannot compute the build order. do you have a dependency cycle ?");
            }

            return false;
        }

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("## starting build pass #%d on %d module(s)", pass, (int) order.size()));

        buildModules(order);

        pass++;
        remain = nextRemain;
        order.clear();
        nextRemain.clear();
    }

    return true;
}

bool Workspace::build()
{
    setup();

    g_Log.messageHeaderCentered("Workspace", format("%s [%s-%s]", workspacePath.filename().string().c_str(), g_CommandLine.config.c_str(), g_CommandLine.arch.c_str()));

    addRuntime();
    setupTarget();
    return buildTarget();
}