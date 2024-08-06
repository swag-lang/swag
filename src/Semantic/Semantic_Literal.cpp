#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Main/Version.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool Semantic::getDigitHex(SemanticContext* context, const SourceLocation& startLoc, const char* pzs, const char** pzr, int& result, const char* errMsg)
{
    const auto node = context->node;
    const auto pz   = *pzr;
    const auto c    = *pz;
    (*pzr)++;

    if (!SWAG_IS_HEX(c))
    {
        auto endLoc = startLoc;
        endLoc.column += static_cast<uint32_t>(*pzr - pzs);
        return context->report({node->token.sourceFile, startLoc, endLoc, errMsg});
    }

    if (c >= 'a' && c <= 'z')
        result = 10 + (c - 'a');
    else if (c >= 'A' && c <= 'Z')
        result = 10 + (c - 'A');
    else
        result = c - '0';
    return true;
}

bool Semantic::processLiteralString(SemanticContext* context)
{
    const auto node = castAst<AstLiteral>(context->node, AstNodeKind::Literal);
    if (node->literalType != LiteralType::TypeStringEscape &&
        node->literalType != LiteralType::TypeStringMultiLineEscape &&
        node->literalType != LiteralType::TypeCharacterEscape)
        return true;

    auto       loc = node->token.startLocation;
    Utf8       result;
    const auto len = node->computedValue()->text.length();
    result.reserve(len);

    const auto start = static_cast<const char*>(node->computedValue()->text.buffer);
    auto       pz    = start;
    while (pz - start < len)
    {
        auto c = *pz++;
        loc.column += 1;

        if (c != '\\')
        {
            result.append(c);
            if (SWAG_IS_EOL(c))
            {
                loc.column = 0;
                loc.line += 1;
            }

            continue;
        }

        c = *pz++;
        loc.column += 1;

        switch (c)
        {
            case '0':
                result.append('\0');
                continue;
            case 'a':
                result.append('\a');
                continue;
            case 'b':
                result.append('\b');
                continue;
            case '\\':
                result.append('\\');
                continue;
            case 't':
                result.append('\t');
                continue;
            case 'n':
                result.append('\n');
                continue;
            case 'f':
                result.append('\f');
                continue;
            case 'r':
                result.append('\r');
                continue;
            case 'v':
                result.append('\v');
                continue;
            case '\'':
                result.append('\'');
                continue;
            case '\"':
                result.append('\"');
                continue;
            case '`':
                result.append('`');
                continue;
            case 'x':
            {
                int        c1, c2;
                auto       msg = toErr(Err0209);
                const auto pzs = pz;
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c1, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c2, msg));
                const char32_t cw = (c1 << 4) + c2;
                result.append(static_cast<char>(cw));
                loc.column += 2;
                continue;
            }
            case 'u':
            {
                int        c1, c2, c3, c4;
                auto       msg = toErr(Err0208);
                const auto pzs = pz;
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c1, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c2, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c3, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c4, msg));
                const char32_t cw = (c1 << 12) + (c2 << 8) + (c3 << 4) + c4;
                result.append(cw);
                loc.column += 4;
                continue;
            }
            case 'U':
            {
                int        c1, c2, c3, c4, c5, c6, c7, c8;
                auto       msg = toErr(Err0207);
                const auto pzs = pz;
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c1, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c2, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c3, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c4, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c5, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c6, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c7, msg));
                SWAG_CHECK(getDigitHex(context, loc, pzs, &pz, c8, msg));
                const char32_t cw = (c1 << 28) + (c2 << 24) + (c3 << 20) + (c4 << 16) + (c5 << 12) + (c6 << 8) + (c7 << 4) + c8;
                if (cw > Utf8::MAX_ENCODED_UNICODE)
                    return context->report({node->token.sourceFile, loc, formErr(Err0353, cw)});
                result.append(cw);
                loc.column += 8;
                continue;
            }
            default:
                break;
        }

        return context->report({node->token.sourceFile, loc, formErr(Err0206, c)});
    }

    node->computedValue()->text = result;
    return true;
}

