#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "Ast.h"

#pragma optimize("", off)
bool SemanticJob::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // Compile time compare of two types
    if (left->isConstantGenTypeInfo() && right->isConstantGenTypeInfo())
    {
        auto ptr1 = left->getConstantGenTypeInfo();
        auto ptr2 = right->getConstantGenTypeInfo();
        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = TypeManager::compareConcreteType(ptr1, ptr2);
    }
    else if (left->hasComputedValue() && right->hasComputedValue())
    {
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isSlice())
        {
            // Can only be compared to null
            // :ComparedToNull
            SWAG_ASSERT(right->castedTypeInfo && right->castedTypeInfo->isPointerNull());
            auto slice                 = (SwagSlice*) left->computedValue->getStorageAddr();
            node->computedValue->reg.b = !slice->buffer;
        }
        else if (leftTypeInfo->isInterface())
        {
            // Can only be compared to null
            // :ComparedToNull
            SWAG_ASSERT(right->castedTypeInfo && right->castedTypeInfo->isPointerNull());
            auto slice                 = (SwagInterface*) left->computedValue->getStorageAddr();
            node->computedValue->reg.b = !slice->itable;
        }
        else if (leftTypeInfo->isAny())
        {
            // Can only be compared to null
            auto any = (SwagAny*) left->computedValue->getStorageAddr();
            if (right->castedTypeInfo)
            {
                // :ComparedToNull
                SWAG_ASSERT(right->castedTypeInfo->isPointerNull());
                node->computedValue->reg.b = !any->type;
            }
            else
            {
                auto ptr1                  = any->type;
                auto ptr2                  = right->getConstantGenTypeInfo();
                node->computedValue->reg.b = TypeManager::compareConcreteType(ptr1, ptr2);
            }
        }
        else if (leftTypeInfo->isPointerNull())
        {
            node->computedValue->reg.b = right->typeInfo->isPointerNull();
        }
        else if (leftTypeInfo->isPointer())
        {
            node->computedValue->reg.b = left->computedValue->storageSegment == right->computedValue->storageSegment &&
                                         left->computedValue->storageOffset == right->computedValue->storageOffset;
        }
        else
        {
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
                node->computedValue->reg.b = left->computedValue->reg.u64 == right->computedValue->reg.u64;
                break;
            case NativeTypeKind::String:
                node->computedValue->reg.b = left->computedValue->text == right->computedValue->text;
                break;

            default:
            {
                Diagnostic diag{context->node, context->node->token, Fmt(Err(Err0001), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }
            }
        }
    }
    else if (leftTypeInfo->isAny() && rightTypeInfo->isPointerToTypeInfo())
    {
        // Ok, specific case
    }
    else if (leftTypeInfo->isStruct() || rightTypeInfo->isStruct())
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

    if (left->hasComputedValue() && right->hasComputedValue())
    {
        node->typeInfo = g_TypeMgr->typeInfoS32;
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isPointer())
        {
            if (left->computedValue->storageSegment == right->computedValue->storageSegment &&
                left->computedValue->storageOffset == right->computedValue->storageOffset)
                node->computedValue->reg.s32 = 0;
            else if (left->computedValue->storageSegment < right->computedValue->storageSegment ||
                     left->computedValue->storageOffset < right->computedValue->storageOffset)
                node->computedValue->reg.s32 = -1;
            else
                node->computedValue->reg.s32 = 1;
        }
        else
        {
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
                node->computedValue->reg.s32 = CMP3(left->computedValue->reg.u64, right->computedValue->reg.u64);
                break;

            default:
            {
                Diagnostic diag{context->node, context->node->token, Fmt(Err(Err0001), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
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

    if (left->hasComputedValue() && right->hasComputedValue())
    {
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isPointer())
        {
            node->computedValue->reg.b = left->computedValue->storageSegment < right->computedValue->storageSegment ||
                                         left->computedValue->storageOffset < right->computedValue->storageOffset;
        }
        else
        {
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
                node->computedValue->reg.b = left->computedValue->reg.u64 < right->computedValue->reg.u64;
                break;

            default:
            {
                Diagnostic diag{context->node, context->node->token, Fmt(Err(Err0001), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
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

    if (left->hasComputedValue() && right->hasComputedValue())
    {
        node->setFlagsValueIsComputed();
        if (leftTypeInfo->isPointer())
        {
            node->computedValue->reg.b = left->computedValue->storageSegment > right->computedValue->storageSegment ||
                                         left->computedValue->storageOffset > right->computedValue->storageOffset;
        }
        else
        {
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
                node->computedValue->reg.b = left->computedValue->reg.u64 > right->computedValue->reg.u64;
                break;

            default:
            {
                Diagnostic diag{context->node, context->node->token, Fmt(Err(Err0001), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }

    return true;
}

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node  = CastAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);
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

    // :ConcreteRef
    auto leftTypeInfo  = getConcreteTypeUnRef(left, CONCRETE_ALL);
    auto rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_ALL);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->isKindGeneric())
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->isKindGeneric())
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    if ((leftTypeInfo->isLambdaClosure() || leftTypeInfo->isPointerNull()) &&
        (rightTypeInfo->isLambdaClosure() || rightTypeInfo->isPointerNull()) &&
        (node->tokenId == TokenId::SymEqualEqual || node->tokenId == TokenId::SymExclamEqual))
    {
        // This is fine to compare two lambdas
    }
    else if (!leftTypeInfo->isNative() &&
             !leftTypeInfo->isPointer() &&
             !leftTypeInfo->isStruct() &&
             !leftTypeInfo->isSlice() &&
             !leftTypeInfo->isInterface())
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0809), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.addRange(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    else if (!rightTypeInfo->isNative() &&
             !rightTypeInfo->isPointer() &&
             !rightTypeInfo->isStruct() &&
             !rightTypeInfo->isInterface())
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0778), node->token.ctext(), rightTypeInfo->getDisplayNameC())};
        diag.addRange(right, Diagnostic::isType(rightTypeInfo));
        return context->report(diag);
    }

    // Cannot compare tuples
    if (leftTypeInfo->isTuple() || rightTypeInfo->isTuple())
    {
        Diagnostic diag{node->sourceFile, node->token, Err(Err0007)};
        diag.addRange(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }

    // :ComparedToNull
    if (!rightTypeInfo->isPointerNull())
    {
        // Slice can only be compared to null
        if (leftTypeInfo->isSlice())
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0009), rightTypeInfo->getDisplayNameC())};
            diag.addRange(left, Diagnostic::isType(leftTypeInfo));
            diag.addRange(right, Diagnostic::isType(rightTypeInfo));
            return context->report(diag);
        }

        // Interface can only be compared to null ar to another interface
        if (leftTypeInfo->isInterface() && !rightTypeInfo->isInterface() && !rightTypeInfo->isPointerToTypeInfo())
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0010), rightTypeInfo->getDisplayNameC())};
            diag.addRange(left, Diagnostic::isType(leftTypeInfo));
            diag.addRange(right, Diagnostic::isType(rightTypeInfo));
            return context->report(diag);
        }

        // Any can only be compared to null or to a type
        if (leftTypeInfo->isAny() && !rightTypeInfo->isPointerToTypeInfo())
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0181), rightTypeInfo->getDisplayNameC())};
            diag.addRange(left, leftTypeInfo->isAny() ? Nte(Nte1116) : Diagnostic::isType(leftTypeInfo));
            diag.addRange(right, rightTypeInfo->isAny() ? Nte(Nte1116) : Diagnostic::isType(rightTypeInfo));
            return context->report(diag);
        }
    }

    // Some types can only be compared for equality
    if (node->tokenId != TokenId::SymEqualEqual && node->tokenId != TokenId::SymExclamEqual)
    {
        if (leftTypeInfo->isSlice() || leftTypeInfo->isInterface())
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
            diag.addRange(left, Diagnostic::isType(leftTypeInfo));
            return context->report(diag);
        }

        if (leftTypeInfo->isAny())
        {
            Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
            diag.addRange(left, Nte(Nte1116));
            return context->report(diag);
        }
    }

    // Return type
    if (node->tokenId == TokenId::SymLowerEqualGreater)
        node->typeInfo = g_TypeMgr->typeInfoS32;
    else
        node->typeInfo = g_TypeMgr->typeInfoBool;

    // :ConcreteRef
    left->typeInfo  = getConcreteTypeUnRef(left, CONCRETE_FUNC | CONCRETE_ENUM);
    right->typeInfo = getConcreteTypeUnRef(right, CONCRETE_FUNC | CONCRETE_ENUM);

    SWAG_CHECK(TypeManager::promote(context, left, right));

    // Keep as it is when comparing an 'any' or an interface to a type, as we will generate an implicit @kindof
    if (rightTypeInfo->isPointerToTypeInfo() &&
        (leftTypeInfo->isAny() || leftTypeInfo->isInterface()))
    {
        node->specFlags |= AstBinaryOpNode::SPECFLAG_IMPLICIT_KINDOF;
    }
    // Must not make types compatible for a struct, as we can compare a struct with whatever other type in a opEquals function.
    else if (!leftTypeInfo->isStruct() && !rightTypeInfo->isStruct())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COMMUTATIVE | CASTFLAG_FORCE_UNCONST | CASTFLAG_COMPARE | CASTFLAG_TRY_COERCE));
    }

    // Struct is on the right, so we need to inverse the test
    else if (!leftTypeInfo->isStruct())
    {
        swap(left, right);
        node->semFlags |= SEMFLAG_INVERSE_PARAMS;
    }

    node->byteCodeFct = ByteCodeGenJob::emitCompareOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    switch (node->tokenId)
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
    case TokenId::SymLowerEqual:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        if (node->computedValue)
            node->computedValue->reg.b = !node->computedValue->reg.b;
        break;
    case TokenId::SymGreater:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        break;
    case TokenId::SymGreaterEqual:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        if (node->computedValue)
            node->computedValue->reg.b = !node->computedValue->reg.b;
        break;
    case TokenId::SymLowerEqualGreater:
        SWAG_CHECK(resolveCompOp3Way(context, left, right));
        break;
    default:
        return Report::internalError(context->node, "resolveCompareExpression, token not supported");
    }

    return true;
}
