#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ModuleBuildJob.h"
#include "Module.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "ErrorIds.h"

Workspace g_Workspace;

void Workspace::setupPaths()
{
    workspacePath = fs::absolute(g_CommandLine.workspacePath);

    testsPath = workspacePath;
    testsPath.append(SWAG_TESTS_FOLDER);
    testsPath.append("/");

    examplesPath = workspacePath;
    examplesPath.append(SWAG_EXAMPLES_FOLDER);
    examplesPath.append("/");

    modulesPath = workspacePath;
    modulesPath.append(SWAG_MODULES_FOLDER);
    modulesPath.append("/");

    dependenciesPath = workspacePath;
    dependenciesPath.append(SWAG_DEPENDENCIES_FOLDER);
    dependenciesPath.append("/");
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
                auto it = g_LangSpec->nativeTypes.find(tokens1[1]);
                if (!it)
                {
                    g_Log.error(Utf8::format(Msg0539, tokens1[0].c_str(), tokens1[1].c_str()));
                    OS::exit(-1);
                }

                literalType = *it;
            }

            // Get value
            // Use the tokenizer
            auto tokenVal = tokens[1];
            tokenVal.trim();

            oneTag.type = nullptr;
            if (literalType != LiteralType::TT_MAX)
                oneTag.type = TypeManager::literalTypeToType(literalType);

            // If type is already specified as string, just take the value part without any conversion
            if (oneTag.type && oneTag.type->isNative(NativeTypeKind::String))
            {
                oneTag.value.text = tokenVal;
            }
            else
            {
                SourceFile fakeFile;
                Tokenizer  tokenizer;
                Token      token;
                fakeFile.setExternalBuffer(tokenVal.buffer, (uint32_t) tokenVal.count);
                tokenizer.setFile(&fakeFile);

                bool neg = false;
                tokenizer.getToken(token);
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
                    g_Log.error(Utf8::format(Msg0538, tokenVal.c_str(), tokens1[0].c_str()));
                    OS::exit(-1);
                }

                // Check type and value
                if (literalType == LiteralType::TT_MAX)
                    literalType = token.literalType;
                oneTag.value.reg  = token.literalValue;
                oneTag.value.text = token.text;

                auto errMsg = SemanticJob::checkLiteralType(oneTag.value, token, oneTag.type, neg);
                if (!errMsg.empty())
                {
                    auto err = Utf8::format(Msg0322, tokens1[0].c_str(), errMsg.c_str());
                    g_Log.error(err);
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
    setupPaths();
    setupInternalTags();
    setupUserTags();

    if (workspacePath.empty())
    {
        g_Log.error(Msg0540);
        OS::exit(-1);
    }

    bool invalid = false;
    if (!fs::exists(workspacePath))
    {
        g_Log.error(Utf8::format(Msg0541, workspacePath.string().c_str()));
        invalid = true;
    }
    else if (!g_CommandLine.scriptCommand && !fs::exists(modulesPath) && !fs::exists(testsPath))
    {
        g_Log.error(Utf8::format(Msg0542, workspacePath.string().c_str()));
        invalid = true;
    }

    if (invalid)
    {
        g_Log.message(Note012);
        OS::exit(-1);
    }

    g_ThreadMgr.init();
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
            cachePath += "/";
        }
    }
}