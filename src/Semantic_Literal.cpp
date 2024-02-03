#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "TypeManager.h"
#include "Version.h"

bool Semantic::getDigitHexa(SemanticContext* context, const SourceLocation& startLoc, const char* pzs, const char** _pz, int& result, const char* errMsg)
{
    auto node = context->node;
    auto pz   = *_pz;
    auto c    = *pz;
    (*_pz)++;

    if (!SWAG_IS_HEX(c))
    {
        auto endLoc = startLoc;
        endLoc.column += (uint32_t) (*_pz - pzs);
        return context->report({node->sourceFile, startLoc, endLoc, errMsg});
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
    auto node = CastAst<AstLiteral>(context->node, AstNodeKind::Literal);
    if (node->literalType != LiteralType::TT_STRING_ESCAPE &&
        node->literalType != LiteralType::TT_STRING_MULTILINE_ESCAPE &&
        node->literalType != LiteralType::TT_CHARACTER_ESCAPE)
        return true;

    auto loc = node->token.startLocation;
    Utf8 result;
    auto len = node->computedValue->text.length();
    result.reserve(len);

    auto start = (const char*) node->computedValue->text.buffer;
    auto pz    = start;
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
            int  c1, c2;
            auto msg = Err(Err0278);
            auto pzs = pz;
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c2, msg));
            char32_t cw = (c1 << 4) + c2;
            result.append((char) cw);
            loc.column += 2;
            continue;
        }
        case 'u':
        {
            int  c1, c2, c3, c4;
            auto msg = Err(Err0277);
            auto pzs = pz;
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c2, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c3, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c4, msg));
            char32_t cw = (c1 << 12) + (c2 << 8) + (c3 << 4) + c4;
            result.append(cw);
            loc.column += 4;
            continue;
        }
        case 'U':
        {
            int  c1, c2, c3, c4, c5, c6, c7, c8;
            auto msg = Err(Err0276);
            auto pzs = pz;
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c2, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c3, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c4, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c5, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c6, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c7, msg));
            SWAG_CHECK(getDigitHexa(context, loc, pzs, &pz, c8, msg));
            char32_t cw = (c1 << 28) + (c2 << 24) + (c3 << 20) + (c4 << 16) + (c5 << 12) + (c6 << 8) + (c7 << 4) + c8;
            if (cw > Utf8::MAX_ENCODED_UNICODE)
                return context->report({node->sourceFile, loc, Fmt(Err(Err0405), cw)});
            result.append(cw);
            loc.column += 8;
            continue;
        }
        }

        return context->report({node->sourceFile, loc, Fmt(Err(Err0275), c)});
    }

    node->computedValue->text = result;
    return true;
}

