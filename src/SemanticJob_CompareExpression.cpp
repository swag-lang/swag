#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeTable.h"
#include "Runtime.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool SemanticJob::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    // Compile time compare of two types
    if ((left->flags & AST_VALUE_IS_TYPEINFO) && (right->flags & AST_VALUE_IS_TYPEINFO))
    {
        node->setFlagsValueIsComputed();
        SWAG_ASSERT(left->computedValue->storageOffset != 0xFFFFFFFF);
        SWAG_ASSERT(right->computedValue->storageOffset != 0xFFFFFFFF);
        SWAG_ASSERT(left->computedValue->storageSegment);
        SWAG_ASSERT(right->computedValue->storageSegment);

        if (left->computedValue->storageOffset == right->computedValue->storageOffset &&
            left->computedValue->storageSegment == right->computedValue->storageSegment)
        {
            node->computedValue->reg.b = true;
        }
        else
        {
            auto ptr1                  = (ConcreteTypeInfo*) left->computedValue->storageSegment->address(left->computedValue->storageOffset);
            auto ptr2                  = (ConcreteTypeInfo*) right->computedValue->storageSegment->address(right->computedValue->storageOffset);
            node->computedValue->reg.b = TypeManager::compareConcreteType(ptr1, ptr2);
        }
    }
    else if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.b = left->computedValue->reg.b == right->computedValue->reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.b = left->computedValue->reg.f32 == right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.b = left->computedValue->reg.f64 == right->computedValue->reg.f64;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue->reg.b = left->computedValue->reg.u8 == right->computedValue->reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue->reg.b = left->computedValue->reg.u16 == right->computedValue->reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.b = left->computedValue->reg.u32 == right->computedValue->reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue->reg.b = left->computedValue->reg.u64 == right->computedValue->reg.u64;
            break;
        case NativeTypeKind::String:
            node->computedValue->reg.b = left->computedValue->text == right->computedValue->text;
            break;

        default:
            return context->report({context->node, Utf8::format(Msg0001, leftTypeInfo->getDisplayName().c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct || rightTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompOp3Way(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

#define CMP3(__a, __b) __a < __b ? -1 : (__a > __b ? 1 : 0)

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->typeInfo = g_TypeMgr->typeInfoS32;
        node->setFlagsValueIsComputed();
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.b, right->computedValue->reg.b);
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.f32, right->computedValue->reg.f32);
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.f64, right->computedValue->reg.f64);
            break;
        case NativeTypeKind::S8:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.s8, right->computedValue->reg.s8);
            break;
        case NativeTypeKind::S16:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.s16, right->computedValue->reg.s16);
            break;
        case NativeTypeKind::S32:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.s32, right->computedValue->reg.s32);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.s64, right->computedValue->reg.s64);
            break;
        case NativeTypeKind::U8:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.u8, right->computedValue->reg.u8);
            break;
        case NativeTypeKind::U16:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.u16, right->computedValue->reg.u16);
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.u32, right->computedValue->reg.u32);
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            node->computedValue->reg.s32 = CMP3(left->computedValue->reg.u64, right->computedValue->reg.u64);
            break;

        default:
            return context->report({context->node, Utf8::format(Msg0001, leftTypeInfo->getDisplayName().c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoS32;
    }

    return true;
}

bool SemanticJob::resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.b = left->computedValue->reg.b < right->computedValue->reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.b = left->computedValue->reg.f32 < right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.b = left->computedValue->reg.f64 < right->computedValue->reg.f64;
            break;
        case NativeTypeKind::S8:
            node->computedValue->reg.b = left->computedValue->reg.s8 < right->computedValue->reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue->reg.b = left->computedValue->reg.s16 < right->computedValue->reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue->reg.b = left->computedValue->reg.s32 < right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            node->computedValue->reg.b = left->computedValue->reg.s64 < right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue->reg.b = left->computedValue->reg.u8 < right->computedValue->reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue->reg.b = left->computedValue->reg.u16 < right->computedValue->reg.u16;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.b = left->computedValue->reg.u32 < right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            node->computedValue->reg.b = left->computedValue->reg.u64 < right->computedValue->reg.u64;
            break;

        default:
            return context->report({context->node, Utf8::format(Msg0001, leftTypeInfo->getDisplayName().c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = left->typeInfo;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.b = left->computedValue->reg.b > right->computedValue->reg.b;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.b = left->computedValue->reg.f32 > right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.b = left->computedValue->reg.f64 > right->computedValue->reg.f64;
            break;
        case NativeTypeKind::S8:
            node->computedValue->reg.b = left->computedValue->reg.s8 > right->computedValue->reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue->reg.b = left->computedValue->reg.s16 > right->computedValue->reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue->reg.b = left->computedValue->reg.s32 > right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            node->computedValue->reg.b = left->computedValue->reg.s64 > right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue->reg.b = left->computedValue->reg.u8 > right->computedValue->reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue->reg.b = left->computedValue->reg.u16 > right->computedValue->reg.u16;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.b = left->computedValue->reg.u32 > right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            node->computedValue->reg.b = left->computedValue->reg.u64 > right->computedValue->reg.u64;
            break;

        default:
            return context->report({context->node, Utf8::format(Msg0001, leftTypeInfo->getDisplayName().c_str())});
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcreteOrType(context, left));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    if (context->result == ContextResult::Pending)
        return true;

    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
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

    if ((leftTypeInfo->kind == TypeInfoKind::Lambda || leftTypeInfo == g_TypeMgr->typeInfoNull) &&
        (rightTypeInfo->kind == TypeInfoKind::Lambda || rightTypeInfo == g_TypeMgr->typeInfoNull) &&
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
        return context->report({left, Utf8::format(Msg0005, node->token.text.c_str(), TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->getDisplayName().c_str())});
    }
    else if (rightTypeInfo->kind != TypeInfoKind::Native &&
             rightTypeInfo->kind != TypeInfoKind::Pointer &&
             rightTypeInfo->kind != TypeInfoKind::Struct)
    {
        return context->report({right, Utf8::format(Msg0005, node->token.text.c_str(), TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->getDisplayName().c_str())});
    }

    // Cannot compare tuples
    if (leftTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({left, Msg0007});
    if (rightTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({right, Msg0007});

    // Slice can only be compared to null
    if (leftTypeInfo->kind == TypeInfoKind::Slice && rightTypeInfo != g_TypeMgr->typeInfoNull)
        return context->report({left, Msg0009});

    // Interface can only be compared to null
    if (leftTypeInfo->kind == TypeInfoKind::Interface && rightTypeInfo != g_TypeMgr->typeInfoNull)
        return context->report({left, Msg0010});

    // Some types can only be compared for equality
    if (leftTypeInfo->kind == TypeInfoKind::Slice || leftTypeInfo->kind == TypeInfoKind::Interface)
    {
        if (node->token.id != TokenId::SymEqualEqual && node->token.id != TokenId::SymExclamEqual)
            return context->report({left, Utf8::format(Msg0005, node->token.text.c_str(), TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->getDisplayName().c_str())});
    }

    if (node->token.id == TokenId::SymLowerEqualGreater)
        node->typeInfo = g_TypeMgr->typeInfoS32;
    else
        node->typeInfo = g_TypeMgr->typeInfoBool;
    TypeManager::promote(left, right);

    left->typeInfo  = TypeManager::concreteReferenceType(left->typeInfo, CONCRETE_FUNC | CONCRETE_ENUM);
    right->typeInfo = TypeManager::concreteReferenceType(right->typeInfo, CONCRETE_FUNC | CONCRETE_ENUM);

    // Must not make types compatible for a struct, as we can compare a struct with whatever other type in
    // a opEquals function
    if (leftTypeInfo->kind != TypeInfoKind::Struct && rightTypeInfo->kind != TypeInfoKind::Struct)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COMMUTATIVE | CASTFLAG_FORCE_UNCONST | CASTFLAG_COMPARE | CASTFLAG_TRY_COERCE));
    }

    // Struct is on the right, so we need to inverse the test
    else if (leftTypeInfo->kind != TypeInfoKind::Struct)
    {
        swap(left, right);
        node->semFlags |= AST_SEM_INVERSE_PARAMS;
    }

    node->byteCodeFct = ByteCodeGenJob::emitCompareOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    switch (node->token.id)
    {
    case TokenId::SymEqualEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        break;
    case TokenId::SymExclamEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        if (node->computedValue)
            node->computedValue->reg.b = !node->computedValue->reg.b;
        break;
    case TokenId::SymLower:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        break;
    case TokenId::SymLowerEqualGreater:
        SWAG_CHECK(resolveCompOp3Way(context, left, right));
        break;
    case TokenId::SymGreater:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        break;
    case TokenId::SymLowerEqual:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        if (node->computedValue)
            node->computedValue->reg.b = !node->computedValue->reg.b;
        break;
    case TokenId::SymGreaterEqual:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        if (node->computedValue)
            node->computedValue->reg.b = !node->computedValue->reg.b;
        break;
    default:
        return context->internalError( "resolveCompareExpression, token not supported");
    }

    return true;
}
