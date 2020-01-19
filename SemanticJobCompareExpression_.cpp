#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeTable.h"

bool SemanticJob::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

    if ((left->flags & AST_VALUE_IS_TYPEINFO) && (right->flags & AST_VALUE_IS_TYPEINFO))
    {
        node->flags |= AST_VALUE_COMPUTED;
        node->computedValue.reg.b = left->typeInfo == right->typeInfo;
    }
    else if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b == right->computedValue.reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 == right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 == right->computedValue.reg.f64;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue.reg.b = left->computedValue.reg.u8 == right->computedValue.reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue.reg.b = left->computedValue.reg.u16 == right->computedValue.reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.b = left->computedValue.reg.u32 == right->computedValue.reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            node->computedValue.reg.b = left->computedValue.reg.u64 == right->computedValue.reg.u64;
            break;
        case NativeTypeKind::String:
            node->computedValue.reg.b = left->computedValue.text == right->computedValue.text;
            break;

        default:
            return context->report({context->node, format("compare operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opEquals", nullptr, nullptr, left, right, false));
        node->typeInfo = g_TypeMgr.typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b < right->computedValue.reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 < right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 < right->computedValue.reg.f64;
            break;
        case NativeTypeKind::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 < right->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 < right->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 < right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 < right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 < right->computedValue.reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 < right->computedValue.reg.u16;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 < right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 < right->computedValue.reg.u64;
            break;

        default:
            return context->report({context->node, format("compare operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opCmp", nullptr, nullptr, left, right, false));
        node->typeInfo = g_TypeMgr.typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b > right->computedValue.reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 > right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 > right->computedValue.reg.f64;
            break;
        case NativeTypeKind::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 > right->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 > right->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 > right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 > right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 > right->computedValue.reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 > right->computedValue.reg.u16;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 > right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 > right->computedValue.reg.u64;
            break;

        default:
            return context->report({context->node, format("compare operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opCmp", nullptr, nullptr, left, right, false));
        node->typeInfo = g_TypeMgr.typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    if ((leftTypeInfo->kind == TypeInfoKind::Lambda || leftTypeInfo == g_TypeMgr.typeInfoNull) &&
        (rightTypeInfo->kind == TypeInfoKind::Lambda || rightTypeInfo == g_TypeMgr.typeInfoNull) &&
        (node->token.id == TokenId::SymEqualEqual || node->token.id == TokenId::SymExclamEqual))
    {
        // This is fine to compare two lambdas
    }
    else if (leftTypeInfo->kind != TypeInfoKind::Native &&
             leftTypeInfo->kind != TypeInfoKind::Pointer &&
             leftTypeInfo->kind != TypeInfoKind::Struct &&
             leftTypeInfo->kind != TypeInfoKind::Slice &&
             leftTypeInfo->kind != TypeInfoKind::Interface)
    {
        return context->report({left, format("operation '%s' not allowed on %s '%s'", node->token.text.c_str(), TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
    }
    else if (rightTypeInfo->kind != TypeInfoKind::Native &&
             rightTypeInfo->kind != TypeInfoKind::Pointer &&
             rightTypeInfo->kind != TypeInfoKind::Struct)
    {
        return context->report({right, format("operation '%s' not allowed on %s '%s'", node->token.text.c_str(), TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())});
    }

    // Slice can only be compared to null
    if (leftTypeInfo->kind == TypeInfoKind::Slice && rightTypeInfo != g_TypeMgr.typeInfoNull)
    {
        return context->report({left, "a slice can only be compared to 'null'"});
    }

    // Some types can only be compared for equality
    if (leftTypeInfo->kind == TypeInfoKind::Slice || leftTypeInfo->kind == TypeInfoKind::Interface)
    {
        if (node->token.id != TokenId::SymEqualEqual && node->token.id != TokenId::SymExclamEqual)
        {
            return context->report({left, format("operation '%s' not allowed on %s '%s'", node->token.text.c_str(), TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
        }
    }

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    node->typeInfo = g_TypeMgr.typeInfoBool;
    TypeManager::promote(left, right);
    left->typeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_ENUM);
    right->typeInfo = TypeManager::concreteType(right->typeInfo, CONCRETE_ENUM);

    // Must not make types compatible for a struct, as we can compare a struct with whatever other type in
    // a opCmp operator
    if (left->typeInfo->kind != TypeInfoKind::Struct && right->typeInfo->kind != TypeInfoKind::Struct)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_BIJECTIF | CASTFLAG_FORCE_UNCONST));
    }

    node->byteCodeFct = ByteCodeGenJob::emitCompareOp;
    node->inheritAndFlag(AST_CONST_EXPR);
    node->inheritAndFlag(AST_R_VALUE);

    switch (node->token.id)
    {
    case TokenId::SymEqualEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        break;
    case TokenId::SymExclamEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    case TokenId::SymLower:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        break;
    case TokenId::SymGreater:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        break;
    case TokenId::SymLowerEqual:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    case TokenId::SymGreaterEqual:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    default:
        return internalError(context, "resolveCompareExpression, token not supported");
    }

    return true;
}