Utf8 Semantic::checkLiteralValue(ComputedValue& computedValue, LiteralType& literalType, Register& literalValue, TypeInfo* typeSuffix, bool negApplied)
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
        case LiteralType::TT_U32:
        case LiteralType::TT_UNTYPED_BINHEXA:
        case LiteralType::TT_UNTYPED_INT:
            literalType = LiteralType::TT_S32;
            break;
        case LiteralType::TT_U64:
            literalType = LiteralType::TT_S64;
            break;
        default:
            break;
        }
    }

    switch (literalType)
    {
    case LiteralType::TT_CHARACTER:
    case LiteralType::TT_CHARACTER_ESCAPE:
    {
        VectorNative<uint32_t> uni;
        computedValue.text.toUni32(uni);
        if (uni.size() != 1)
            return Fmt(Err(Err0236), computedValue.text.c_str());

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
                    return Fmt(Err(Err0607), uni[0]);
                computedValue.reg.u8 = (uint8_t) uni[0];
                break;

            case NativeTypeKind::U16:
                if (uni[0] > UINT16_MAX)
                    return Fmt(Err(Err0606), uni[0]);
                computedValue.reg.u16 = (uint16_t) uni[0];
                break;

            case NativeTypeKind::U32:
                computedValue.reg.u32 = uni[0];
                break;

            case NativeTypeKind::U64:
                computedValue.reg.u64 = uni[0];
                break;

            default:
                return Fmt(Err(Err0235), typeSuffix->getDisplayNameC());
            }
        }

        break;
    }

    case LiteralType::TT_UNTYPED_FLOAT:
        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::F32:
            computedValue.reg.f32 = (float) literalValue.f64;
            if (negApplied)
                computedValue.reg.f32 = -computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            if (negApplied)
                computedValue.reg.f64 = -computedValue.reg.f64;
            break;
        default:
            return Fmt(Err(Err0321), typeSuffix->getDisplayNameC());
        }
        break;

    case LiteralType::TT_S32:
    case LiteralType::TT_S64:
    case LiteralType::TT_UNTYPED_INT:
        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::U8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u8"));
            break;
        case NativeTypeKind::U16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u16"));
            break;
        case NativeTypeKind::U32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u32"));
            break;
        case NativeTypeKind::U64:
            break;

        case NativeTypeKind::S8:
            if (computedValue.reg.s64 < INT8_MIN || computedValue.reg.s64 > INT8_MAX)
                return Fmt(Err(Err0425), computedValue.reg.s64, "s8");
            break;
        case NativeTypeKind::S16:
            if (computedValue.reg.s64 < INT16_MIN || computedValue.reg.s64 > INT16_MAX)
                return Fmt(Err(Err0425), computedValue.reg.s64, "s16");
            break;
        case NativeTypeKind::S32:
            if (computedValue.reg.s64 < INT32_MIN || computedValue.reg.s64 > INT32_MAX)
                return Fmt(Err(Err0425), computedValue.reg.s64, "s32");
            break;
        case NativeTypeKind::S64:
            break;

        case NativeTypeKind::Rune:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "rune"));
            break;

        case NativeTypeKind::F32:
            computedValue.reg.f32 = static_cast<float>(computedValue.reg.s64);
            break;
        case NativeTypeKind::F64:
            computedValue.reg.f64 = static_cast<double>(computedValue.reg.s64);
            break;

        default:
            return Fmt(Err(Err0318), computedValue.reg.u64);
        }

        break;

    case LiteralType::TT_U32:
    case LiteralType::TT_U64:
    case LiteralType::TT_UNTYPED_BINHEXA:
        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::U8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u8"));
            break;
        case NativeTypeKind::U16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u16"));
            break;
        case NativeTypeKind::U32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "u32"));
            break;
        case NativeTypeKind::U64:
            break;

        case NativeTypeKind::S8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "s8"));
            break;
        case NativeTypeKind::S16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "s16"));
            break;
        case NativeTypeKind::S32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "s32"));
            break;
        case NativeTypeKind::S64:
            if (computedValue.reg.u64 > (uint64_t) INT64_MAX + 1)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "s64"));
            break;

        case NativeTypeKind::Rune:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Fmt(Err(Err0425), computedValue.reg.u64, "rune"));
            break;

        default:
            return Fmt(Err(Err0318), computedValue.reg.u64);
        }

        break;
    default:
        break;
    }

    return "";
}

bool Semantic::resolveLiteralSuffix(SemanticContext* context)
{
    auto node = context->node;

    // Can be a predefined native type
    if (node->tokenId == TokenId::NativeType)
    {
        auto it = g_LangSpec->nativeTypes.find(node->token.text);
        SWAG_ASSERT(it);
        node->typeInfo = TypeManager::literalTypeToType(*it);
        return true;
    }

    // Search if identifier is a type
    if (node->tokenId == TokenId::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);

        // We do not want error. If identifier is not known, then we consider it as a 'user' suffix
        identifier->identifierRef()->flags |= AST_SILENT_CHECK;

        auto res = resolveIdentifier(context, identifier, RI_ZERO);
        YIELD();
        if (res && identifier->typeInfo)
            return true;

        SWAG_ASSERT(node->parent->parent && node->parent->parent->kind == AstNodeKind::Literal);
        node->parent->parent->semFlags |= SEMFLAG_LITERAL_SUFFIX;
        return true;
    }

    return Report::internalError(context->node, "resolveLiteralSuffix, invalid token");
}

