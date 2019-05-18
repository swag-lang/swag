#include "pch.h"
#include "LanguageSpec.h"


LanguageSpec::LanguageSpec()
{
	m_keywords[L"#pass"] = TokenId::CompilerPass;
}
