#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Version.h"
#include "LanguageSpec.h"
#include "Ast.h"

bool SemanticJob::getDigitHexa(SemanticContext* context, const char** _pz, int& result, const char* errMsg)
{
    auto node = context->node;
    auto pz   = *_pz;
    auto c    = *pz;
    (*_pz)++;

    if (!SWAG_IS_HEX(c))
    {
        auto loc = node->token.startLocation;
        loc.column += (uint32_t) (pz - node->computedValue->text.c_str());
        if (c == '"')
            return context->report({node->sourceFile, loc, Fmt(Err(Err0158), errMsg)});
        else
            return context->report({node->sourceFile, loc, Fmt(Err(Err0174), c, errMsg)});
    }

    if (c >= 'a' && c <= 'z')
        result = 10 + (c - 'a');
    else if (c >= 'A' && c <= 'Z')
        result = 10 + (c - 'A');
    else
        result = c - '0';
    return true;
}

bool SemanticJob::processLiteralString(SemanticContext* context)
{
    auto node = CastAst<AstLiteral>(context->node, AstNodeKind::Literal);
    if (node->literalType != LiteralType::TT_ESCAPE_STRING)
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

        if (c != '\\')
        {
            result.append(c);
            continue;
        }

        c = *pz++;
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
        case 'x':
        {
            int  c1, c2;
            auto msg = Err(Err0184);
            SWAG_CHECK(getDigitHexa(context, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c2, msg));
            char32_t cw = (c1 << 4) + c2;
            result.append((char) cw);
            continue;
        }
        case 'u':
        {
            int  c1, c2, c3, c4;
            auto msg = Err(Err0224);
            SWAG_CHECK(getDigitHexa(context, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c2, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c3, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c4, msg));
            char32_t cw = (c1 << 12) + (c2 << 8) + (c3 << 4) + c4;
            result.append(cw);
            continue;
        }
        case 'U':
        {
            int  c1, c2, c3, c4, c5, c6, c7, c8;
            auto msg = Err(Err0253);
            SWAG_CHECK(getDigitHexa(context, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c2, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c3, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c4, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c5, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c6, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c7, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c8, msg));
            char32_t cw = (c1 << 28) + (c2 << 24) + (c3 << 20) + (c4 << 16) + (c5 << 12) + (c6 << 8) + (c7 << 4) + c8;
            result.append(cw);
            continue;
        }
        }

        loc.column += (uint32_t) (pz - start);
        return context->report({node->sourceFile, loc, Fmt(Err(Err0259), c)});
    }

    node->computedValue->text = result;
    return true;
}

