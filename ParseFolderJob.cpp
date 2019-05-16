#include "pch.h"
#include "ParseFolderJob.h"
#include "SourceFile.h"
#include "ReadFileJob.h"

ParseFolderJob::ParseFolderJob(const fs::path& path) : m_path{path}
{
}

void ParseFolderJob::execute()
{
	for (auto& p : fs::recursive_directory_iterator(m_path))
	{
		if (p.path().extension() == ".cs")
		{
			auto job = new ReadFileJob(new SourceFile(p.path()));
			ThreadManager::m_instance->addJob(job);
		}
	}
}
