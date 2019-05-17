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
    while (true)
    {
        if (!m_tokenizer.getToken(m_token))
            return;
        if (m_token.id == TokenId::EndOfFile)
            break;

        // Top level
        switch (m_token.id)
        {
        case TokenId::CompilerPass:
            break;
        }
    }
}
