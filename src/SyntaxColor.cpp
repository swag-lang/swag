#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Log.h"
#include "LanguageSpec.h"
#include "SyntaxColor.h"

static Utf8 getColor(SyntaxColorMode mode, SyntaxColor color)
{
    switch (mode)
    {
    case SyntaxColorMode::ForLog:
        switch (color)
        {
        case SyntaxColor::SyntaxCode:
            return "\x1b[97m";
        case SyntaxColor::SyntaxComment:
            return "\x1b[38;2;106;153;85m";
        case SyntaxColor::SyntaxCompiler:
            return "\x1b[38;2;255;116;17m";
        case SyntaxColor::SyntaxFunction:
            return "\x1b[38;2;255;116;17m";
        case SyntaxColor::SyntaxConstant:
            return "\x1b[38;2;78;201;176m";
        case SyntaxColor::SyntaxIntrinsic:
            return "\x1b[38;2;220;220;170m";
        case SyntaxColor::SyntaxType:
            return "\x1b[38;2;246;204;134m";
        case SyntaxColor::SyntaxKeyword:
            return "\x1b[38;2;86;156;214m";
        case SyntaxColor::SyntaxLogic:
            return "\x1b[38;2;216;160;223m";
        case SyntaxColor::SyntaxNumber:
            return "\x1b[38;2;181;206;168m";
        case SyntaxColor::SyntaxString:
            return "\x1b[38;2;206;145;120m";
        }
        break;

    case SyntaxColorMode::ForDoc:
    {
        const char* colorName = nullptr;
        switch (color)
        {
        case SyntaxColor::SyntaxCode:
            colorName = "SyntaxCode";
            break;
        case SyntaxColor::SyntaxComment:
            colorName = "SyntaxComment";
            break;
        case SyntaxColor::SyntaxCompiler:
            colorName = "SyntaxCompiler";
            break;
        case SyntaxColor::SyntaxFunction:
            colorName = "SyntaxFunction";
            break;
        case SyntaxColor::SyntaxConstant:
            colorName = "SyntaxConstant";
            break;
        case SyntaxColor::SyntaxIntrinsic:
            colorName = "SyntaxIntrinsic";
            break;
        case SyntaxColor::SyntaxType:
            colorName = "SyntaxType";
            break;
        case SyntaxColor::SyntaxKeyword:
            colorName = "SyntaxKeyword";
            break;
        case SyntaxColor::SyntaxLogic:
            colorName = "SyntaxLogic";
            break;
        case SyntaxColor::SyntaxNumber:
            colorName = "SyntaxNumber";
            break;
        case SyntaxColor::SyntaxString:
            colorName = "SyntaxString";
            break;
        }

        if (colorName)
            return Fmt("</span><span class=\"%s\">", colorName);
        break;
    }
    }

    SWAG_ASSERT(false);
    return "";
}

