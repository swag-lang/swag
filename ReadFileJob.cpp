#include "pch.h"
#include "ReadFileJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"

atomic<int> cptToParse;

ReadFileJob::ReadFileJob(SourceFile* file) : m_file{file}
{
}

void ReadFileJob::execute()
{
	Tokenizer tokenizer;
	tokenizer.setFile(m_file);

	Token token;
	while (tokenizer.getToken(token))
	{
	}

	cptToParse++;
}
