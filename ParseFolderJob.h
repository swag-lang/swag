#pragma once
#include "ThreadManager.h"

class ParseFolderJob : public Job
{
public:
	ParseFolderJob(const fs::path& path);
	void execute() override;
	fs::path m_path;
};

