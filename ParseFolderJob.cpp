#include "pch.h"
#include "ParseFolderJob.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "Global.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "CommandLine.h"

ParseFolderJob::ParseFolderJob(const fs::path& path)
    : m_path{path}
{
}

bool ParseFolderJob::execute()
{
    for (auto& p : fs::recursive_directory_iterator(m_path))
    {
        if (p.path().extension() == ".swg")
        {
			// File filtering by name
			if (!g_CommandLine.fileFilter.empty() && p.path().string().find(g_CommandLine.fileFilter) == string::npos)
				continue;

            auto job  = g_Pool.m_readFileJob.alloc();
            auto file = g_Pool.m_sourceFile.alloc();

            job->setFile(file);
            file->m_path = p.path();

            g_ThreadMgr.addJob(job);
        }
    }

	return true;
}
