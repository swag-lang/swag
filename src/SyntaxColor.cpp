#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Log.h"
#include "LanguageSpec.h"
#include "SyntaxColor.h"

struct RGBColor
{
    unsigned char r, g, b;
};

static void rgbToHsl(RGBColor color, float* h, float* s, float* l)
{
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;

    float maxVal = fmaxf(fmaxf(r, g), b);
    float minVal = fminf(fminf(r, g), b);

    *l = (maxVal + minVal) / 2;

    if (maxVal == minVal)
    {
        *h = *s = 0;
    }
    else
    {
        float d = maxVal - minVal;
        *s      = (*l > 0.5) ? d / (2 - maxVal - minVal) : d / (maxVal + minVal);

        if (maxVal == r)
        {
            *h = (g - b) / d + ((g < b) ? 6 : 0);
        }
        else if (maxVal == g)
        {
            *h = (b - r) / d + 2;
        }
        else if (maxVal == b)
        {
            *h = (r - g) / d + 4;
        }
        *h /= 6;
    }
}

static float hueToRgb(float p, float q, float t)
{
    if (t < 0)
        t += 1;
    if (t > 1)
        t -= 1;
    if (t < 1.0f / 6)
        return p + (q - p) * 6 * t;
    if (t < 1.0f / 2)
        return q;
    if (t < 2.0f / 3)
        return p + (q - p) * (2.0f / 3 - t) * 6;
    return p;
}

static RGBColor hslToRgb(float h, float s, float l)
{
    float r, g, b;

    if (s == 0)
    {
        r = g = b = l;
    }
    else
    {
        float q = (l < 0.5) ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;

        r = hueToRgb(p, q, h + 1.0f / 3);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1.0f / 3);
    }

    RGBColor result;
    result.r = (unsigned char) (r * 255.0f);
    result.g = (unsigned char) (g * 255.0f);
    result.b = (unsigned char) (b * 255.0f);
    return result;
}

uint32_t getSyntaxColor(SyntaxColor color, float lum)
{
    RGBColor rgb;
    switch (color)
    {
    case SyntaxColor::SyntaxCode:
        rgb = {0xCC, 0xCC, 0xCC};
        break;
    case SyntaxColor::SyntaxComment:
        rgb = {0x6A, 0x99, 0x55};
        break;
    case SyntaxColor::SyntaxCompiler:
        rgb = {0xAA, 0xAA, 0xAA};
        break;
    case SyntaxColor::SyntaxFunction:
        rgb = {0xFF, 0x74, 0x11};
        break;
    case SyntaxColor::SyntaxConstant:
        rgb = {0x56, 0x8c, 0xd6};
        break;
    case SyntaxColor::SyntaxIntrinsic:
        rgb = {0xdc, 0xdc, 0xaa};
        break;
    case SyntaxColor::SyntaxType:
        rgb = {0x4e, 0xc9, 0xb0};
        break;
    case SyntaxColor::SyntaxKeyword:
        rgb = {0x56, 0x9c, 0xd6};
        break;
    case SyntaxColor::SyntaxLogic:
        rgb = {0xd8, 0xa0, 0xdf};
        break;
    case SyntaxColor::SyntaxNumber:
        rgb = {0xb5, 0xce, 0xa8};
        break;
    case SyntaxColor::SyntaxString:
        rgb = {0xce, 0x91, 0x78};
        break;
    case SyntaxColor::SyntaxAttribute:
        rgb = {0xaa, 0xaa, 0xaa};
        break;
    default:
        rgb = {0x00, 0x00, 0x00};
        SWAG_ASSERT(false);
        break;
    }

    if (lum > 0)
    {
        float h, s, l;
        rgbToHsl(rgb, &h, &s, &l);
        rgb = hslToRgb(h, s, lum);
    }

    return (rgb.r << 16) | (rgb.g << 8) | (rgb.b);
}

