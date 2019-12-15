#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Stats.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "CopyFileJob.h"
#include "EnumerateModuleJob.h"
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

void Workspace::addBootstrap()
{
    if (!g_CommandLine.addBootstrap)
        return;

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
    bootstrapModule = g_Pool_module.alloc();
    bootstrapModule->setup("");
    modules.push_back(bootstrapModule);

    auto     file        = g_Pool_sourceFile.alloc();
    auto     job         = g_Pool_syntaxJob.alloc();
    fs::path p           = g_CommandLine.exePath;
    file->path           = p.parent_path().string() + "/bootstrap.swg";
    file->module         = bootstrapModule;
    file->swagFile       = true;
    file->externalBuffer = (uint8_t*) ptr;
    file->externalSize   = size;
    bootstrapModule->addFile(file);
    job->sourceFile = file;
    g_ThreadMgr.addJob(job);
}

void Workspace::setup()
{
    workspacePath = g_CommandLine.workspacePath;
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

    testsPath = workspacePath;
    testsPath.append("tests/");
    if (!fs::exists(testsPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'tests/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    modulesPath = workspacePath;
    modulesPath.append("modules/");
    if (!fs::exists(modulesPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'modules/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

    dependenciesPath = workspacePath;
    dependenciesPath.append("dependencies/");
    if (!fs::exists(dependenciesPath))
    {
        g_Log.error(format("fatal error: invalid workspace '%s', subfolder 'dependencies/' does not exist", workspacePath.string().c_str()));
        exit(-1);
    }

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
    targetPath.append("output/");
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

    auto enumJob = new EnumerateModuleJob;
    g_ThreadMgr.addJob(enumJob);
    g_ThreadMgr.waitEndJobs();

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   syntax pass done on %d file(s) in %d module(s)", g_Stats.numFiles.load(), modules.size()));

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

    // Build modules
    //////////////////////////////////////////////////
    for (auto module : modules)
    {
        if (module == bootstrapModule)
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
    addBootstrap();
    setupTarget();
    SWAG_CHECK(buildTarget());

    return true;
}