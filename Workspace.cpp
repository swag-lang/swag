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

void Workspace::enumerateFilesInModule(const fs::path& path, Module* module, bool tests)
{
    // Scan source folder
    WIN32_FIND_DATAA findfile;
    vector<string>   directories;

    directories.push_back(path.string());
    string     tmp, tmp1;
    fs::path   modulePath;
    auto       leftNameOffset = workspacePath.string().size() + 1;
    char       moduleName[_MAX_PATH];
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
            if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                    continue;
                tmp1 = tmp + findfile.cFileName;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(findfile.cFileName, '.');
                if (pz && !_strcmpi(pz, ".swg"))
                {
                    // File filtering by name
                    if (filterIsEmpty || strstr(findfile.cFileName, g_CommandLine.fileFilter.c_str()))
                    {
                        auto job  = g_Pool_syntaxJob.alloc();
                        auto file = g_Pool_sourceFile.alloc();

                        // When gattering files from the test folder, each file is in its own module
                        if (tests)
                        {
                            // Generate a module name, depending on the test file location, and its name
                            auto pz1 = tmp.c_str() + leftNameOffset;
                            auto pzn = moduleName;
                            while (*pz1)
                            {
                                if (*pz1 == '/' || *pz1 == '\\')
                                    *pzn++ = '_';
                                else
                                    *pzn++ = *pz1;
                                pz1++;
                            }

                            *pzn++ = '_';
                            pz1    = findfile.cFileName;
                            while (true)
                            {
                                if (pz1[0] == '.' && pz1[4] == 0)
                                    break;
                                *pzn++ = *pz1++;
                            }

                            *pzn                   = 0;
                            module                 = createOrUseModule(moduleName);
                            module->compileVersion = g_CommandLine.compileVersion;
                        }

                        job->sourceFile = file;
                        module->addFile(file);
                        file->path = tmp + "\\" + findfile.cFileName;

                        g_ThreadMgr.addJob(job);
                    }
                }
            }
        } while (::FindNextFileA(h, &findfile));

        ::FindClose(h);
    }
}

void Workspace::enumerateModules()
{
    // Add ./tests folder
    if (g_CommandLine.test)
        enumerateFilesInModule(testsPath, nullptr, true);

    // Scan source folder
    WIN32_FIND_DATAA findfile;
    string           searchPath = sourcePath.string() + "/*";
    HANDLE           h          = ::FindFirstFileA(searchPath.c_str(), &findfile);
    if (h != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                continue;
            if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                continue;

            auto module            = createOrUseModule(findfile.cFileName);
            module->compileVersion = g_CommandLine.compileVersion;
            string tmp             = sourcePath.string() + findfile.cFileName;
            enumerateFilesInModule(tmp, module, false);

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
        g_Log.verbose("starting dependency pass");

    auto timeBefore = chrono::high_resolution_clock::now();

    // Dependency pass
    for (auto module : list)
    {
        module->hasBeenBuilt = true;
        for (auto node : module->moduleDependencies)
        {
            // Now the .swg export file is in the cache
            auto path = targetPath.string() + node->name + ".swg";
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

    // Errors in swag.swg !!!
    if (runtimeModule && runtimeModule->numErrors)
    {
        g_Log.error("some syntax errors have been found in 'swag.swg' ! exiting...");
        return false;
    }

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("starting semantic pass");

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
            auto node        = semanticJob->nodes.back();
            auto sourceFile  = semanticJob->sourceFile;
            if (!sourceFile->module->numErrors)
            {
                auto& name = semanticJob->waitingSymbolSolved ? semanticJob->waitingSymbolSolved->name : node->name;
                sourceFile->report({sourceFile, node->token, format("cannot resolve type of identifier '%s'", name.c_str())});
                sourceFile->module->numErrors = 0;
            }
        }

        g_ThreadMgr.pendingJobs.clear();
    }

    // Call bytecode test functions
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
    {
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose("running bytecode test functions");

        for (auto module : list)
        {
            if (!module->numErrors)
            {
                for (auto func : module->byteCodeInitFunc)
                {
                    module->executeNode(module->files[func->node->sourceFileIdx], func->node);
                }
            }

            if (!module->numErrors)
            {
                for (auto func : module->byteCodeTestFunc)
                {
                    g_Stats.testFunctions++;
                    module->executeNode(module->files[func->node->sourceFileIdx], func->node);
                }
            }

            if (!module->numErrors)
            {
                for (auto func : module->byteCodeRunFunc)
                {
                    g_Stats.testFunctions++;
                    module->executeNode(module->files[func->node->sourceFileIdx], func->node);
                }
            }

            if (!module->numErrors)
            {
                for (auto func : module->byteCodeDropFunc)
                {
                    module->executeNode(module->files[func->node->sourceFileIdx], func->node);
                }
            }
        }
    }

    // Output pass on all modules
    if (g_CommandLine.backendOutput)
    {
        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose("starting backend pass...");

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
        g_Log.verbose(format("building workspace '%s'", workspacePath.string().c_str()));

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
    auto outPath = workspacePath;
    outPath.append("out/");

    targetPath = outPath;
    targetPath.append(currentTarget->name);
    targetPath.append("/");

    // Clean target
    if (g_CommandLine.cleanTarget)
    {
        if (fs::exists(targetPath))
            clearPath(targetPath);
    }

    // Be sure the output folder exists
    if (!fs::exists(outPath))
    {
        try
        {
            fs::create_directory(outPath);
        }
        catch (...)
        {
            g_Log.error(format("fatal error: cannot create output directory '%s'", outPath.string().c_str()));
            exit(-1);
        }
    }

    // Be sure the output folder exists
    if (!fs::exists(targetPath))
    {
        try
        {
            fs::create_directory(targetPath);
        }
        catch (...)
        {
            g_Log.error(format("fatal error: cannot create target directory '%s'", targetPath.string().c_str()));
            exit(-1);
        }
    }
}

bool Workspace::buildTarget()
{
    // Ask for a syntax pass on all files of all modules
    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("starting syntax pass");

    auto timeBefore = chrono::high_resolution_clock::now();
    enumerateModules();
    g_ThreadMgr.waitEndJobs();
    auto timeAfter = chrono::high_resolution_clock::now();
    g_Stats.frontendTime += timeAfter - timeBefore;

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("syntax pass done on %d file(s) in %d module(s)", g_Stats.numFiles.load(), modules.size()));

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

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("starting build pass on %d module(s)", (int) order.size()));

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

    auto target                     = new Target;
    target->name                    = "debug";
    target->backendDebugInformations       = false;
    target->backendOptimizeLevel           = 0;
    target->backendC.writeSourceCode     = true;
    target->backendC.writeByteCodeInstruction = true;

	bool ok = true;

	currentTarget = target;
    addRuntime();
    setupTarget();
    ok &= buildTarget();

	return ok;
}