Utf8 SemanticJob::checkLiteralValue(ComputedValue& computedValue, LiteralType& literalType, Register& literalValue, TypeInfo* typeSuffix, bool negApplied)
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
        }
    }

    switch (literalType)
    {
    case LiteralType::TT_BOOL:
        if (typeSuffix->isNativeInteger())
        {
            computedValue.reg.u64 = computedValue.reg.b;
            break;
        }

        if (typeSuffix->nativeType != NativeTypeKind::Bool)
            return Fmt(Err(Err0261), typeSuffix->getDisplayNameC());
        break;

    case LiteralType::TT_RAW_STRING:
    case LiteralType::TT_ESCAPE_STRING:
    case LiteralType::TT_STRING:
    {
        VectorNative<uint32_t> uni;
        computedValue.text.toUni32(uni);
        if (uni.size() != 1)
            return Fmt(Err(Err0262), computedValue.text.c_str());

        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::Rune:
            computedValue.reg.ch = uni[0];
            break;

        case NativeTypeKind::U8:
            if (uni[0] > UINT8_MAX)
                return Fmt(Err(Err0263), uni[0]);
            computedValue.reg.u8 = (uint8_t) uni[0];
            break;

        case NativeTypeKind::U16:
            if (uni[0] > UINT16_MAX)
                return Fmt(Err(Err0287), uni[0]);
            computedValue.reg.u16 = (uint16_t) uni[0];
            break;

        case NativeTypeKind::U32:
            computedValue.reg.u32 = uni[0];
            break;

        case NativeTypeKind::U64:
            computedValue.reg.u64 = uni[0];
            break;

        default:
            return Fmt(Err(Err0302), typeSuffix->getDisplayNameC());
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
            return Fmt(Err(Err0317), typeSuffix->getDisplayNameC());
        }
        break;

    case LiteralType::TT_S32:
    case LiteralType::TT_S64:
    case LiteralType::TT_UNTYPED_INT:
        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::U8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Err(Err0341), computedValue.reg.u64);
            break;
        case NativeTypeKind::U16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Err(Err0357), computedValue.reg.u64);
            break;
        case NativeTypeKind::U32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Err(Err0358), computedValue.reg.u64);
            break;
        case NativeTypeKind::U64:
            break;

        case NativeTypeKind::S8:
            if (computedValue.reg.s64 < INT8_MIN || computedValue.reg.s64 > INT8_MAX)
                return Fmt(Err(Err0359), computedValue.reg.s64);
            break;
        case NativeTypeKind::S16:
            if (computedValue.reg.s64 < INT16_MIN || computedValue.reg.s64 > INT16_MAX)
                return Fmt(Err(Err0360), computedValue.reg.s64);
            break;
        case NativeTypeKind::S32:
            if (computedValue.reg.s64 < INT32_MIN || computedValue.reg.s64 > INT32_MAX)
                return Fmt(Err(Err0361), computedValue.reg.s64);
            break;
        case NativeTypeKind::S64:
            break;

        case NativeTypeKind::Rune:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Err(Err0362), computedValue.reg.u64);
            break;

        case NativeTypeKind::F32:
            computedValue.reg.f32 = static_cast<float>(computedValue.reg.s64);
            break;
        case NativeTypeKind::F64:
            computedValue.reg.f64 = static_cast<double>(computedValue.reg.s64);
            break;

        default:
            return Fmt(Err(Err0387), computedValue.reg.u64);
        }

        break;

    case LiteralType::TT_U32:
    case LiteralType::TT_U64:
    case LiteralType::TT_UNTYPED_BINHEXA:
        switch (typeSuffix->nativeType)
        {
        case NativeTypeKind::U8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Err(Err0341), computedValue.reg.u64);
            break;
        case NativeTypeKind::U16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Err(Err0357), computedValue.reg.u64);
            break;
        case NativeTypeKind::U32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Err(Err0358), computedValue.reg.u64);
            break;
        case NativeTypeKind::U64:
            break;

        case NativeTypeKind::S8:
            if (computedValue.reg.u64 > UINT8_MAX)
                return Fmt(Err(Err0415), computedValue.reg.u64);
            break;
        case NativeTypeKind::S16:
            if (computedValue.reg.u64 > UINT16_MAX)
                return Fmt(Err(Err0422), computedValue.reg.u64);
            break;
        case NativeTypeKind::S32:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Err(Err0429), computedValue.reg.u64);
            break;
        case NativeTypeKind::S64:
            if (computedValue.reg.u64 > (uint64_t) INT64_MAX + 1)
                return Fmt(Err(Err0430), computedValue.reg.u64);
            break;

        case NativeTypeKind::Rune:
            if (computedValue.reg.u64 > UINT32_MAX)
                return Fmt(Err(Err0362), computedValue.reg.u64);
            break;

        default:
            return Fmt(Err(Err0387), computedValue.reg.u64);
        }

        break;
    }

    return "";
}

