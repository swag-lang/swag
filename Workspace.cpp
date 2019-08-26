#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Stats.h"
#include "Runtime.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ByteCode.h"
#include "SymTable.h"

Workspace g_Workspace;

Module* Workspace::getModuleByName(const string& name)
{
    scoped_lock lk(mutexModules);
    auto        it = mapModulesNames.find(name);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const fs::path& path)
{
    Module* module;

    {
        scoped_lock lk(mutexModules);

        auto it = mapModulesPaths.find(path);
        if (it != mapModulesPaths.end())
            return it->second;

        module = g_Pool_module.alloc();
        module->setup(this, path);
        modules.push_back(module);
        mapModulesPaths[path]         = module;
        mapModulesNames[module->name] = module;
    }

    if (g_CommandLine.stats)
        g_Stats.numModules++;
    if (g_CommandLine.debug)
        module->backendParameters.debugInformations = true;

    return module;
}

void Workspace::enumerateFilesInModule(const fs::path& path)
{
    // Create a default module
    auto module            = createOrUseModule(path);
    module->compileVersion = g_CommandLine.compileVersion;

    // Scan source folder
    WIN32_FIND_DATAA findfile;
    vector<string>   directories;

    directories.push_back(path.string());
    string     tmp, tmp1;
    const bool filterIsEmpty = g_CommandLine.fileFilter.empty();
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
            tmp1 = tmp + "\\" + findfile.cFileName;
            if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                    continue;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(findfile.cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    // File filtering by name
                    if (filterIsEmpty || strstr(tmp1.c_str(), g_CommandLine.fileFilter.c_str()))
                    {
                        auto job  = g_Pool_syntaxJob.alloc();
                        auto file = g_Pool_sourceFile.alloc();

                        job->sourceFile = file;
                        module->addFile(file);
                        file->path = move(tmp1);

                        g_ThreadMgr.addJob(job);
                    }
                }
            }
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
    runtimeModule->setup(this, "");
    modules.push_back(runtimeModule);

    auto file = g_Pool_sourceFile.alloc();
    auto job  = g_Pool_syntaxJob.alloc();
    runtimeModule->addFile(file);
    job->sourceFile      = file;
    file->path           = "<swag.runtime>";
    file->module         = runtimeModule;
    file->externalBuffer = g_Runtime;
    g_ThreadMgr.addJob(job);
}

void Workspace::removeCache()
{
    for (auto& p : fs::directory_iterator(cachePath))
    {
        bool ok = true;
        try
        {
            ok = fs::remove_all(p.path());
        }
        catch (...)
        {
            ok = false;
        }

        if (!ok)
        {
            g_Log.error(format("fatal error: can't delete cache content '%s'", p.path().string().c_str()));
            exit(-1);
        }
    }
}

void Workspace::enumerateModules()
{
    cachePath = "f:/temp/";

    // Clean cache
    if (g_CommandLine.unittest || g_CommandLine.cleanCache)
    {
        if (fs::exists(cachePath))
            removeCache();
    }

    // Be sure the cache folder exists
    if (!fs::exists(cachePath))
    {
        if (!fs::create_directory(cachePath))
        {
            g_Log.error(format("fatal error: can't create cache directory '%s'", cachePath.c_str()));
            exit(-1);
        }
    }

    enumerateFilesInModule("f:/swag/unittest/single");
}

bool Workspace::buildModules(const vector<Module*>& list)
{
    if (g_CommandLine.verbose_build_pass)
        g_Log.verbose("starting dependency pass...");

    // Dependency pass
    for (auto module : list)
    {
        module->hasBeenBuilt = true;
        for (auto node : module->moduleDependencies)
        {
            // Now the .swg export file is in the cache
            auto path = cachePath.string() + node->name + ".swg";
            if (!fs::exists(path))
            {
                auto sourceFile = module->files[node->sourceFileIdx];
                sourceFile->report({sourceFile, node, format("could not find module export file '%s'", path.c_str())});
                continue;
            }

            // Then do syntax on it
            auto job        = g_Pool_syntaxJob.alloc();
            auto file       = g_Pool_sourceFile.alloc();
            job->sourceFile = file;
            module->addFile(file);
            file->path = move(path);
            g_ThreadMgr.addJob(job);
        }
    }

    g_ThreadMgr.waitEndJobs();

    if (g_CommandLine.verbose_build_pass)
        g_Log.verbose("starting semantic pass...");

    // Semantic pass
    for (auto module : list)
    {
        auto job    = g_Pool_moduleSemanticJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    // If we have some pending jobs, that means we don't have succeeded to resolve everything
    if (g_ThreadMgr.pendingJobs.size() > 0)
    {
        for (auto pendingJob : g_ThreadMgr.pendingJobs)
        {
            auto semanticJob = static_cast<SemanticJob*>(pendingJob);
            auto node        = semanticJob->nodes.back();
            auto sourceFile  = semanticJob->sourceFile;
            if (!sourceFile->module->numErrors)
            {
                auto& name = semanticJob->waitingSymbolSolved ? semanticJob->waitingSymbolSolved->name : node->name;
                sourceFile->report({sourceFile, node->token, format("cannot resolve type of identifier '%s'", name.c_str())});
            }
        }

        g_ThreadMgr.pendingJobs.clear();
    }

    // Output pass on all modules
    if (g_CommandLine.output)
    {
        if (g_CommandLine.verbose_build_pass)
            g_Log.verbose("starting backend pass...");

        auto timeBefore = chrono::high_resolution_clock::now();

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

        auto timeAfter = chrono::high_resolution_clock::now();
        g_Stats.outputTime += timeAfter - timeBefore;
    }

    // Call test functions
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
    {
        if (g_CommandLine.verbose_build_pass)
            g_Log.verbose("running bytecode test functions...");

        for (auto module : list)
        {
            for (auto func : module->byteCodeTestFunc)
            {
                if (func->sourceFile->module->numErrors)
                    continue;
                g_Stats.testFunctions++;
                module->executeNode(module->files[func->node->sourceFileIdx], func->node);
            }
        }
    }

    // During unit testing, be sure we don't have remaining not raised errors
    if (g_CommandLine.unittest)
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

    return true;
}

bool Workspace::build()
{
    g_ThreadMgr.init();

    // Ask for a syntax pass on all files of all modules
    addRuntime();
    enumerateModules();
    g_ThreadMgr.waitEndJobs();

    if (g_CommandLine.verbose_build_pass)
        g_Log.verbose(format("## syntax pass done on %d module(s)", modules.size()));

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
                module->error("module can't be compiled because it depends on other modules with errors");
            else if (canBuild)
                order.push_back(module);
            else
                nextRemain.push_back(module);
        }

        // Nothing to build !
        if (order.empty())
        {
            for (auto module : remain)
                module->error("can't compute the build order. do you have a dependency cycle ?");
            return false;
        }

        if (g_CommandLine.verbose_build_pass)
            g_Log.verbose(format("## starting build pass %d on %d module(s)", pass, (int) order.size()));

        buildModules(order);

        pass++;
        remain = nextRemain;
        order.clear();
        nextRemain.clear();
    }

    return true;
}