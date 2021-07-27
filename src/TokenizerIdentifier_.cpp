#include "pch.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "SourceFile.h"
#include "Version.h"
#include "Timer.h"

void Tokenizer::relaxIdentifier(Token& token)
{
    switch (token.id)
    {
    case TokenId::KwdAnd:
    case TokenId::KwdOr:
    case TokenId::KwdOrElse:
        token.id = TokenId::Identifier;
        return;
    }
}

void Tokenizer::doIdentifier(Token& token, uint32_t c, unsigned offset)
{
    while (SWAG_IS_ALPHA(c) || SWAG_IS_DIGIT(c) || c == '_')
    {
        treatChar(c, offset);
        c = getCharNoSeek(offset);
    }

    appendTokenName(token);

    auto crc = token.text.hash();
    auto it  = g_LangSpec.keywords.find(token.text.buffer, token.text.count, crc);
    if (it)
        token.id = (*it).first;
    else
        token.id = TokenId::Identifier;

    // Special keywords, replace with literal, except in 'docMode'
    switch (token.id)
    {
    case TokenId::NativeType:
        token.literalType = (*it).second;
        return;
    case TokenId::KwdTrue:
        token.id             = TokenId::LiteralNumber;
        token.literalType    = LiteralType::TT_BOOL;
        token.literalValue.b = true;
        return;
    case TokenId::KwdFalse:
        token.id             = TokenId::LiteralNumber;
        token.literalType    = LiteralType::TT_BOOL;
        token.literalValue.b = false;
        return;
    case TokenId::KwdNull:
        token.id                   = TokenId::LiteralNumber;
        token.literalType          = LiteralType::TT_NULL;
        token.literalValue.pointer = nullptr;
        return;
    case TokenId::CompilerFile:
        token.id          = TokenId::LiteralString;
        token.literalType = LiteralType::TT_STRING;
        token.text        = sourceFile->path;
        return;
    case TokenId::CompilerLine:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = LiteralType::TT_UNTYPED_INT;
        token.literalValue.u32 = location.line + 1;
        return;
    case TokenId::CompilerBuildVersion:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = LiteralType::TT_S32;
        token.literalValue.s32 = SWAG_BUILD_VERSION;
        return;
    case TokenId::CompilerBuildRevision:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = LiteralType::TT_S32;
        token.literalValue.s32 = SWAG_BUILD_REVISION;
        return;
    case TokenId::CompilerBuildNum:
        token.id               = TokenId::LiteralNumber;
        token.literalType      = LiteralType::TT_S32;
        token.literalValue.s32 = SWAG_BUILD_NUM;
        return;
    case TokenId::CompilerSwagOs:
        token.id          = TokenId::LiteralNumber;
        token.literalType = LiteralType::TT_STRING;
#ifdef _WIN32
        token.text = "windows";
#else
        token.text = "?";
#endif
        return;
    }
}