Utf8 Semantic::checkLiteralValue(ComputedValue& computedValue, LiteralType& literalType, const Register& literalValue, TypeInfo* typeSuffix, bool negApplied)
{
    if (negApplied)
    {
        switch (typeSuffix->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                computedValue.reg.s64 = -computedValue.reg.s64;
                break;
            default:
                break;
        }

        switch (literalType)
        {
            case LiteralType::TypeUnsigned32:
            case LiteralType::TypeUntypedBinHexa:
            case LiteralType::TypeUntypedInt:
                literalType = LiteralType::TypeSigned32;
                break;
            case LiteralType::TypeUnsigned64:
                literalType = LiteralType::TypeSigned64;
                break;
            default:
                break;
        }
    }

    switch (literalType)
    {
        case LiteralType::TypeCharacter:
        case LiteralType::TypeCharacterEscape:
        {
            VectorNative<uint32_t> uni;
            computedValue.text.toUni32(uni);
            if (uni.size() != 1)
                return formErr(Err0189, computedValue.text.c_str());

            if (typeSuffix->isUntypedInteger())
            {
                computedValue.reg.u64 = uni[0];
            }
            else
            {
                switch (typeSuffix->nativeType)
                {
                    case NativeTypeKind::Rune:
                        computedValue.reg.ch = uni[0];
                        break;

                    case NativeTypeKind::U8:
                        if (uni[0] > UINT8_MAX)
                            return formErr(Err0560, uni[0]);
                        computedValue.reg.u8 = static_cast<uint8_t>(uni[0]);
                        break;

                    case NativeTypeKind::U16:
                        if (uni[0] > UINT16_MAX)
                            return formErr(Err0559, uni[0]);
                        computedValue.reg.u16 = static_cast<uint16_t>(uni[0]);
                        break;

                    case NativeTypeKind::U32:
                        computedValue.reg.u32 = uni[0];
                        break;

                    case NativeTypeKind::U64:
                        computedValue.reg.u64 = uni[0];
                        break;

                    default:
                        return formErr(Err0188, typeSuffix->getDisplayNameC());
                }
            }

            break;
        }

        case LiteralType::TypeUntypedFloat:
            switch (typeSuffix->nativeType)
            {
                case NativeTypeKind::F32:
                    computedValue.reg.f32 = static_cast<float>(literalValue.f64);
                    if (negApplied)
                        computedValue.reg.f32 = -computedValue.reg.f32;
                    break;
                case NativeTypeKind::F64:
                    if (negApplied)
                        computedValue.reg.f64 = -computedValue.reg.f64;
                    break;
                default:
                    return formErr(Err0244, typeSuffix->getDisplayNameC());
            }
            break;

        case LiteralType::TypeSigned32:
        case LiteralType::TypeSigned64:
        case LiteralType::TypeUntypedInt:
            switch (typeSuffix->nativeType)
            {
                case NativeTypeKind::U8:
                    if (computedValue.reg.u64 > UINT8_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u8"));
                    break;
                case NativeTypeKind::U16:
                    if (computedValue.reg.u64 > UINT16_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u16"));
                    break;
                case NativeTypeKind::U32:
                    if (computedValue.reg.u64 > UINT32_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u32"));
                    break;
                case NativeTypeKind::U64:
                    break;

                case NativeTypeKind::S8:
                    if (computedValue.reg.s64 < INT8_MIN || computedValue.reg.s64 > INT8_MAX)
                        return formErr(Err0366, computedValue.reg.s64, "s8");
                    break;
                case NativeTypeKind::S16:
                    if (computedValue.reg.s64 < INT16_MIN || computedValue.reg.s64 > INT16_MAX)
                        return formErr(Err0366, computedValue.reg.s64, "s16");
                    break;
                case NativeTypeKind::S32:
                    if (computedValue.reg.s64 < INT32_MIN || computedValue.reg.s64 > INT32_MAX)
                        return formErr(Err0366, computedValue.reg.s64, "s32");
                    break;
                case NativeTypeKind::S64:
                    break;

                case NativeTypeKind::Rune:
                    if (computedValue.reg.u64 > UINT32_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "rune"));
                    break;

                case NativeTypeKind::F32:
                    computedValue.reg.f32 = static_cast<float>(computedValue.reg.s64);
                    break;
                case NativeTypeKind::F64:
                    computedValue.reg.f64 = static_cast<double>(computedValue.reg.s64);
                    break;

                default:
                    return formErr(Err0241, computedValue.reg.u64);
            }

            break;

        case LiteralType::TypeUnsigned32:
        case LiteralType::TypeUnsigned64:
        case LiteralType::TypeUntypedBinHexa:
            switch (typeSuffix->nativeType)
            {
                case NativeTypeKind::U8:
                    if (computedValue.reg.u64 > UINT8_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u8"));
                    break;
                case NativeTypeKind::U16:
                    if (computedValue.reg.u64 > UINT16_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u16"));
                    break;
                case NativeTypeKind::U32:
                    if (computedValue.reg.u64 > UINT32_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "u32"));
                    break;
                case NativeTypeKind::U64:
                    break;

                case NativeTypeKind::S8:
                    if (computedValue.reg.u64 > UINT8_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "s8"));
                    break;
                case NativeTypeKind::S16:
                    if (computedValue.reg.u64 > UINT16_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "s16"));
                    break;
                case NativeTypeKind::S32:
                    if (computedValue.reg.u64 > UINT32_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "s32"));
                    break;
                case NativeTypeKind::S64:
                    if (computedValue.reg.u64 > static_cast<uint64_t>(INT64_MAX) + 1)
                        return form(formErr(Err0366, computedValue.reg.u64, "s64"));
                    break;

                case NativeTypeKind::Rune:
                    if (computedValue.reg.u64 > UINT32_MAX)
                        return form(formErr(Err0366, computedValue.reg.u64, "rune"));
                    break;

                default:
                    return formErr(Err0241, computedValue.reg.u64);
            }

            break;
    }

    return "";
}

