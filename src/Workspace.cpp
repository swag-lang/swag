#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ModuleBuildJob.h"
#include "Module.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "ErrorIds.h"
#include "Report.h"

Workspace* g_Workspace = nullptr;

static void helpUserTags()
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

        vector<Utf8> tokens;
        Utf8::tokenize(oneTagName, '=', tokens);

        if (tokens.size() == 2)
        {
            tokens[0].trim();
            tokens[1].trim();

            // Get the type
            LiteralType  literalType = LiteralType::TT_MAX;
            vector<Utf8> tokens1;
            Utf8::tokenize(tokens[0], ':', tokens1);
            if (tokens1.size() == 2)
            {
                tokens1[0].trim();
                tokens1[1].trim();
                auto it = g_LangSpec->keywords.find(tokens1[1]);
                if (!it || *it != TokenId::NativeType)
                {
                    Report::error(Fmt(Err(Fat0024), tokens1[1].c_str(), oneTagName.c_str()));
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
                SourceFile fakeFile;
                Tokenizer  tokenizer;
                TokenParse token;
                fakeFile.setExternalBuffer(tokenVal.buffer, (uint32_t) tokenVal.count);
                tokenizer.setFile(&fakeFile);

                bool neg = false;
                tokenizer.getToken(token);

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
                        tokenizer.getToken(token);
                    }
                    else if (token.id == TokenId::SymPlus)
                    {
                        tokenizer.getToken(token);
                    }

                    if (token.id != TokenId::LiteralNumber && token.id != TokenId::LiteralString)
                    {
                        Report::error(Fmt(Err(Fat0023), tokenVal.c_str(), oneTagName.c_str()));
                        helpUserTags();
                        OS::exit(-1);
                    }
                }

                // Check type and value
                if (literalType == LiteralType::TT_MAX)
                    literalType = token.literalType;
                oneTag.value.reg  = token.literalValue;
                oneTag.value.text = token.text;

                auto errMsg = SemanticJob::checkLiteralValue(oneTag.value, token, oneTag.type, neg);
                if (!errMsg.empty())
                {
                    auto err = Fmt(Err(Fat0015), oneTagName.c_str(), errMsg.c_str());
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

void Workspace::setup()
{
    g_TypeMgr = g_Allocator.alloc<TypeManager>();
    g_TypeMgr->setup();

    setupPaths();
    setupInternalTags();
    setupUserTags();

    if (workspacePath.empty())
    {
        Report::error(Err(Fat0011));
        OS::exit(-1);
    }

    bool invalid = false;
    if (!fs::exists(workspacePath))
    {
        Report::error(Fmt(Err(Fat0026), workspacePath.string().c_str()));
        invalid = true;
    }
    else if (!g_CommandLine.scriptCommand && !fs::exists(modulesPath) && !fs::exists(testsPath))
    {
        Report::error(Fmt(Err(Fat0016), workspacePath.string().c_str()));
        invalid = true;
    }

    if (invalid)
    {
        g_Log.message(Nte(Nte0012));
        OS::exit(-1);
    }

    g_ThreadMgr.init();
}

void Workspace::setupPaths()
{
    workspacePath = fs::absolute(g_CommandLine.workspacePath);
    if (workspacePath.empty())
        workspacePath = fs::current_path();

    testsPath = workspacePath;
    testsPath.append(SWAG_TESTS_FOLDER);
    testsPath += "/";

    examplesPath = workspacePath;
    examplesPath.append(SWAG_EXAMPLES_FOLDER);
    examplesPath += "/";

    modulesPath = workspacePath;
    modulesPath.append(SWAG_MODULES_FOLDER);
    modulesPath += "/";

    dependenciesPath = workspacePath;
    dependenciesPath.append(SWAG_DEPENDENCIES_FOLDER);
    dependenciesPath += "/";
}

void Workspace::setupCachePath()
{
    // Cache directory
    cachePath = g_CommandLine.cachePath;
    if (cachePath.empty())
    {
        cachePath = OS::getTemporaryFolder();
        if (cachePath.empty())
        {
            cachePath = workspacePath;
            cachePath += SWAG_OUTPUT_FOLDER;
        }
    }
}