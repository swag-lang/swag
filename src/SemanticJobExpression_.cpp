#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

bool SemanticJob::getDigitHexa(SemanticContext* context, const char** _pz, int& result, const char* errMsg)
{
    auto node = context->node;
    auto pz   = *_pz;
    auto c    = *pz;
    (*_pz)++;

    if (!SWAG_IS_HEX(c))
    {
        auto loc = node->token.startLocation;
        loc.column += (uint32_t)(pz - node->computedValue.text.c_str());
        if (c == '"')
            return context->report({node->sourceFile, loc, format("not enough hexadecimal digit, %s", errMsg)});
        else
            return context->report({node->sourceFile, loc, format("invalid hexadecimal digit '%c', %s", c, errMsg)});
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
    auto node = context->node;
    if (node->token.literalType != LiteralType::TT_ESCAPE_STRING)
        return true;

    auto loc = node->token.startLocation;
    Utf8 result;
    auto len = node->computedValue.text.length();
    result.reserve(len);

    auto start = node->computedValue.text.c_str();
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
            int         c1, c2;
            const char* msg = "'\\x' escape code needs 2 of them";
            SWAG_CHECK(getDigitHexa(context, &pz, c1, msg));
            SWAG_CHECK(getDigitHexa(context, &pz, c2, msg));
            char32_t cw = (c1 << 4) + c2;
            result.append((char) cw);
            continue;
        }
        case 'u':
        {
            int         c1, c2, c3, c4;
            const char* msg = "'\\u' escape code needs 4 of them";
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
            int         c1, c2, c3, c4, c5, c6, c7, c8;
            const char* msg = "'\\U' escape code needs 8 of them";
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

        loc.column += (uint32_t)(pz - start) - 1;
        return context->report({node->sourceFile, loc, format("unrecognized character escape sequence '%c'", c)});
    }

    node->computedValue.text = result;
    return true;
}