bool Semantic::resolveLiteral(SemanticContext* context)
{
    auto node       = CastAst<AstLiteral>(context->node, AstNodeKind::Literal);
    auto sourceFile = context->sourceFile;

    switch (node->tokenId)
    {
    case TokenId::KwdTrue:
        node->tokenId        = TokenId::LiteralNumber;
        node->literalType    = LiteralType::TT_BOOL;
        node->literalValue.b = true;
        break;
    case TokenId::KwdFalse:
        node->tokenId        = TokenId::LiteralNumber;
        node->literalType    = LiteralType::TT_BOOL;
        node->literalValue.b = false;
        break;
    case TokenId::KwdNull:
        node->tokenId              = TokenId::LiteralNumber;
        node->literalType          = LiteralType::TT_NULL;
        node->literalValue.pointer = nullptr;
        break;
    case TokenId::CompilerFile:
        node->tokenId     = TokenId::LiteralString;
        node->literalType = LiteralType::TT_STRING;
        node->token.text  = sourceFile->path.string();
        break;
    case TokenId::CompilerModule:
        node->tokenId     = TokenId::LiteralString;
        node->literalType = LiteralType::TT_STRING;
        node->token.text  = sourceFile->module ? sourceFile->module->name : Utf8("?");
        break;
    case TokenId::CompilerLine:
        node->tokenId          = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_UNTYPED_INT;
        node->literalValue.u32 = node->token.startLocation.line + 1;
        break;
    case TokenId::CompilerBuildVersion:
        node->tokenId          = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_VERSION;
        break;
    case TokenId::CompilerBuildRevision:
        node->tokenId          = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_REVISION;
        break;
    case TokenId::CompilerBuildNum:
        node->tokenId          = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_NUM;
        break;
    default:
        break;
    }

    node->typeInfo = TypeManager::literalTypeToType(node->literalType, node->literalValue);
    node->setFlagsValueIsComputed();
    node->computedValue->reg  = node->literalValue;
    node->computedValue->text = node->token.text;
    node->flags |= AST_R_VALUE;

    SWAG_CHECK(processLiteralString(context));

    // Suffix
    auto suffix = node->childs.empty() ? nullptr : node->childs.front();
    if (!suffix || !suffix->typeInfo)
    {
        // If there's a suffix without a type, then this should be a 'user' suffix
        // Literal must have been flagged
        SWAG_ASSERT(!suffix || node->semFlags & SEMFLAG_LITERAL_SUFFIX);

        // Convert to unsigned int for a character without suffix
        if (node->tokenId == TokenId::LiteralCharacter)
        {
            auto errMsg = checkLiteralValue(*node->computedValue, node->literalType, node->literalValue, g_TypeMgr->typeInfoCharacter, false);
            if (!errMsg.empty())
                return context->report({node, errMsg});
            node->typeInfo    = g_TypeMgr->typeInfoCharacter;
            node->byteCodeFct = ByteCodeGen::emitLiteral;
            return true;
        }

        // By default, a float without a suffix is considered as f32 (not f64 like in C).
        if (node->typeInfo->isNative(NativeTypeKind::F32) && node->literalType == LiteralType::TT_UNTYPED_FLOAT)
            node->computedValue->reg.f32 = (float) node->literalValue.f64;
        return true;
    }

    // Check suffix type is correct (should be native)
    if (suffix->tokenId != TokenId::NativeType)
    {
        if (suffix->resolvedSymbolName && suffix->resolvedSymbolName->kind != SymbolKind::TypeAlias)
        {
            return context->report({suffix, Fmt(Err(Err0319), suffix->resolvedSymbolName->name.c_str(), Naming::aKindName(suffix->resolvedSymbolName->kind).c_str())});
        }
    }

    auto suffixType = TypeManager::concreteType(suffix->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(suffixType->isNative(), context->report({suffix, Fmt(Err(Err0320), suffixType->getDisplayNameC())}));

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
        return context->report({suffix, Fmt(Err(Err0320), suffix->typeInfo->getDisplayNameC())});
    }

    // Check if this is in fact a negative literal. This is important to know now, in order
    // to be able to correctly check bounds.
    bool negApplied = false;
    if (node->parent->kind == AstNodeKind::SingleOp && node->parent->tokenId == TokenId::SymMinus)
    {
        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            node->semFlags |= SEMFLAG_NEG_EATEN;
            negApplied = true;
            break;
        default:
            break;
        }
    }

    auto errMsg = checkLiteralValue(*node->computedValue, node->literalType, node->literalValue, suffixType, negApplied);
    if (!errMsg.empty())
    {
        Diagnostic  diag{node, node->token, errMsg};
        Diagnostic* note = nullptr;
        if (suffix && suffix->typeInfo)
            note = Diagnostic::note(suffix, Fmt(Nte(Nte0198), suffix->typeInfo->getDisplayNameC()));
        return context->report(diag, note);
    }

    node->typeInfo    = suffixType;
    node->byteCodeFct = ByteCodeGen::emitLiteral;
    return true;
}
