#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Stats.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ByteCode.h"
#include "Target.h"
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
    WIN32_FIND_DATAA findfile;
    vector<string>   directories;

    directories.push_back(path.string());
    string   tmp, tmp1;
    fs::path modulePath;
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

        tmp1     = tmp + "\\*";
        HANDLE h = ::FindFirstFileA(tmp1.c_str(), &findfile);
        if (h == INVALID_HANDLE_VALUE)
            continue;

        do
        {
            if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                    continue;
                tmp1 = tmp + "/" + findfile.cFileName;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(findfile.cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    if (g_CommandLine.fileFilter.empty() || strstr(findfile.cFileName, g_CommandLine.fileFilter.c_str()))
                    {
                        // File filtering by name
                        if (!module->fromTests || g_CommandLine.testFilter.empty() || strstr(findfile.cFileName, g_CommandLine.testFilter.c_str()))
                        {
                            auto job        = g_Pool_syntaxJob.alloc();
                            auto file       = g_Pool_sourceFile.alloc();
                            job->sourceFile = file;
                            file->fromTests = module->fromTests;
                            file->path      = tmp + "\\" + findfile.cFileName;
                            module->addFile(file);
                            g_ThreadMgr.addJob(job);
                        }
                    }
                }
            }
        } while (::FindNextFileA(h, &findfile));

        ::FindClose(h);
    }
}

void Workspace::enumerateModules(const fs::path& path, bool fromTests)
{
    // Scan source folder
    WIN32_FIND_DATAA findfile;
    string           searchPath = path.string() + "/*";
    HANDLE           h          = ::FindFirstFileA(searchPath.c_str(), &findfile);
    if (h != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                continue;
            if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                continue;

            string moduleName;
            if (fromTests)
                moduleName = "tests.";
            moduleName += findfile.cFileName;

            auto module       = createOrUseModule(moduleName);
            module->fromTests = fromTests;
            string tmp        = path.string() + findfile.cFileName;
            enumerateFilesInModule(tmp, module);

        } while (::FindNextFileA(h, &findfile));

        ::FindClose(h);
    }
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

bool Workspace::buildModules(const vector<Module*>& list)
{
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("-> dependency pass");

    auto timeBefore = chrono::high_resolution_clock::now();

    // Dependency pass
    for (auto module : list)
    {
        module->hasBeenBuilt = true;
        for (auto node : module->moduleDependencies)
        {
            // Now the .swg export file is in the cache
            auto path = targetPath.string() + "\\" + node->name + ".swg";
            if (!fs::exists(path))
            {
                auto sourceFile = node->sourceFile;
                sourceFile->report({node, format("cannot find module export file '%s'", path.c_str())});
                continue;
            }

            // Then do syntax on it
            auto job        = g_Pool_syntaxJob.alloc();
            auto file       = g_Pool_sourceFile.alloc();
            job->sourceFile = file;
            file->path      = move(path);
            file->generated = true;
            module->addFile(file);
            g_ThreadMgr.addJob(job);
        }
    }

    g_ThreadMgr.waitEndJobs();

    // Then load all dependencies
    for (auto module : list)
    {
        for (auto name : module->moduleDependenciesNames)
        {
            if (!g_ModuleMgr.loadModule(name))
            {
                module->error(format("fail to load module '%s' => %s", name.c_str(), OS::getLastErrorAsString().c_str()));
            }
        }
    }

    // Errors in swag.swg !!!
    if (runtimeModule && runtimeModule->numErrors)
    {
        g_Log.error("some syntax errors have been found in 'swag.swg' ! exiting...");
        return false;
    }

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("-> semantic pass");

    // Semantic pass on runtime module first
    ModuleSemanticJob* job = nullptr;
    if (runtimeModule)
    {
        job         = g_Pool_moduleSemanticJob.alloc();
        job->module = runtimeModule;
        g_ThreadMgr.addJob(job);
        g_ThreadMgr.waitEndJobs();

        // Errors in swag.swg !!!
        if (runtimeModule->numErrors)
        {
            g_Log.error("some semantic errors have been found in 'swag.swg' ! exiting...");
            return false;
        }
    }

    // Semantic pass on all other modules
    for (auto module : list)
    {
        if (module == runtimeModule)
            continue;
        job         = g_Pool_moduleSemanticJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    // If we have some pending jobs, that means we don't have succeeded to resolve everything
    if (g_ThreadMgr.pendingJobs.size() > 0)
    {
        for (auto pendingJob : g_ThreadMgr.pendingJobs)
        {
            auto semanticJob = static_cast<SemanticJob*>(pendingJob);
            auto firstNode   = semanticJob->nodes.front();
            if (!(firstNode->flags & AST_GENERATED))
            {
                auto sourceFile = semanticJob->sourceFile;
                auto node       = semanticJob->nodes.back();
                if (!sourceFile->numErrors)
                {
                    auto& name = semanticJob->waitingSymbolSolved ? semanticJob->waitingSymbolSolved->name : node->name;
                    sourceFile->report({node, node->token, format("cannot resolve %s '%s' because identifier '%s' has not been solved (do you have a cycle ?)", AstNode::getNakedKindName(firstNode).c_str(), firstNode->name.c_str(), name.c_str())});
                    sourceFile->numErrors = 0;
                }
            }
        }

        g_ThreadMgr.pendingJobs.clear();
    }

    // Call bytecode test functions
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

    // During unit testing, be sure we don't have remaining not raised errors
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
    sourcePath = workspacePath;
    sourcePath.append("src/");

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("=> building workspace '%s'", workspacePath.string().c_str()));

    g_ThreadMgr.init();
}

void Workspace::clearPath(const fs::path& path)
{
    for (auto& p : fs::directory_iterator(path))
    {
        try
        {
            fs::remove_all(p.path());
        }
        catch (...)
        {
            g_Log.error(format("fatal error: cannot delete file '%s'", p.path().string().c_str()));
            exit(-1);
        }
    }
}

void Workspace::setupTarget()
{
    targetPath = workspacePath;
    targetPath.append("out/");
    targetPath.append(currentTarget->config + "-" + currentTarget->arch);

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("=> target is '%s'", targetPath.string().c_str()));

    // Clean target
    if (g_CommandLine.cleanTarget)
    {
        if (fs::exists(targetPath))
            clearPath(targetPath);
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
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("## starting syntax pass");

    auto timeBefore = chrono::high_resolution_clock::now();
    // Add ./tests folder
    if (g_CommandLine.test)
        enumerateModules(testsPath, true);
    enumerateModules(sourcePath, false);
    g_ThreadMgr.waitEndJobs();
    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   syntax pass done on %d file(s) in %d module(s)", g_Stats.numFiles.load(), modules.size()));

    // Build modules in dependency order
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
            for (auto depName : module->moduleDependenciesNames)
            {
                auto it = mapModulesNames.find(depName);
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

    auto target                               = new Target;
    target->config                            = g_CommandLine.config;
    target->arch                              = g_CommandLine.arch;
	target->backendDebugInformations = true;// false;
    target->backendOptimizeLevel              = 0;
    target->backendC.writeSourceCode          = true;
    target->backendC.writeByteCodeInstruction = true;

    bool ok = true;

    g_Log.messageHeaderCentered("Workspace", format("%s [%s-%s]", workspacePath.filename().string().c_str(), target->config.c_str(), target->arch.c_str()));

    currentTarget = target;
    addRuntime();
    setupTarget();
    ok &= buildTarget();

    return ok;
}