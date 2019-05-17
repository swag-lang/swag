#include "pch.h"
#include "LanguageSpec.h"


LanguageSpec::LanguageSpec()
{
	m_keywords["#pass"] = TokenId::CompilerPass;
}
