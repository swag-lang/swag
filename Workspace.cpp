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
#ifdef SWAG_TEST_CPP
                if (pz && !_strcmpi(pz, ".cpp"))
#else
                if (pz && !_strcmpi(pz, ".swg"))
#endif
                {
                    // File filtering by name
                    if (filterIsEmpty || strstr(tmp1.c_str(), g_CommandLine.fileFilter.c_str()))
                    {
                        auto job  = g_Pool.syntaxJob.alloc();
                        auto file = g_Pool.sourceFile.alloc();

                        job->setFile(file);
                        module->addFile(file);
                        file->m_path = move(tmp1);

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
	if (g_Workspace.numErrors > 0)
		return false;
	if(g_CommandLine.syntaxOnly)
		return true;

	// Build each module
	for (auto module : m_modules)
	{
        auto job = g_Pool.moduleJob.alloc();
		job->module = module;
		g_ThreadMgr.addJob(job);
	}

	g_ThreadMgr.waitEndJobs();

    return true;
}