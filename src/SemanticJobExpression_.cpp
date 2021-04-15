#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "TypeManager.h"

bool SemanticJob::convertTokenLiteral(SemanticContext* context, Token& token)
{
    if (token.literalCastedType == LiteralType::TT_MAX)
        return true;

    auto node = context->node;
    switch (token.literalType)
    {
    case LiteralType::TT_F32:
    case LiteralType::TT_F64:
        switch (token.literalCastedType)
        {
        case LiteralType::TT_F32:
        case LiteralType::TT_F64:
            break;
        default:
            return context->report({node, format("can't convert floating point number '%Lf'", token.literalValue.f64)});
        }
        break;

    case LiteralType::TT_S32:
    case LiteralType::TT_S64:
    case LiteralType::TT_INT:
        switch (token.literalCastedType)
        {
        case LiteralType::TT_U8:
            if (token.literalValue.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u8'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U16:
            if (token.literalValue.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u16'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U32:
            if (token.literalValue.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u32'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U64:
        case LiteralType::TT_UINT:
            break;

        case LiteralType::TT_S8:
            if (token.literalValue.s64 < INT8_MIN || token.literalValue.s64 > INT8_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's8'", token.literalValue.s64)});
            break;
        case LiteralType::TT_S16:
            if (token.literalValue.s64 < INT16_MIN || token.literalValue.s64 > INT16_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's16'", token.literalValue.s64)});
            break;
        case LiteralType::TT_S32:
            if (token.literalValue.s64 < INT32_MIN || token.literalValue.s64 > INT32_MAX)
                return context->report({node, format("literal number '%I64d' is not in the range of 's32'", token.literalValue.s64)});
            break;
        case LiteralType::TT_S64:
        case LiteralType::TT_INT:
            break;

        case LiteralType::TT_CHAR:
            if (token.literalValue.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'char'", token.literalValue.u64)});
            break;

        case LiteralType::TT_F32:
        {
            float   tmpF = static_cast<float>(token.literalValue.s64);
            int64_t tmp  = static_cast<int64_t>(tmpF);
            if (tmp != token.literalValue.s64)
                return context->report({node, format("value '%I64d' is truncated in 'f32'", token.literalValue.s64)});
            token.literalValue.f64 = tmpF;
            break;
        }
        case LiteralType::TT_F64:
        {
            double  tmpF = static_cast<double>(token.literalValue.s64);
            int64_t tmp  = static_cast<int64_t>(tmpF);
            if (tmp != token.literalValue.s64)
                return context->report({node, format("value '%I64d' is truncated in 'f64'", token.literalValue.s64)});
            token.literalValue.f64 = tmpF;
            break;
        }

        default:
            return context->report({node, format("invalid literal number conversion of '%I64u'", token.literalValue.u64)});
        }

        break;

    case LiteralType::TT_U32:
    case LiteralType::TT_U64:
    case LiteralType::TT_UINT:
        switch (token.literalCastedType)
        {
        case LiteralType::TT_U8:
            if (token.literalValue.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u8'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U16:
            if (token.literalValue.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u16'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U32:
            if (token.literalValue.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'u32'", token.literalValue.u64)});
            break;
        case LiteralType::TT_U64:
        case LiteralType::TT_UINT:
            break;

        case LiteralType::TT_S8:
            if (token.literalValue.u64 > UINT8_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's8'", token.literalValue.u64)});
            break;
        case LiteralType::TT_S16:
            if (token.literalValue.u64 > UINT16_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's16'", token.literalValue.u64)});
            break;
        case LiteralType::TT_S32:
            if (token.literalValue.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 's32'", token.literalValue.u64)});
            break;
        case LiteralType::TT_S64:
        case LiteralType::TT_INT:
            if (token.literalValue.u64 > (uint64_t) INT64_MAX + 1)
                return context->report({node, format("literal number '%I64u' is not in the range of 's64'", token.literalValue.u64)});
            break;

        case LiteralType::TT_CHAR:
            if (token.literalValue.u64 > UINT32_MAX)
                return context->report({node, format("literal number '%I64u' is not in the range of 'char'", token.literalValue.u64)});
            break;

        default:
            return context->report({node, format("invalid literal number conversion of '%I64u'", token.literalValue.u64)});
        }

        break;
    }

    token.literalType       = token.literalCastedType;
    token.literalCastedType = LiteralType::TT_MAX;
    return true;
}

void SemanticJob::putLiteralInNode(AstNode* node)
{
    node->typeInfo           = TypeManager::literalTypeToType(node->token);
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    node->setFlagsValueIsComputed();

    // Convert to 32 bits now
    if (node->token.literalType == LiteralType::TT_F32 || node->token.literalType == LiteralType::TT_UNTYPED_FLOAT)
        node->computedValue.reg.f32 = (float) node->computedValue.reg.f64;
}

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = context->node;
    SWAG_CHECK(convertTokenLiteral(context, node->token));
    node->byteCodeFct = ByteCodeGenJob::emitLiteral;
    putLiteralInNode(node);
    node->flags |= AST_R_VALUE;
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
        if ((typeInfo->flags & TYPEINFO_INTEGER) || (typeInfo->flags & TYPEINFO_FLOAT) || typeInfo->isNative(NativeTypeKind::Char))
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
                 !typeInfo->isNative(NativeTypeKind::Char) &&
                 typeInfo->kind != TypeInfoKind::Pointer &&
                 typeInfo->kind != TypeInfoKind::Interface &&
                 !(typeInfo->flags & TYPEINFO_INTEGER) &&
                 !(typeInfo->flags & TYPEINFO_FLOAT) &&
                 typeInfo->kind != TypeInfoKind::Lambda)
        {
            return context->report({expression, format("cannot use operator '??' on type '%s'", typeInfo->name.c_str())});
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