bool SemanticJob::resolveLiteralSuffix(SemanticContext* context)
{
    auto   node  = context->node;
    Token& token = node->token;

    node->typeInfo           = TypeManager::literalTypeToType(node->token);
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    node->setFlagsValueIsComputed();
    node->flags |= AST_R_VALUE;

    processLiteralString(context);

    // No suffix
    if (node->childs.empty())
    {
        // By default, a float without a suffix is considered as f32 (not f64 like in C).
        if (node->typeInfo->isNative(NativeTypeKind::F32) && token.literalType == LiteralType::TT_UNTYPED_FLOAT)
            node->computedValue.reg.f32 = (float) token.literalValue.f64;
        return true;
    }

    auto suffix = node->childs.front();
    SWAG_ASSERT(suffix->typeInfo);
    SWAG_VERIFY(suffix->typeInfo->kind == TypeInfoKind::Native, context->report({suffix, format("literal suffix type must be native ('%s' provided)", suffix->typeInfo->getDisplayName().c_str())}));

    switch (suffix->typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::UInt:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
        break;
    default:
        return context->report({suffix, format("invalid literal suffix type ('%s')", suffix->typeInfo->getDisplayName().c_str())});
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
            node->computedValue.reg.s64 = -node->computedValue.reg.s64;
            node->doneFlags |= AST_DONE_NEG_EATEN;
            negApplied = true;
            break;
        }

        if (negApplied)
        {
            switch (token.literalType)
            {
            case LiteralType::TT_U32:
            case LiteralType::TT_UNTYPED_BINHEXA:
            case LiteralType::TT_UNTYPED_INT:
                token.literalType = LiteralType::TT_S32;
                break;
            case LiteralType::TT_U64:
            case LiteralType::TT_UINT:
                token.literalType = LiteralType::TT_S64;
                break;
            }
        }
    }

    switch (token.literalType)
    {
    case LiteralType::TT_RAW_STRING:
    case LiteralType::TT_ESCAPE_STRING:
    case LiteralType::TT_STRING:
    {
        VectorNative<uint32_t> uni;
        node->computedValue.text.toUni32(uni);
        SWAG_VERIFY(uni.size() == 1, context->report({node, node->token, format("invalid character literal '%s'; this is a string, not a character", token.text.c_str())}));

        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::Rune:
            node->computedValue.reg.ch = uni[0];
            break;

        case NativeTypeKind::U8:
            SWAG_VERIFY(uni[0] <= UINT8_MAX, context->report({suffix, format("cannot convert character literal to 'u8', value '%u' is too big", uni[0])}));
            node->computedValue.reg.u8 = (uint8_t) uni[0];
            break;

        case NativeTypeKind::U16:
            SWAG_VERIFY(uni[0] <= UINT16_MAX, context->report({suffix, format("cannot convert character literal to 'u16', value '%u' is too big", uni[0])}));
            node->computedValue.reg.u16 = (uint16_t) uni[0];
            break;

        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = uni[0];
            break;

        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = uni[0];
            break;

        default:
            context->report({suffix, format("cannot convert from 'string' to '%s'", suffix->typeInfo->getDisplayName().c_str())});
            break;
        }
        break;
    }

    case LiteralType::TT_UNTYPED_FLOAT:
        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = (float) token.literalValue.f64;
            break;
        case NativeTypeKind::F64:
            break;
        default:
            return context->report({node, format("cannot convert floating point number '%Lf'", token.literalValue.f64)});
        }
        break;

    case LiteralType::TT_S32:
    case LiteralType::TT_S64:
    case LiteralType::TT_INT:
    case LiteralType::TT_UNTYPED_INT:
        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            if (node->computedValue.reg.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u8'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U16:
            if (node->computedValue.reg.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u16'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U32:
            if (node->computedValue.reg.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u32'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            break;

        case NativeTypeKind::S8:
            if (node->computedValue.reg.s64 < INT8_MIN || node->computedValue.reg.s64 > INT8_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's8'", node->computedValue.reg.s64)});
            break;
        case NativeTypeKind::S16:
            if (node->computedValue.reg.s64 < INT16_MIN || node->computedValue.reg.s64 > INT16_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's16'", node->computedValue.reg.s64)});
            break;
        case NativeTypeKind::S32:
            if (node->computedValue.reg.s64 < INT32_MIN || node->computedValue.reg.s64 > INT32_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's32'", node->computedValue.reg.s64)});
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            break;

        case NativeTypeKind::Rune:
            if (node->computedValue.reg.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'rune'", node->computedValue.reg.u64)});
            break;

        case NativeTypeKind::F32:
        {
            float   tmpF = static_cast<float>(node->computedValue.reg.s64);
            int64_t tmp  = static_cast<int64_t>(tmpF);
            if (tmp != node->computedValue.reg.s64)
                return context->report({node, format("value '%I64d' is truncated in 'f32'", node->computedValue.reg.s64)});
            node->computedValue.reg.f32 = tmpF;
            break;
        }
        case NativeTypeKind::F64:
        {
            double  tmpF = static_cast<double>(node->computedValue.reg.s64);
            int64_t tmp  = static_cast<int64_t>(tmpF);
            if (tmp != node->computedValue.reg.s64)
                return context->report({node, format("value '%I64d' is truncated in 'f64'", node->computedValue.reg.s64)});
            node->computedValue.reg.f64 = tmpF;
            break;
        }

        default:
            return context->report({node, format("invalid literal number conversion of '%I64u'", node->computedValue.reg.u64)});
        }

        break;

    case LiteralType::TT_U32:
    case LiteralType::TT_U64:
    case LiteralType::TT_UINT:
    case LiteralType::TT_UNTYPED_BINHEXA:
        switch (suffix->typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            if (node->computedValue.reg.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u8'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U16:
            if (node->computedValue.reg.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u16'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U32:
            if (node->computedValue.reg.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u32'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            break;

        case NativeTypeKind::S8:
            if (node->computedValue.reg.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's8'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::S16:
            if (node->computedValue.reg.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's16'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::S32:
            if (node->computedValue.reg.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's32'", node->computedValue.reg.u64)});
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (node->computedValue.reg.u64 > (uint64_t) INT64_MAX + 1)
                return context->report({node, format("literal number '%I64u' is not in the range of 's64'", node->computedValue.reg.u64)});
            break;

        case NativeTypeKind::Rune:
            if (node->computedValue.reg.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'rune'", node->computedValue.reg.u64)});
            break;

        default:
            return context->report({node, format("invalid literal number conversion of '%I64u'", node->computedValue.reg.u64)});
        }

        break;
    }

    node->typeInfo = suffix->typeInfo;
    return true;
}

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = context->node;
    SWAG_CHECK(resolveLiteralSuffix(context));
    node->byteCodeFct = ByteCodeGenJob::emitLiteral;
    return true;
}

bool SemanticJob::resolveExplicitNoInit(SemanticContext* context)
{
    auto node = context->node;
    node->parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
    node->flags |= AST_CONST_EXPR;
    node->typeInfo = g_TypeMgr.typeInfoVoid;
    return true;
}

bool SemanticJob::computeExpressionListTupleType(SemanticContext* context, AstNode* node)
{
    auto typeInfo    = allocType<TypeInfoList>();
    typeInfo->kind   = TypeInfoKind::TypeListTuple;
    typeInfo->name   = "{";
    typeInfo->sizeOf = 0;

    int idx = 0;
    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));

        if (!typeInfo->subTypes.empty())
            typeInfo->name += ", ";

        auto typeParam      = allocType<TypeInfoParam>();
        typeParam->typeInfo = child->typeInfo;
        typeInfo->subTypes.push_back(typeParam);

        // Value has been named
        if (!child->token.text.empty() && (child->flags & AST_IS_NAMED))
        {
            typeInfo->name += child->token.text;
            typeInfo->name += ": ";
            typeParam->namedParam = child->token.text;
        }

        typeInfo->name += child->typeInfo->name;

        if (child->castOffset)
            typeInfo->sizeOf += child->castOffset;
        else
            typeInfo->sizeOf += child->typeInfo->sizeOf;

        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;

        idx++;
    }

    typeInfo->name += "}";
    node->typeInfo = typeInfo;
    return true;
}

bool SemanticJob::resolveExpressionListTuple(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    SWAG_CHECK(computeExpressionListTupleType(context, node));

    node->allocateExtension();
    node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct                  = ByteCodeGenJob::emitExpressionList;

    // If the literal tuple is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the tuple will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->computedValue.reg.offset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::resolveExpressionListArray(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo  = allocType<TypeInfoList>();
    typeInfo->kind = TypeInfoKind::TypeListArray;
    SWAG_ASSERT(node->childs.size());
    typeInfo->name = format("[%u] %s", node->childs.size(), node->childs.front()->typeInfo->name.c_str());

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));
        auto typeParam      = allocType<TypeInfoParam>();
        typeParam->typeInfo = child->typeInfo;
        typeInfo->subTypes.push_back(typeParam);
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    node->allocateExtension();
    node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct                  = ByteCodeGenJob::emitExpressionList;
    node->typeInfo                     = typeInfo;

    // If the literal array is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the array will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->computedValue.reg.offset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if ((node->flags & AST_CONST_EXPR) &&
        node->typeInfo->kind != TypeInfoKind::TypeListArray &&
        node->typeInfo->kind != TypeInfoKind::TypeListTuple &&
        node->typeInfo->kind != TypeInfoKind::Slice)
    {
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(executeNode(context, node, true));
        if (context->result == ContextResult::Pending)
            return true;
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node2));
    if (context->result == ContextResult::Pending)
        return true;
    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2, AstNode* node3)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node2));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node3));
    if (context->result == ContextResult::Pending)
        return true;
    return true;
}