Utf8 syntaxColor(const Utf8& line, SyntaxColorMode mode)
{
    const char* pz = line.c_str();
    uint32_t    c, offset;
    Utf8        result;

    pz = Utf8::decodeUtf8(pz, c, offset);
    while (c)
    {
        Utf8 identifier;

        // Raw string
        if (c == '@' && *pz == '"')
        {
            result += getColor(mode, SyntaxColor::SyntaxString);
            result += c;
            while (*pz && (pz[0] != '"' || pz[1] != '@'))
                result += *pz++;

            if (*pz)
            {
                result += *pz++;
                result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // String
        if (c == '"')
        {
            result += getColor(mode, SyntaxColor::SyntaxString);
            result += c;
            while (*pz && *pz != '"')
            {
                if (*pz == '\\')
                    result += *pz++;
                if (*pz)
                    result += *pz++;
            }

            if (*pz == '"')
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Character
        if (c == '`')
        {
            result += getColor(mode, SyntaxColor::SyntaxString);
            result += c;
            while (*pz && *pz != '`')
            {
                if (*pz == '\\')
                    result += *pz++;
                if (*pz)
                    result += *pz++;
            }

            if (*pz == '`')
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Line comment
        if (c == '/' && pz[0] == '/')
        {
            result += getColor(mode, SyntaxColor::SyntaxComment);
            result += c;
            while (*pz && !SWAG_IS_EOL(*pz))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Binary literal
        if (c == '0' && (*pz == 'b' || *pz == 'B'))
        {
            result += getColor(mode, SyntaxColor::SyntaxNumber);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Hexadecimal literal
        if (c == '0' && (*pz == 'x' || *pz == 'X'))
        {
            result += getColor(mode, SyntaxColor::SyntaxNumber);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Number
        if (SWAG_IS_DIGIT(c))
        {
            result += getColor(mode, SyntaxColor::SyntaxNumber);
            result += c;

            while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                result += *pz++;

            if (*pz == '.')
            {
                result += *pz++;
                while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                    result += *pz++;
            }

            if (*pz == 'e' || *pz == 'E')
            {
                result += *pz++;
                if (*pz == '-' || *pz == '+')
                    result += *pz++;
                while (*pz && (SWAG_IS_DIGIT(*pz) || *pz == '_'))
                    result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxCode);
            continue;
        }

        // Word
        if (SWAG_IS_ALPHA(c) || c == '_' || c == '#' || c == '@')
        {
            identifier += c;
            pz = Utf8::decodeUtf8(pz, c, offset);
            while (SWAG_IS_ALPHA(c) || c == '_' || SWAG_IS_DIGIT(c))
            {
                identifier += c;
                pz = Utf8::decodeUtf8(pz, c, offset);
            }

            auto it = g_LangSpec->keywords.find(identifier);
            if (it)
            {
                switch (*it)
                {
                case TokenId::KwdUsing:
                case TokenId::KwdWith:
                case TokenId::KwdCast:
                case TokenId::KwdAutoCast:
                case TokenId::KwdDeRef:
                case TokenId::KwdRef:
                case TokenId::KwdMoveRef:
                case TokenId::KwdRetVal:
                case TokenId::KwdTry:
                case TokenId::KwdCatch:
                case TokenId::KwdTryCatch:
                case TokenId::KwdAssume:
                case TokenId::KwdThrow:
                case TokenId::KwdDiscard:

                case TokenId::KwdVar:
                case TokenId::KwdLet:
                case TokenId::KwdConst:
                case TokenId::KwdUndefined:

                case TokenId::KwdEnum:
                case TokenId::KwdStruct:
                case TokenId::KwdUnion:
                case TokenId::KwdImpl:
                case TokenId::KwdInterface:
                case TokenId::KwdFunc:
                case TokenId::KwdClosure:
                case TokenId::KwdMethod:
                case TokenId::KwdNamespace:
                case TokenId::KwdTypeAlias:
                case TokenId::KwdNameAlias:
                case TokenId::KwdAttr:

                case TokenId::KwdTrue:
                case TokenId::KwdFalse:
                case TokenId::KwdNull:

                case TokenId::KwdPublic:
                case TokenId::KwdInternal:
                case TokenId::KwdPrivate:
                    result += getColor(mode, SyntaxColor::SyntaxKeyword);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                    break;

                case TokenId::NativeType:
                case TokenId::CompilerType:
                    result += getColor(mode, SyntaxColor::SyntaxType);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                    break;

                case TokenId::KwdIf:
                case TokenId::KwdElse:
                case TokenId::KwdElif:
                case TokenId::KwdFor:
                case TokenId::KwdWhile:
                case TokenId::KwdSwitch:
                case TokenId::KwdDefer:
                case TokenId::KwdLoop:
                case TokenId::KwdVisit:
                case TokenId::KwdBreak:
                case TokenId::KwdFallThrough:
                case TokenId::KwdUnreachable:
                case TokenId::KwdReturn:
                case TokenId::KwdCase:
                case TokenId::KwdContinue:
                case TokenId::KwdDefault:
                case TokenId::KwdAnd:
                case TokenId::KwdOr:
                case TokenId::KwdOrElse:
                    result += getColor(mode, SyntaxColor::SyntaxLogic);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                    break;

                case TokenId::CompilerFuncCompiler:
                case TokenId::CompilerFuncDrop:
                case TokenId::CompilerFuncInit:
                case TokenId::CompilerFuncMain:
                case TokenId::CompilerFuncPreMain:
                case TokenId::CompilerFuncTest:
                    result += getColor(mode, SyntaxColor::SyntaxCompiler);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                    break;

                default:
                    if (identifier[0] == '@')
                    {
                        result += getColor(mode, SyntaxColor::SyntaxIntrinsic);
                        result += identifier;
                        result += getColor(mode, SyntaxColor::SyntaxCode);
                    }
                    else
                    {
                        result += identifier;
                    }

                    break;
                }
            }
            else
            {
                if (identifier[0] >= 'a' and identifier[0] <= 'z' && (c == '(' || c == '\''))
                {
                    result += getColor(mode, SyntaxColor::SyntaxFunction);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                }
                else if (identifier[0] >= 'A' and identifier[0] <= 'Z')
                {
                    result += getColor(mode, SyntaxColor::SyntaxConstant);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxCode);
                }
                else
                {
                    result += identifier;
                }
            }

            continue;
        }

        result += c;
        pz = Utf8::decodeUtf8(pz, c, offset);
    }

    return result;
}
