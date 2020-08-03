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

    auto it = g_LangSpec.keywords.find(token.text);
    if (it != g_LangSpec.keywords.end())
        token.id = it->second;
    else
        token.id = TokenId::Identifier;

    // Special keywords, replace with literal, except in 'docMode'
    switch (token.id)
    {
    case TokenId::KwdTrue:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType    = g_TypeMgr.typeInfoBool;
        token.literalValue.b = true;
        return;
    case TokenId::KwdFalse:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType    = g_TypeMgr.typeInfoBool;
        token.literalValue.b = false;
        return;
    case TokenId::KwdNull:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType          = g_TypeMgr.typeInfoNull;
        token.literalValue.pointer = nullptr;
        return;
    case TokenId::CompilerFile:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralString;
        token.literalType = g_TypeMgr.typeInfoString;
        token.text        = sourceFile->path;
        return;
    case TokenId::CompilerLine:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoUntypedU64;
        token.literalValue.u64 = location.line + 1;
        return;
    case TokenId::CompilerBuildVersion:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_VERSION;
        return;
    case TokenId::CompilerBuildRevision:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_REVISION;
        return;
    case TokenId::CompilerBuildNum:
        if (!(parseFlags & TOKENIZER_NO_LITERAL_CONVERSION))
            token.id = TokenId::LiteralNumber;
        token.literalType      = g_TypeMgr.typeInfoS32;
        token.literalValue.s32 = SWAG_BUILD_NUM;
        return;
    case TokenId::NativeType:
        token.literalType = g_LangSpec.nativeTypes[token.text];
        return;
    }
}
