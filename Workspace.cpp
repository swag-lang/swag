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
#include "ModuleBuildJob.h"
#include "Os.h"

Workspace g_Workspace;

Module* Workspace::getModuleByName(const string& moduleName)
{
    shared_lock lk(mutexModules);
    auto        it = mapModulesNames.find(moduleName);
    if (it == mapModulesNames.end())
        return nullptr;
    return it->second;
}

Module* Workspace::createOrUseModule(const string& moduleName)
{
    Module* module;

    {
        unique_lock lk(mutexModules);

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

        OS::visitFilesFolders(tmp.c_str(), [&](uint64_t writeTime, const char* cFileName, bool isFolder) {
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
                        auto job        = g_Pool_syntaxJob.alloc();
                        auto file       = g_Pool_sourceFile.alloc();
                        job->sourceFile = file;
                        file->fromTests = module->fromTests;
                        file->path      = tmp + "\\" + cFileName;
                        file->writeTime = writeTime;
                        module->addFile(file);
                        g_ThreadMgr.addJob(job);
                    }
                }
            }
        });
    }
}

Module* Workspace::addModule(const fs::path& path)
{
    auto parent    = path.parent_path().filename();
    auto cFileName = path.filename().string();

    // Module name is equivalent to the folder name
    string moduleName;
    if (parent == "tests")
        moduleName = "tests.";
    moduleName += cFileName;

    // Create module
    auto module       = createOrUseModule(moduleName);
    module->fromTests = parent == "tests";

    // Add the build.swg file if it exists
    string tmp;
    tmp = path.string() + "/build.swg";
    if (fs::exists(tmp))
    {
        auto job          = g_Pool_syntaxJob.alloc();
        auto file         = g_Pool_sourceFile.alloc();
        job->sourceFile   = file;
        file->fromTests   = module->fromTests;
        file->path        = tmp;
        module->buildFile = file;
        module->addFile(file);
        g_ThreadMgr.addJob(job);
    }

    // Parse all files in the "src" sub folder, except for tests where all the source code
    // is at the root folder
    tmp          = path.string();
    module->path = tmp;
    if (!module->fromTests)
        tmp += "/src";
    enumerateFilesInModule(tmp, module);
    return module;
}

void Workspace::enumerateModules(const fs::path& path)
{
    // Scan source folder
    OS::visitFolders(path.string().c_str(), [&](const char* cFileName) {
        addModule(path.string() + cFileName);
    });
}

void Workspace::enumerateModules()
{
    // If no module is specified, just compiled all modules in the workspace
    if (g_CommandLine.modulePath.empty())
    {
        enumerateModules(dependenciesPath);
        enumerateModules(modulesPath);
        if (g_CommandLine.test)
            enumerateModules(testsPath);
        g_ThreadMgr.waitEndJobs();
    }

    // Else compile the module and its dependencies
    else
    {
        vector<fs::path> modulesToGo;
        fs::path         modulePath = g_CommandLine.workspacePath;
        modulePath.append(g_CommandLine.modulePath);
        modulesToGo.push_back(modulePath);
        for (int i = 0; i < modulesToGo.size(); i++)
        {
            auto module = addModule(modulesToGo[i]);
            g_ThreadMgr.waitEndJobs();
            for (auto& dep : module->moduleDependencies)
            {
                auto it = mapModulesNames.find(dep.first);
                if (it == mapModulesNames.end())
                {
                    auto depName = dep.second.name;
                    auto depPath = dependenciesPath;
                    depPath.append(depName);
                    if (!fs::exists(depPath))
                    {
                        depPath = modulesPath;
                        depPath.append(depName);
                        if (!fs::exists(depPath))
                        {
                            continue;
                        }
                    }

                    modulesToGo.push_back(depPath);
                }
            }
        }
    }
}

void Workspace::publishModule(Module* module)
{
    if (module->path.empty())
        return;
    // Scan source folder
    string publishPath = module->path + "/publish";
    if (fs::exists(publishPath))
    {
        OS::visitFiles(publishPath.c_str(), [&](const char* cFileName) {
            auto job        = g_Pool_copyFileJob.alloc();
            job->module     = module;
            job->sourcePath = publishPath + "/" + cFileName;
            job->destPath   = targetPath.string() + "/" + cFileName;
            g_ThreadMgr.addJob(job);
        });
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
}

bool Workspace::buildTarget()
{
    // Ask for a syntax pass on all files of all modules
    //////////////////////////////////////////////////

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose("## starting syntax pass");

    auto timeBefore = chrono::high_resolution_clock::now();
    enumerateModules();
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

    // Build modules
    //////////////////////////////////////////////////
    for (auto module : modules)
    {
		if (module == runtimeModule)
			continue;
        auto job    = g_Pool_moduleBuildJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();
    return true;
}

bool Workspace::build()
{
    setup();

    g_Log.messageHeaderCentered("Workspace", format("%s [%s-%s]", workspacePath.filename().string().c_str(), g_CommandLine.config.c_str(), g_CommandLine.arch.c_str()));
    addRuntime();
    setupTarget();
    SWAG_CHECK(buildTarget());

	return true;
}