bool SemanticJob::resolveConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 3);

    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    auto ifFalse    = node->childs[2];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifTrue));
    SWAG_CHECK(checkIsConcrete(context, ifFalse));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifTrue, ifFalse));
    if (context->result == ContextResult::Pending)
        return true;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, expression, CASTFLAG_AUTO_BOOL));
    if (expression->flags & AST_VALUE_COMPUTED)
    {
        node->childs.clear();

        if (expression->computedValue.reg.b)
        {
            node->inheritComputedValue(ifTrue);
            node->typeInfo = ifTrue->typeInfo;
            node->childs.push_back(ifTrue);
        }
        else
        {
            node->inheritComputedValue(ifFalse);
            node->typeInfo = ifFalse->typeInfo;
            node->childs.push_back(ifFalse);
        }

        node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
        return true;
    }
    else
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, ifFalse, ifTrue, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));
        node->typeInfo = ifTrue->typeInfo;
    }

    expression->allocateExtension();
    expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitConditionalOpAfterExpr;
    ifTrue->allocateExtension();
    ifTrue->extension->byteCodeAfterFct = ByteCodeGenJob::emitConditionalOpAfterIfTrue;
    node->byteCodeFct                   = ByteCodeGenJob::emitConditionalOp;
    return true;
}

bool SemanticJob::resolveNullConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 2);

    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifTrue));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifTrue));
    if (context->result == ContextResult::Pending)
        return true;

    auto typeInfo = TypeManager::concreteReferenceType(expression->typeInfo);

    if (expression->flags & AST_VALUE_COMPUTED)
    {
        bool notNull = true;
        if ((typeInfo->flags & TYPEINFO_INTEGER) || (typeInfo->flags & TYPEINFO_FLOAT) || typeInfo->isNative(NativeTypeKind::Rune))
        {
            switch (typeInfo->sizeOf)
            {
            case 1:
                notNull = expression->computedValue.reg.u8 != 0;
                break;
            case 2:
                notNull = expression->computedValue.reg.u16 != 0;
                break;
            case 4:
                notNull = expression->computedValue.reg.u32 != 0;
                break;
            case 8:
                notNull = expression->computedValue.reg.u64 != 0;
                break;
            }
        }

        if (notNull)
        {
            node->inheritComputedValue(expression);
            node->typeInfo = expression->typeInfo;
        }
        else
        {
            node->inheritComputedValue(ifTrue);
            node->typeInfo = ifTrue->typeInfo;
        }
    }
    else
    {
        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            SWAG_CHECK(resolveUserOp(context, "opData", nullptr, nullptr, expression, nullptr, false));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else if (!typeInfo->isNative(NativeTypeKind::String) &&
                 !typeInfo->isNative(NativeTypeKind::Rune) &&
                 typeInfo->kind != TypeInfoKind::Pointer &&
                 typeInfo->kind != TypeInfoKind::Interface &&
                 !(typeInfo->flags & TYPEINFO_INTEGER) &&
                 !(typeInfo->flags & TYPEINFO_FLOAT) &&
                 typeInfo->kind != TypeInfoKind::Lambda)
        {
            return context->report({expression, format("cannot use operator '??' on type '%s'", typeInfo->getDisplayName().c_str())});
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, expression, ifTrue, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));

        node->typeInfo    = expression->typeInfo;
        node->byteCodeFct = ByteCodeGenJob::emitNullConditionalOp;
    }

    return true;
}

bool SemanticJob::resolveDefer(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = ByteCodeGenJob::emitDefer;

    SWAG_ASSERT(node->childs.size() == 1);
    auto expr = node->childs.front();
    expr->flags |= AST_NO_BYTECODE;

    return true;
}