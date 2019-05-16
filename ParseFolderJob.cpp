#include "pch.h"
#include "ParseFolderJob.h"
#include "SourceFile.h"
#include "ReadFileJob.h"
#include "Global.h"

ParseFolderJob::ParseFolderJob(const fs::path& path) : m_path{path}
{
}

void ParseFolderJob::execute()
{
	for (auto& p : fs::recursive_directory_iterator(m_path))
	{
		if (p.path().extension() == ".cpp")
		{
			auto job = new ReadFileJob(new SourceFile(p.path()));
			g_ThreadMgr.addJob(job);
		}
	}
}
