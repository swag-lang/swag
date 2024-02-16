#include "pch.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Log.h"
#include "ModuleBuildJob.h"
#include "Os.h"
#include "Report.h"
#include "Semantic.h"
#include "TypeManager.h"
#include "Workspace.h"

namespace
{
	void helpUserTags()
	{
		g_Log.eol();
		g_Log.print("Syntax is:\n");
		g_Log.eol();
		g_Log.print("--tag:\"<TagName>\"\n");
		g_Log.print("--tag:\"<TagName> = <literal>\"\n");
		g_Log.print("--tag:\"<TagName>: <type> = <literal>\"\n");
		g_Log.eol();
		g_Log.print("Examples:\n");
		g_Log.eol();
		g_Log.print("--tag:\"Swag.DebugAllocator.captureAllocStack\"\n");
		g_Log.print("--tag:\"Swag.DebugAllocator.captureAllocStack = false\"\n");
		g_Log.print("--tag:\"Swag.DebugAllocator.captureAllocStack: bool = true\"\n");
		g_Log.print("--tag:\"Swag.ScratchAllocator.capacity: u64 = 4000_0000\"\n");
		g_Log.eol();
		g_Log.print("");
	}
}

void Workspace::setupUserTags()
{
	// Command line tags
	// Format is --tag:"TagName : type = value"
	for (auto& tag : g_CommandLine.tags)
	{
		OneTag oneTag;
		oneTag.cmdLine = tag;

		Utf8 oneTagName = tag;
		oneTagName.trim();

		Vector<Utf8> tokens;
		Utf8::tokenize(oneTagName, '=', tokens);

		if (tokens.size() == 2)
		{
			tokens[0].trim();
			tokens[1].trim();

			// Get the type
			auto         literalType = LiteralType::TT_MAX;
			Vector<Utf8> tokens1;
			Utf8::tokenize(tokens[0], ':', tokens1);
			if (tokens1.size() == 2)
			{
				tokens1[0].trim();
				tokens1[1].trim();
				auto it = g_LangSpec->keywords.find(tokens1[1]);
				if (!it || *it != TokenId::NativeType)
				{
					Report::error(FMT(Err(Fat0033), tokens1[1].c_str(), oneTagName.c_str()));
					helpUserTags();
					OS::exit(-1);
				}

				auto it1 = g_LangSpec->nativeTypes.find(tokens1[1]);
				SWAG_ASSERT(it1);
				literalType = *it1;
			}

			// Get value
			// Use the tokenizer
			auto tokenVal = tokens[1];
			tokenVal.trim();

			bool defaultType = false;
			oneTag.type      = nullptr;
			if (literalType != LiteralType::TT_MAX)
				oneTag.type = TypeManager::literalTypeToType(literalType);
			else
			{
				defaultType = true;
				oneTag.type = g_TypeMgr->typeInfoS32;
			}

			// If type is already specified as string, just take the value part without any conversion
			if (oneTag.type && oneTag.type->isString())
			{
				oneTag.value.text = tokenVal;
			}
			else
			{
				ErrorContext errorContext;
				SourceFile   fakeFile;
				Tokenizer    tokenizer;
				TokenParse   token;
				fakeFile.setExternalBuffer(tokenVal);
				tokenizer.setup(&errorContext, &fakeFile);

				bool neg = false;
				tokenizer.nextToken(token);

				if (token.id == TokenId::KwdTrue)
				{
					if (defaultType)
						oneTag.type = g_TypeMgr->typeInfoBool;
					token.literalValue.b = true;
				}
				else if (token.id == TokenId::KwdFalse)
				{
					if (defaultType)
						oneTag.type = g_TypeMgr->typeInfoBool;
					token.literalValue.b = false;
				}
				else
				{
					if (token.id == TokenId::SymMinus)
					{
						neg = true;
						tokenizer.nextToken(token);
					}
					else if (token.id == TokenId::SymPlus)
					{
						tokenizer.nextToken(token);
					}

					if (token.id != TokenId::LiteralNumber && token.id != TokenId::LiteralString)
					{
						Report::error(FMT(Err(Fat0032), tokenVal.c_str(), oneTagName.c_str()));
						helpUserTags();
						OS::exit(-1);
					}
				}

				// Check type and value
				if (literalType == LiteralType::TT_MAX)
					literalType = token.literalType;
				oneTag.value.reg  = token.literalValue;
				oneTag.value.text = token.text;

				auto errMsg = Semantic::checkLiteralValue(oneTag.value, literalType, token.literalValue, oneTag.type, neg);
				if (!errMsg.empty())
				{
					auto err = FMT(Err(Fat0024), oneTagName.c_str(), errMsg.c_str());
					Report::error(err);
					helpUserTags();
					OS::exit(-1);
				}
			}

			tokens1[0].trim();
			oneTag.name = tokens1[0];
			tags.push_back(oneTag);
		}
		else
		{
			oneTag.type        = g_TypeMgr->typeInfoBool;
			oneTag.value.reg.b = true;
			oneTag.name        = oneTagName;
			tags.push_back(oneTag);
		}
	}
}

void Workspace::setupInternalTags()
{
	OneTag oneTag;

	// Swag.Endian = "little" or "big" depending on the architecture
	switch (g_CommandLine.target.arch)
	{
	case SwagTargetArch::X86_64:
		oneTag.type = g_TypeMgr->typeInfoString;
		oneTag.value.text = "little";
		oneTag.name       = "Swag.Endian";
		tags.push_back(oneTag);
		break;
	default:
		SWAG_ASSERT(false);
		break;
	}
}

OneTag* Workspace::hasTag(const Utf8& name)
{
	for (auto& tag : tags)
	{
		if (tag.name == name)
			return &tag;
	}

	return nullptr;
}
