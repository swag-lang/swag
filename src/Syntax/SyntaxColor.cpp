#include "pch.h"
#include "Syntax/SyntaxColor.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

struct RgbColor
{
    unsigned char r, g, b;
};

namespace
{
    void rgbToHsl(const RgbColor& color, float* h, float* s, float* l)
    {
        const float r = static_cast<float>(color.r) / 255.0f;
        const float g = static_cast<float>(color.g) / 255.0f;
        const float b = static_cast<float>(color.b) / 255.0f;

        const float maxVal = fmaxf(fmaxf(r, g), b);
        const float minVal = fminf(fminf(r, g), b);

        *l = (maxVal + minVal) / 2;

        if (maxVal == minVal)
        {
            *h = *s = 0;
        }
        else
        {
            const float d = maxVal - minVal;
            *s            = *l > 0.5f ? d / (2 - maxVal - minVal) : d / (maxVal + minVal);

            if (maxVal == r)
            {
                *h = (g - b) / d + (g < b ? 6.0f : 0.0f);
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

    float hueToRgb(float p, float q, float t)
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

    RgbColor hslToRgb(float h, float s, float l)
    {
        float r, g, b;

        if (s == 0)
        {
            r = g = b = l;
        }
        else
        {
            const float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
            const float p = 2 * l - q;

            r = hueToRgb(p, q, h + 1.0f / 3);
            g = hueToRgb(p, q, h);
            b = hueToRgb(p, q, h - 1.0f / 3);
        }

        RgbColor result;
        result.r = static_cast<unsigned char>(r * 255.0f);
        result.g = static_cast<unsigned char>(g * 255.0f);
        result.b = static_cast<unsigned char>(b * 255.0f);
        return result;
    }
}

Utf8 syntaxColorToVTS(SyntaxColor color, SyntaxColorMode mode)
{
    switch (mode)
    {
        case SyntaxColorMode::ForLog:
        {
            if (color == SyntaxColor::SyntaxDefault)
                color = SyntaxColor::SyntaxCode;
            const auto rgb = getSyntaxColorRgb(mode, color, g_CommandLine.errorSyntaxColorLum);
            return form("\x1b[38;2;%d;%d;%dm", (rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
        }

        case SyntaxColorMode::ForDoc:
        {
            const char* colorName = nullptr;
            switch (color)
            {
                case SyntaxColor::SyntaxDefault:
                    return "</span>";
                case SyntaxColor::SyntaxCode:
                    colorName = SYN_CODE;
                    break;
                case SyntaxColor::SyntaxComment:
                    colorName = SYN_COMMENT;
                    break;
                case SyntaxColor::SyntaxCompiler:
                    colorName = SYN_COMPILER;
                    break;
                case SyntaxColor::SyntaxFunction:
                    colorName = SYN_FUNCTION;
                    break;
                case SyntaxColor::SyntaxConstant:
                    colorName = SYN_CONSTANT;
                    break;
                case SyntaxColor::SyntaxIntrinsic:
                    colorName = SYN_INTRINSIC;
                    break;
                case SyntaxColor::SyntaxType:
                    colorName = SYN_TYPE;
                    break;
                case SyntaxColor::SyntaxKeyword:
                    colorName = SYN_KEYWORD;
                    break;
                case SyntaxColor::SyntaxLogic:
                    colorName = SYN_LOGIC;
                    break;
                case SyntaxColor::SyntaxNumber:
                    colorName = SYN_NUMBER;
                    break;
                case SyntaxColor::SyntaxBcRegister:
                    colorName = SYN_BC_REGISTER;
                    break;
                case SyntaxColor::SyntaxBcKeyword:
                    colorName = SYN_BC_KEYWORD;
                    break;
                case SyntaxColor::SyntaxBcConstant:
                    colorName = SYN_BC_CONSTANT;
                    break;
                case SyntaxColor::SyntaxString:
                    colorName = SYN_STRING;
                    break;
                case SyntaxColor::SyntaxAttribute:
                    colorName = SYN_ATTRIBUTE;
                    break;
                case SyntaxColor::SyntaxInvalid:
                    colorName = SYN_INVALID;
                    break;
            }

            if (colorName)
                return form("<span class=\"%s\">", colorName);
            break;
        }
    }

    SWAG_ASSERT(false);
    return "";
}

uint32_t getSyntaxColorRgb(SyntaxColorMode /*mode*/, SyntaxColor color, float lum)
{
    RgbColor rgb;
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
            rgb = {0x4E, 0xC9, 0xB0};
            break;
        case SyntaxColor::SyntaxIntrinsic:
            rgb = {0xdc, 0xdc, 0xaa};
            break;
        case SyntaxColor::SyntaxType:
            rgb = {0xf6, 0xcc, 0x86};
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

        case SyntaxColor::SyntaxBcKeyword:
            rgb = {0x76, 0xBc, 0xF6};
            break;
        case SyntaxColor::SyntaxBcConstant:
            rgb = {0x76, 0xF6, 0xBc};
            break;
        case SyntaxColor::SyntaxBcRegister:
            rgb = {0xAF, 0xAF, 0x00};
            break;

        case SyntaxColor::SyntaxInvalid:
            rgb = {0xFF, 0x47, 0x47};
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

    return rgb.r << 16 | rgb.g << 8 | rgb.b;
}

Utf8 doSyntaxColor(const Utf8& line, SyntaxColorContext& context)
{
    if (!g_CommandLine.errorSyntaxColor)
        return line;
    if (!g_CommandLine.logColors)
        return line;

    auto     mode = context.mode;
    auto     pz   = line.c_str();
    uint32_t c, offset;
    Utf8     result;

    bool hasCode = false;
    pz           = Utf8::decodeUtf8(pz, c, offset);
    while (c)
    {
        // Multi-line comment
        if (context.multiLineCommentLevel || (c == '/' && pz[0] == '*'))
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxComment, mode);

            result += c;
            if (!context.multiLineCommentLevel)
            {
                result += *pz++;
                context.multiLineCommentLevel++;
            }

            while (*pz)
            {
                if (pz[0] == '/' && pz[1] == '*')
                {
                    context.multiLineCommentLevel++;
                    result += "*/";
                    pz += 2;
                    continue;
                }

                if (pz[0] == '*' && pz[1] == '/')
                {
                    result += "*/";
                    pz += 2;
                    context.multiLineCommentLevel--;
                    if (context.multiLineCommentLevel == 0)
                        break;
                    continue;
                }

                result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Line comment
        if (c == '/' && pz[0] == '/')
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxComment, mode);
            result += c;
            while (*pz && SWAG_IS_NOT_EOL(*pz))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Attribute
        if (c == '#' && *pz == '[')
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxAttribute, mode);
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
                if (*pz == '[')
                    cpt++;
                else if (*pz == ']')
                    cpt--;
                result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Raw string
        if (c == '#' && *pz == '"')
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxString, mode);
            result += c;
            while (*pz && (pz[0] != '"' || pz[1] != '#'))
                result += *pz++;

            if (*pz)
            {
                result += *pz++;
                result += *pz++;
            }

            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // String
        if (c == '"')
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxString, mode);
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
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Character
        if (c == '`')
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxString, mode);
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
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Binary literal
        if (c == '0' && (*pz == 'b' || *pz == 'B'))
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxNumber, mode);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Hexadecimal literal
        if (c == '0' && (*pz == 'x' || *pz == 'X'))
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxNumber, mode);
            result += c;
            result += *pz++;
            while (*pz && (SWAG_IS_HEX(*pz) || *pz == '_'))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Number
        if (SWAG_IS_DIGIT(c))
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxNumber, mode);
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
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Bytecode register
        if (context.forByteCode && c == '$' && SWAG_IS_ALPHA(pz[0]))
        {
            result += syntaxColorToVTS(SyntaxColor::SyntaxBcRegister, mode);
            result += c;

            while (*pz && SWAG_IS_AL_NUM(*pz))
                result += *pz++;
            pz = Utf8::decodeUtf8(pz, c, offset);
            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            continue;
        }

        // Modifier
        if (c == ',')
        {
            Utf8     identifier;
            uint32_t c1;
            uint32_t offset1;
            auto     pz1 = Utf8::decodeUtf8(pz, c1, offset1);
            while (SWAG_IS_ALPHA(c1))
            {
                identifier += c1;
                pz1 = Utf8::decodeUtf8(pz1, c1, offset1);
            }

            auto it = g_LangSpec->modifiers.find(identifier);
            if (it)
            {
                result += syntaxColorToVTS(SyntaxColor::SyntaxIntrinsic, mode);
                result += ',';
                result += identifier;
                pz     = pz1;
                c      = c1;
                offset = offset1;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                continue;
            }
        }

        // Word
        if (SWAG_IS_ALPHA(c) || c == '_' || c == '#' || c == '@')
        {
            Utf8 identifier;
            identifier += c;
            pz = Utf8::decodeUtf8(pz, c, offset);
            while (SWAG_IS_ALPHA(c) || c == '_' || SWAG_IS_DIGIT(c))
            {
                identifier += c;
                pz = Utf8::decodeUtf8(pz, c, offset);
            }

            // Bytecode keyword
            if (context.forByteCode)
            {
                auto it = g_LangSpec->bckeywords.find(identifier);
                if (it != g_LangSpec->bckeywords.end())
                {
                    result += syntaxColorToVTS(SyntaxColor::SyntaxBcKeyword, mode);
                    result += identifier;
                    result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                    continue;
                }
            }

            // Bytecode constant
            if (context.forByteCode)
            {
                auto it = g_LangSpec->bcconstants.find(identifier);
                if (it != g_LangSpec->bcconstants.end())
                {
                    result += syntaxColorToVTS(SyntaxColor::SyntaxBcConstant, mode);
                    result += identifier;
                    result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                    continue;
                }
            }

            auto it = g_LangSpec->keywords.find(identifier);
            if (it)
            {
                switch (*it)
                {
                    case TokenId::KwdReserved:
                        result += syntaxColorToVTS(SyntaxColor::SyntaxInvalid, mode);
                        result += identifier;
                        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                        break;

                    case TokenId::KwdUsing:
                    case TokenId::KwdWith:
                    case TokenId::KwdCast:
                    case TokenId::KwdAutoCast:
                    case TokenId::KwdDeRef:
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
                        result += syntaxColorToVTS(SyntaxColor::SyntaxKeyword, mode);
                        result += identifier;
                        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                        break;

                    case TokenId::KwdCode:
                    case TokenId::KwdCVarArgs:
                    case TokenId::NativeType:
                    case TokenId::CompilerType:
                        result += syntaxColorToVTS(SyntaxColor::SyntaxType, mode);
                        result += identifier;
                        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
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
                    case TokenId::KwdTo:
                    case TokenId::KwdUntil:
                    case TokenId::KwdDo:
                        result += syntaxColorToVTS(SyntaxColor::SyntaxLogic, mode);
                        result += identifier;
                        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                        break;

                    case TokenId::CompilerDependencies:
                    case TokenId::CompilerRun:
                    case TokenId::CompilerAst:
                    case TokenId::CompilerFuncMessage:
                    case TokenId::CompilerFuncDrop:
                    case TokenId::CompilerFuncInit:
                    case TokenId::CompilerFuncMain:
                    case TokenId::CompilerFuncPreMain:
                    case TokenId::CompilerFuncTest:
                        result += syntaxColorToVTS(SyntaxColor::SyntaxFunction, mode);
                        result += identifier;
                        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                        break;

                    default:
                        if (identifier[0] == '@')
                        {
                            result += syntaxColorToVTS(SyntaxColor::SyntaxIntrinsic, mode);
                            result += identifier;
                            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
                        }
                        else if (identifier[0] == '#')
                        {
                            result += syntaxColorToVTS(SyntaxColor::SyntaxCompiler, mode);
                            result += identifier;
                            result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
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
                result += syntaxColorToVTS(SyntaxColor::SyntaxIntrinsic, mode);
                result += identifier;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            }
            else if (identifier == g_LangSpec->name_self)
            {
                result += syntaxColorToVTS(SyntaxColor::SyntaxKeyword, mode);
                result += identifier;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            }
            else if (identifier == g_LangSpec->name_Self)
            {
                result += syntaxColorToVTS(SyntaxColor::SyntaxType, mode);
                result += identifier;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            }
            else if (identifier[0] >= 'a' and identifier[0] <= 'z' && (c == '(' || c == '\''))
            {
                result += syntaxColorToVTS(SyntaxColor::SyntaxFunction, mode);
                result += identifier;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            }
            else if (identifier[0] >= 'A' and identifier[0] <= 'Z')
            {
                result += syntaxColorToVTS(SyntaxColor::SyntaxConstant, mode);
                result += identifier;
                result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
            }
            else
            {
                hasCode = true;
                result += identifier;
            }

            continue;
        }

        hasCode = true;
        result += c;
        pz = Utf8::decodeUtf8(pz, c, offset);
    }

    if (hasCode)
    {
        result.insert(0, syntaxColorToVTS(SyntaxColor::SyntaxCode, mode));
        result += syntaxColorToVTS(SyntaxColor::SyntaxDefault, mode);
    }

    return result;
}
