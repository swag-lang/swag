#pragma once
#include "Job.h"

class ParseFolderJob : public Job
{
public:
	ParseFolderJob(const fs::path& path);
	void execute() override;
	void reset() override {}
	void construct() override {}
	fs::path m_path;
};

