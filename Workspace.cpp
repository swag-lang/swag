#include "pch.h"
#include "Workspace.h"
#include "Global.h"
#include "ThreadManager.h"
#include "CommandLine.h"
#include "Global.h"
#include "Pool.h"
#include "Module.h"
#include "Stats.h"
#include "PoolFactory.h"

Module* Workspace::createOrUseModule(const fs::path& path)
{
    for (const auto& module : m_modules)
    {
        if (module->m_path == path)
            return module;
    }

    auto module = new Module(path);
    m_modules.push_back(module);
	if (g_CommandLine.stats)
		g_Stats.numModules++;
    return module;
}

void Workspace::enumerateFilesInModule(const fs::path& path)
{
    // Create a module
    auto module = createOrUseModule(path);

    // Scan source folder
    WIN32_FIND_DATAA file;
    vector<string>   directories;

    directories.push_back(path.string());
    string     tmp, tmp1;
    const bool filterIsEmpty = g_CommandLine.fileFilter.empty();
    while (directories.size())
    {
        tmp = move(directories.back());
        directories.pop_back();

        tmp1     = tmp + "\\*";
        HANDLE h = ::FindFirstFileA(tmp1.c_str(), &file);
        if (h == INVALID_HANDLE_VALUE)
            continue;

        do
        {
            tmp1 = tmp + "\\" + file.cFileName;
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((file.cFileName[0] == '.') && (!file.cFileName[1] || (file.cFileName[1] == '.' && !file.cFileName[2])))
                    continue;
                directories.emplace_back(move(tmp1));
            }
            else
            {
                auto pz = strrchr(file.cFileName, '.');
#ifdef SWAG_TEST_CPP
                if (pz && !_strcmpi(pz, ".cpp"))
#else
                if (pz && !_strcmpi(pz, ".swg"))
#endif
                {
                    // File filtering by name
                    if (filterIsEmpty || strstr(tmp1.c_str(), g_CommandLine.fileFilter.c_str()))
                    {
                        auto job  = g_Pool.m_syntaxJob.alloc();
                        auto file = g_Pool.m_sourceFile.alloc();

                        job->setFile(file);
                        file->m_module = module;
                        file->m_path   = move(tmp1);

                        g_ThreadMgr.addJob(job);
                    }
                }
            }
        } while (::FindNextFileA(h, &file));

        ::FindClose(h);
    }
}

void Workspace::enumerateModules()
{
#ifdef SWAG_TEST_CPP
    enumerateFilesInModule("c:\\boulot\\sdb\\");
#else
    enumerateFilesInModule("c:\\boulot\\swag\\unittest");
#endif
}

bool Workspace::build()
{
    g_ThreadMgr.init();

    // Ask for a syntax pass on all files of all modules
    enumerateModules();
    g_ThreadMgr.waitEndJobs();

    return true;
}