bool SemanticJob::resolveLiteralSuffix(SemanticContext* context)
{
    auto node = context->node;

    // Can be a predefined native type
    if (node->token.id == TokenId::NativeType)
    {
        auto it = g_LangSpec->nativeTypes.find(node->token.text);
        SWAG_ASSERT(it);
        node->typeInfo = TypeManager::literalTypeToType(*it);
        return true;
    }

    // Search if identifier is a type
    if (node->token.id == TokenId::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);

        // We do not want error. If identifier is not known, then we consider it as a 'user' suffix
        identifier->identifierRef->flags |= AST_SILENT_CHECK;

        auto res = resolveIdentifier(context, identifier, RI_ZERO);
        if (context->result != ContextResult::Done)
            return true;
        if (res && identifier->typeInfo)
            return true;

        SWAG_ASSERT(node->parent->parent && node->parent->parent->kind == AstNodeKind::Literal);
        node->parent->parent->semFlags |= AST_SEM_LITERAL_SUFFIX;
        return true;
    }

    return Report::internalError(context->node, "resolveLiteralSuffix, invalid token");
}

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto   node       = CastAst<AstLiteral>(context->node, AstNodeKind::Literal);
    auto   sourceFile = context->sourceFile;
    Token& token      = node->token;

    switch (token.id)
    {
    case TokenId::KwdTrue:
        token.id             = TokenId::LiteralNumber;
        node->literalType    = LiteralType::TT_BOOL;
        node->literalValue.b = true;
        break;
    case TokenId::KwdFalse:
        token.id             = TokenId::LiteralNumber;
        node->literalType    = LiteralType::TT_BOOL;
        node->literalValue.b = false;
        break;
    case TokenId::KwdNull:
        token.id                   = TokenId::LiteralNumber;
        node->literalType          = LiteralType::TT_NULL;
        node->literalValue.pointer = nullptr;
        break;
    case TokenId::CompilerFile:
        token.id          = TokenId::LiteralString;
        node->literalType = LiteralType::TT_STRING;
        token.text        = sourceFile->path;
        break;
    case TokenId::CompilerModule:
        token.id          = TokenId::LiteralString;
        node->literalType = LiteralType::TT_STRING;
        token.text        = sourceFile->module ? sourceFile->module->name : Utf8("?");
        break;
    case TokenId::CompilerLine:
        token.id               = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_UNTYPED_INT;
        node->literalValue.u32 = token.startLocation.line + 1;
        break;
    case TokenId::CompilerBuildVersion:
        token.id               = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_VERSION;
        break;
    case TokenId::CompilerBuildRevision:
        token.id               = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_REVISION;
        break;
    case TokenId::CompilerBuildNum:
        token.id               = TokenId::LiteralNumber;
        node->literalType      = LiteralType::TT_S32;
        node->literalValue.s32 = SWAG_BUILD_NUM;
        break;
    }

    node->typeInfo = TypeManager::literalTypeToType(node->literalType, node->literalValue);
    node->setFlagsValueIsComputed();
    node->computedValue->reg  = node->literalValue;
    node->computedValue->text = node->token.text;
    node->flags |= AST_R_VALUE;

    processLiteralString(context);

    // Suffix
    auto suffix = node->childs.empty() ? nullptr : node->childs.front();
    if (!suffix || !suffix->typeInfo)
    {
        // If there's a suffix without a type, then this should be a 'user' suffix
        // Literal must have been flagged
        SWAG_ASSERT(!suffix || node->semFlags & AST_SEM_LITERAL_SUFFIX);

        // By default, a float without a suffix is considered as f32 (not f64 like in C).
        if (node->typeInfo->isNative(NativeTypeKind::F32) && node->literalType == LiteralType::TT_UNTYPED_FLOAT)
            node->computedValue->reg.f32 = (float) node->literalValue.f64;
        return true;
    }

    SWAG_VERIFY(suffix->typeInfo->isNative(), context->report({suffix, Fmt(Err(Err0437), suffix->typeInfo->getDisplayNameC())}));

    switch (suffix->typeInfo->nativeType)
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
        return context->report({suffix, Fmt(Err(Err0439), suffix->typeInfo->getDisplayNameC())});
    }

    // Check if this is in fact a negative literal. This is important to know now, in order
    // to be able to correctly check bounds.
    bool negApplied = false;
    if (node->parent->kind == AstNodeKind::SingleOp && node->parent->token.id == TokenId::SymMinus)
    {
        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            node->doneFlags |= AST_DONE_NEG_EATEN;
            negApplied = true;
            break;
        }
    }

    auto errMsg = checkLiteralValue(*node->computedValue, node->literalType, node->literalValue, suffix->typeInfo, negApplied);
    if (!errMsg.empty())
        return context->report({node, errMsg});

    node->typeInfo    = suffix->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitLiteral;
    return true;
}
