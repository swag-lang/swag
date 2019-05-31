#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Stats.h"
#include "Global.h"
#include "CommandLine.h"
#include "Diagnostic.h"
#include "Runtime.h"

Module* Workspace::createOrUseModule(const fs::path& path)
{
    Module* module;

    {
        scoped_lock lk(mutexModules);

        auto it = mapModules.find(path);
        if (it != mapModules.end())
            return it->second;

        module = new Module(this, path);
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
                        auto job  = g_Pool.syntaxJob.alloc();
                        auto file = g_Pool.sourceFile.alloc();

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
    scopeRoot       = poolFactory.scope.alloc();
    scopeRoot->kind = ScopeKind::Workspace;
    scopeRoot->allocateSymTable();

    // Runtime will be compiled in the workspace scope, in order to be defined once
    // for all modules
    auto runtimeModule = new Module(this, "", true);
    modules.push_back(runtimeModule);

    auto file = g_Pool.sourceFile.alloc();
    auto job  = g_Pool.syntaxJob.alloc();
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
    if (g_CommandLine.syntaxOnly)
        return true;

    // Semantic pass on all modules
    for (auto module : modules)
    {
        if (module->numErrors > 0)
            continue;
        auto job    = g_Pool.moduleSemanticJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    g_ThreadMgr.waitEndJobs();

    if (g_CommandLine.output == false)
        return true;

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

    // Output pass on all modules
    for (auto module : modules)
    {
        if (module->numErrors > 0)
            continue;
        auto job    = g_Pool.moduleOutputJob.alloc();
        job->module = module;
        g_ThreadMgr.addJob(job);
    }

    return true;
}