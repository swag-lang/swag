#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Stats.h"
#include "Global.h"
#include "CommandLine.h"
#include "Diagnostic.h"
#include "Runtime.h"
#include "SourceFile.h"
#include "Scope.h"
#include "SemanticJob.h"
#include "ModuleSemanticJob.h"
#include "ModuleOutputJob.h"
#include "ByteCode.h"

Workspace g_Workspace;

Module* Workspace::createOrUseModule(const fs::path& path)
{
    Module* module;

    {
        scoped_lock lk(mutexModules);

        auto it = mapModules.find(path);
        if (it != mapModules.end())
            return it->second;

        module = g_Pool_module.alloc();
        module->setup(this, path);
        modules.push_back(module);
        mapModules[path] = module;
    }

    if (g_CommandLine.stats)
        g_Stats.numModules++;

    return module;
}

void Workspace::enumerateFilesInModule(const fs::path& path)
{
    // Create a default module
    auto module = createOrUseModule(path);

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

void Workspace::buildRuntime()
{
    scopeRoot       = g_Pool_scope.alloc();
    scopeRoot->kind = ScopeKind::Workspace;
    scopeRoot->allocateSymTable();

    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    auto runtimeModule = g_Pool_module.alloc();
    runtimeModule->setup(this, "", true);
    runtimeModule->isRuntime = true;
    modules.push_back(runtimeModule);

    auto file = g_Pool_sourceFile.alloc();
    auto job  = g_Pool_syntaxJob.alloc();
    runtimeModule->addFile(file);
    job->sourceFile      = file;
    job->currentScope    = scopeRoot;
    file->path           = "<swag.runtime>";
    file->module         = runtimeModule;
    file->externalBuffer = g_Runtime;
    g_ThreadMgr.addJob(job);
}

void Workspace::enumerateModules()
{
    enumerateFilesInModule("f:\\swag\\unittest");
}

bool Workspace::build()
{
    g_ThreadMgr.init();

    buildRuntime();

    // Ask for a syntax pass on all files of all modules
    enumerateModules();
    g_ThreadMgr.waitEndJobs();

    // Semantic pass on all modules
    for (auto module : modules)
    {
        if (module->numErrors > 0)
            continue;
        if (module->buildPass < BuildPass::Semantic)
            continue;
        auto job    = g_Pool_moduleSemanticJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    // If we have some pending jobs, that means we don't have succeeded to resolve everything
    if (g_ThreadMgr.pendingJobs.size() > 0)
    {
        for (auto job : g_ThreadMgr.pendingJobs)
        {
            auto semanticJob = static_cast<SemanticJob*>(job);
            auto node        = semanticJob->nodes.back();
            auto sourceFile  = semanticJob->sourceFile;
            sourceFile->report({sourceFile, node->token, format("can't resolve type of identifier '%s'", node->name.c_str())});
        }
    }

    // Call test functions
    if (g_CommandLine.test && g_CommandLine.runByteCodeTests)
    {
        for (auto module : modules)
        {
            for (auto func : module->byteCodeTestFunc)
            {
                g_Stats.testFunctions++;
                module->executeNode(module->files[func->node->sourceFileIdx], func->node);
            }
        }
    }

    if (g_CommandLine.output == false)
        return true;

    // Output pass on all modules
    if (g_CommandLine.output)
    {
        auto timeBefore = chrono::high_resolution_clock::now();
        for (auto module : modules)
        {
            if (module->numErrors > 0)
                continue;
            if (module->name.empty())
                continue;
            if (module->buildPass < BuildPass::Backend)
                continue;
            if (module->files.size() == 0)
                continue;
            auto job    = g_Pool_moduleOutputJob.alloc();
            job->module = module;
            g_ThreadMgr.addJob(job);
        }

        g_ThreadMgr.waitEndJobs();
        auto timeAfter = chrono::high_resolution_clock::now();
        g_Stats.outputTime += timeAfter - timeBefore;
    }

    return true;
}