bool Semantic::resolveLiteralSuffix(SemanticContext* context)
{
    const auto node = context->node;

    // Can be a predefined native type
    if (node->token.is(TokenId::NativeType))
    {
        const auto it = g_LangSpec->nativeTypes.find(node->token.text);
        SWAG_ASSERT(it);
        node->typeInfo = TypeManager::literalTypeToType(*it);
        return true;
    }

    // Search if identifier is a type
    if (node->token.is(TokenId::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);

        // We do not want error. If identifier is not known, then we consider it as a 'user' suffix
        identifier->identifierRef()->addAstFlag(AST_SILENT_CHECK);

        const auto res = resolveIdentifier(context, identifier, RI_ZERO);
        YIELD();
        if (res && identifier->typeInfo)
            return true;

        SWAG_ASSERT(node->getParent(2)->is(AstNodeKind::Literal));
        node->getParent(2)->addSemFlag(SEMFLAG_LITERAL_SUFFIX);
        return true;
    }

    return Report::internalError(context->node, "resolveLiteralSuffix, invalid token");
}

bool Semantic::resolveLiteral(SemanticContext* context)
{
    auto       node       = castAst<AstLiteral>(context->node, AstNodeKind::Literal);
    const auto sourceFile = context->sourceFile;

    switch (node->token.id)
    {
        case TokenId::KwdTrue:
            node->token.id       = TokenId::LiteralNumber;
            node->literalType    = LiteralType::TypeBool;
            node->literalValue.b = true;
            break;
        case TokenId::KwdFalse:
            node->token.id       = TokenId::LiteralNumber;
            node->literalType    = LiteralType::TypeBool;
            node->literalValue.b = false;
            break;
        case TokenId::KwdNull:
            node->token.id             = TokenId::LiteralNumber;
            node->literalType          = LiteralType::TypeNull;
            node->literalValue.pointer = nullptr;
            break;
        case TokenId::CompilerFile:
            node->token.id    = TokenId::LiteralString;
            node->literalType = LiteralType::TypeString;
            node->token.text  = sourceFile->path;
            break;
        case TokenId::CompilerModule:
            node->token.id    = TokenId::LiteralString;
            node->literalType = LiteralType::TypeString;
            node->token.text  = sourceFile->module ? sourceFile->module->name : Utf8("?");
            break;
        case TokenId::CompilerLine:
            node->token.id         = TokenId::LiteralNumber;
            node->literalType      = LiteralType::TypeUntypedInt;
            node->literalValue.u32 = node->token.startLocation.line + 1;
            break;
        case TokenId::CompilerBuildVersion:
            node->token.id         = TokenId::LiteralNumber;
            node->literalType      = LiteralType::TypeSigned32;
            node->literalValue.s32 = SWAG_BUILD_VERSION;
            break;
        case TokenId::CompilerBuildRevision:
            node->token.id         = TokenId::LiteralNumber;
            node->literalType      = LiteralType::TypeSigned32;
            node->literalValue.s32 = SWAG_BUILD_REVISION;
            break;
        case TokenId::CompilerBuildNum:
            node->token.id         = TokenId::LiteralNumber;
            node->literalType      = LiteralType::TypeSigned32;
            node->literalValue.s32 = SWAG_BUILD_NUM;
            break;
    }

    node->typeInfo = TypeManager::literalTypeToType(node->literalType, node->literalValue);
    node->setFlagsValueIsComputed();
    node->computedValue()->reg  = node->literalValue;
    node->computedValue()->text = node->token.text;
    node->addAstFlag(AST_R_VALUE);

    SWAG_CHECK(processLiteralString(context));

    // Suffix
    auto suffix = node->children.empty() ? nullptr : node->firstChild();
    if (!suffix || !suffix->typeInfo)
    {
        // If there's a suffix without a type, then this should be a 'user' suffix
        // Literal must have been flagged
        SWAG_ASSERT(!suffix || node->hasSemFlag(SEMFLAG_LITERAL_SUFFIX));

        // Convert to unsigned int for a character without suffix
        if (node->token.is(TokenId::LiteralCharacter))
        {
            auto errMsg = checkLiteralValue(*node->computedValue(), node->literalType, node->literalValue, g_TypeMgr->typeInfoCharacter, false);
            if (!errMsg.empty())
                return context->report({node, errMsg});
            node->typeInfo    = g_TypeMgr->typeInfoCharacter;
            node->byteCodeFct = ByteCodeGen::emitLiteral;
            return true;
        }

        // By default, a float without a suffix is considered as f32 (not f64 like in C).
        if (node->typeInfo->isNative(NativeTypeKind::F32) && node->literalType == LiteralType::TypeUntypedFloat)
            node->computedValue()->reg.f32 = static_cast<float>(node->literalValue.f64);
        return true;
    }

    // Check suffix type is correct (should be native)
    if (suffix->token.isNot(TokenId::NativeType))
    {
        const auto symbolName = suffix->resolvedSymbolName();
        if (symbolName && symbolName->isNot(SymbolKind::TypeAlias))
            return context->report({suffix, formErr(Err0242, suffix->resolvedSymbolName()->name.c_str(), Naming::aKindName(symbolName->kind).c_str())});
    }

    const auto suffixType = TypeManager::concreteType(suffix->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(suffixType->isNative(), context->report({suffix, formErr(Err0243, suffixType->getDisplayNameC())}));

    switch (suffixType->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::Rune:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
            break;
        default:
            return context->report({suffix, formErr(Err0243, suffix->typeInfo->getDisplayNameC())});
    }

    // Check if this is in fact a negative literal. This is important to know now, in order
    // to be able to correctly check bounds.
    bool negApplied = false;
    if (node->parent->is(AstNodeKind::SingleOp) && node->parent->token.is(TokenId::SymMinus))
    {
        switch (suffix->typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                node->addSemFlag(SEMFLAG_NEG_EATEN);
                negApplied = true;
                break;
            default:
                break;
        }
    }

    const auto errMsg = checkLiteralValue(*node->computedValue(), node->literalType, node->literalValue, suffixType, negApplied);
    if (!errMsg.empty())
    {
        Diagnostic err{node, node->token, errMsg};
        if (suffix && suffix->typeInfo)
            err.addNote(suffix, formNte(Nte0192, suffix->typeInfo->getDisplayNameC()));
        return context->report(err);
    }

    node->typeInfo    = suffixType;
    node->byteCodeFct = ByteCodeGen::emitLiteral;
    return true;
}
