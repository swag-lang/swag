#include "pch.h"
#include "ParseFolderJob.h"
#include "SourceFile.h"
#include "ReadFileJob.h"
#include "Global.h"
#include "JobThread.h"
#include "ThreadManager.h"

ParseFolderJob::ParseFolderJob(const fs::path& path)
    : m_path{path}
{
}

void ParseFolderJob::execute()
{
    for (auto& p : fs::recursive_directory_iterator(m_path))
    {
        if (p.path().extension() == ".swg" || p.path().extension() == ".cpp")
        {
            auto job  = g_Pool.m_readFileJob.alloc();
            auto file = g_Pool.m_sourceFile.alloc();

            job->setFile(file);
            file->setPath(p.path());

            g_ThreadMgr.addJob(job);
        }
    }
}
