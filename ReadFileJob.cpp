#include "pch.h"
#include "ReadFileJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Global.h"
#include "Stats.h"

ReadFileJob::ReadFileJob(SourceFile* file) : m_file{file}
{
}

void ReadFileJob::execute()
{
	g_Stats.numFiles++;

	Tokenizer tokenizer;
	tokenizer.setFile(m_file);

	Token token;
	while(true)
	{
		auto result = tokenizer.getToken(token);
		if (result != TokenizerResult::Pending)
			break;
	}
}
