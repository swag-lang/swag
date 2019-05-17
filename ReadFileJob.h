#pragma once
#include "ThreadManager.h"
#include "Tokenizer.h"

class ReadFileJob : public Job
{
public:
	ReadFileJob(class SourceFile* file);
	void execute() override;

	class SourceFile* m_file = nullptr;
	Tokenizer m_tokenizer;
	Token m_token;
};

