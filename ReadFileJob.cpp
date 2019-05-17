#include "pch.h"
#include "ReadFileJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Global.h"
#include "Stats.h"

void ReadFileJob::execute()
{
	g_Stats.numFiles++;
	m_tokenizer.setFile(m_file);

	bool canLex = true;
	while(true)
	{
		auto result = m_tokenizer.getToken(m_token);
		if (result != TokenizerResult::Pending)
			break;

		// Top level
		switch (m_token.id)
		{
		case TokenId::CompilerPass:
			break;
		}
	}
}
