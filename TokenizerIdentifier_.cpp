#include "pch.h"
#include "Global.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "SourceFile.h"
#include "Version.h"

void Tokenizer::getIdentifier(Token& token, char32_t c, unsigned offset)
{
    while (SWAG_IS_ALPHA(c) || SWAG_IS_DIGIT(c) || c == '_')
    {
        token.text += c;
        treatChar(c, offset);
        c = getCharNoSeek(offset);
    }

    if (token.text[0] == '@')
    {
        token.id = TokenId::Intrinsic;
    }
    else
    {
        auto it = g_LangSpec.keywords.find(token.text);
        if (it != g_LangSpec.keywords.end())
            token.id = it->second;
        else
            token.id = TokenId::Identifier;
    }

    // Special keywords
    switch (token.id)
    {
    case TokenId::KwdTrue:
        token.id             = TokenId::LiteralNumber;
        token.literalType    = g_TypeMgr.typeInfoBool;
        token.literalValue.b = true;
        return;
    case TokenId::KwdFalse:
        token.id             = TokenId::LiteralNumber;
        token.literalType    = g_TypeMgr.typeInfoBool;
        token.literalValue.b = false;
        return;
    case TokenId::KwdNull:
        token.id                   = TokenId::LiteralNumber;
        token.literalType          = g_TypeMgr.typeInfoNull;
        token.literalValue.pointer = nullptr;
        return;
    case TokenId::CompilerFile:
        token.id          = TokenId::LiteralString;
        token.literalType = g_TypeMgr.typeInfoString;
        token.text        = sourceFile->path.string();
        return;
    case TokenId::CompilerLine:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoUntypedU64;
        token.literalValue.u64 = location.line + 1;
        return;
    case TokenId::CompilerBuildVersion:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_VERSION;
        return;
    case TokenId::CompilerBuildRevision:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_REVISION;
        return;
    case TokenId::CompilerBuildNum:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_NUM;
        return;
    case TokenId::NativeType:
        token.literalType = g_LangSpec.nativeTypes[token.text];
        return;
    }
}
