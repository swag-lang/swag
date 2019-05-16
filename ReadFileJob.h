#pragma once
#include "ThreadManager.h"

class ReadFileJob : public Job
{
public:
	ReadFileJob(class SourceFile* file);
	void execute() override;
	class SourceFile* m_file = nullptr;
};