static Utf8 getColor(SyntaxColorMode mode, SyntaxColor color)
{
    switch (mode)
    {
    case SyntaxColorMode::ForLog:
    {
        if (color == SyntaxColor::SyntaxDefault)
            color = SyntaxColor::SyntaxCode;
        auto rgb = getSyntaxColor(color, g_CommandLine.errorSyntaxColorLum);
        return Fmt("\x1b[38;2;%d;%d;%dm", (rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
    break;

    case SyntaxColorMode::ForDoc:
    {
        const char* colorName = nullptr;
        switch (color)
        {
        case SyntaxColor::SyntaxDefault:
            return "</span>";
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
        case SyntaxColor::SyntaxAttribute:
            colorName = "SyntaxAttribute";
            break;
        }

        if (colorName)
            return Fmt("<span class=\"%s\">", colorName);
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

    result += getColor(mode, SyntaxColor::SyntaxCode);
    pz = Utf8::decodeUtf8(pz, c, offset);
    while (c)
    {
        Utf8 identifier;

        // Attribute
        if (c == '#' && *pz == '[')
        {
            result += getColor(mode, SyntaxColor::SyntaxAttribute);
            result += c;
            result += *pz++;

            int cpt = 1;
            while (cpt && *pz)
            {
                if (*pz == '"')
                {
                    result += *pz++;
                    while (*pz && *pz != '"')
                    {
                        if (*pz == '\\')
                        {
                            result += *pz++;
                            result += *pz++;
                        }
                        else
                            result += *pz++;
                    }
                    if (*pz)
                        result += *pz++;
                    continue;
                }
                else if (*pz == '[')
                    cpt++;
                else if (*pz == ']')
                    cpt--;
                result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += getColor(mode, SyntaxColor::SyntaxDefault);
            continue;
        }

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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
            result += getColor(mode, SyntaxColor::SyntaxDefault);
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
                    result += getColor(mode, SyntaxColor::SyntaxDefault);
                    break;

                case TokenId::KwdCode:
                case TokenId::KwdCVarArgs:
                case TokenId::NativeType:
                case TokenId::CompilerType:
                    result += getColor(mode, SyntaxColor::SyntaxType);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxDefault);
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
                    result += getColor(mode, SyntaxColor::SyntaxDefault);
                    break;

                case TokenId::CompilerRun:
                case TokenId::CompilerAst:
                case TokenId::CompilerFuncMessage:
                case TokenId::CompilerFuncDrop:
                case TokenId::CompilerFuncInit:
                case TokenId::CompilerFuncMain:
                case TokenId::CompilerFuncPreMain:
                case TokenId::CompilerFuncTest:
                    result += getColor(mode, SyntaxColor::SyntaxFunction);
                    result += identifier;
                    result += getColor(mode, SyntaxColor::SyntaxDefault);
                    break;

                default:
                    if (identifier[0] == '@')
                    {
                        result += getColor(mode, SyntaxColor::SyntaxIntrinsic);
                        result += identifier;
                        result += getColor(mode, SyntaxColor::SyntaxDefault);
                    }
                    else if (identifier[0] == '#')
                    {
                        result += getColor(mode, SyntaxColor::SyntaxCompiler);
                        result += identifier;
                        result += getColor(mode, SyntaxColor::SyntaxDefault);
                    }
                    else
                    {
                        result += identifier;
                    }

                    break;
                }
            }
            else if (identifier[0] == '@')
            {
                result += getColor(mode, SyntaxColor::SyntaxIntrinsic);
                result += identifier;
                result += getColor(mode, SyntaxColor::SyntaxDefault);
            }
            else if (identifier == "self" || identifier == "Self")
            {
                result += getColor(mode, SyntaxColor::SyntaxKeyword);
                result += identifier;
                result += getColor(mode, SyntaxColor::SyntaxDefault);
            }
            else if (identifier[0] >= 'a' and identifier[0] <= 'z' && (c == '(' || c == '\''))
            {
                result += getColor(mode, SyntaxColor::SyntaxFunction);
                result += identifier;
                result += getColor(mode, SyntaxColor::SyntaxDefault);
            }
            else if (identifier[0] >= 'A' and identifier[0] <= 'Z')
            {
                result += getColor(mode, SyntaxColor::SyntaxConstant);
                result += identifier;
                result += getColor(mode, SyntaxColor::SyntaxDefault);
            }
            else
            {
                result += identifier;
            }

            continue;
        }

        result += c;
        pz = Utf8::decodeUtf8(pz, c, offset);
    }

    result += getColor(mode, SyntaxColor::SyntaxDefault);
    return result;
}
