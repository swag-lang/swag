#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Semantic::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node          = context->node;
    auto       leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto       rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // compile-time compare of two types
    if (left->isConstantGenTypeInfo() && right->isConstantGenTypeInfo())
    {
        const auto ptr1 = left->getConstantGenTypeInfo();
        const auto ptr2 = right->getConstantGenTypeInfo();
        node->setFlagsValueIsComputed();
        node->computedValue()->reg.b = TypeManager::compareConcreteType(ptr1, ptr2);
        return true;
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isSlice())
        {
            // Can only be compared to null
            // @ComparedToNull
            SWAG_ASSERT(right->typeInfoCast && right->typeInfoCast->isPointerNull());
            const auto slice             = static_cast<SwagSlice*>(left->computedValue()->getStorageAddr());
            node->computedValue()->reg.b = !slice->buffer;
            return true;
        }

        if (leftTypeInfo->isInterface())
        {
            // Can only be compared to null
            // @ComparedToNull
            SWAG_ASSERT(right->typeInfoCast && right->typeInfoCast->isPointerNull());
            const auto slice             = static_cast<SwagInterface*>(left->computedValue()->getStorageAddr());
            node->computedValue()->reg.b = !slice->itable;
            return true;
        }

        if (leftTypeInfo->isAny())
        {
            ExportedTypeInfo* anyType;
            const auto        leftParam = left->resolvedSymbolOverload() ? left->resolvedSymbolOverload()->fromInlineParam : nullptr;
            if (leftParam && leftParam->hasExtraPointer(ExtraPointerKind::AnyTypeValue))
            {
                anyType = leftParam->extraPointer<ExportedTypeInfo>(ExtraPointerKind::AnyTypeValue);
            }
            else
                anyType = static_cast<SwagAny*>(left->computedValue()->getStorageAddr())->type;

            // Can only be compared to null
            if (right->typeInfoCast)
            {
                // @ComparedToNull
                SWAG_ASSERT(right->typeInfoCast->isPointerNull());
                node->computedValue()->reg.b = !anyType;
            }
            else
            {
                const auto ptr1              = anyType;
                const auto ptr2              = right->getConstantGenTypeInfo();
                node->computedValue()->reg.b = TypeManager::compareConcreteType(ptr1, ptr2);
            }
            return true;
        }

        if (leftTypeInfo->isPointerNull())
        {
            node->computedValue()->reg.b = right->typeInfo->isPointerNull();
            return true;
        }

        if (leftTypeInfo->isPointer())
        {
            node->computedValue()->reg.b = left->computedValue()->storageSegment == right->computedValue()->storageSegment &&
                                           left->computedValue()->storageOffset == right->computedValue()->storageOffset;
            return true;
        }

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                node->computedValue()->reg.b = left->computedValue()->reg.b == right->computedValue()->reg.b;
                break;
            case NativeTypeKind::F32:
                node->computedValue()->reg.b = left->computedValue()->reg.f32 == right->computedValue()->reg.f32;
                break;
            case NativeTypeKind::F64:
                node->computedValue()->reg.b = left->computedValue()->reg.f64 == right->computedValue()->reg.f64;
                break;
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                node->computedValue()->reg.b = left->computedValue()->reg.u8 == right->computedValue()->reg.u8;
                break;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                node->computedValue()->reg.b = left->computedValue()->reg.u16 == right->computedValue()->reg.u16;
                break;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                node->computedValue()->reg.b = left->computedValue()->reg.u32 == right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                node->computedValue()->reg.b = left->computedValue()->reg.u64 == right->computedValue()->reg.u64;
                break;
            case NativeTypeKind::String:
                node->computedValue()->reg.b = left->computedValue()->text == right->computedValue()->text;
                break;

            default:
            {
                Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
                err.addNote(left, Diagnostic::isType(leftTypeInfo));
                return context->report(err);
            }
        }

        return true;
    }

    // Any versus type, always valid
    if (leftTypeInfo->isAny() && rightTypeInfo->isPointerToTypeInfo())
    {
        return true;
    }

    // Struct against no struct. Must have opEquals.
    if (leftTypeInfo->isStruct() != rightTypeInfo->isStruct() &&
        (leftTypeInfo->isStruct() || rightTypeInfo->isStruct()))
    {
        if (leftTypeInfo->isTuple())
        {
            Diagnostic err{node, node->token, formErr(Err0248, rightTypeInfo->getDisplayNameC())};
            err.addNote(Diagnostic::note(left, Diagnostic::isType(left)));
            err.addNote(Diagnostic::note(right, Diagnostic::isType(right)));
            return context->report(err);
        }

        if (rightTypeInfo->isTuple())
        {
            Diagnostic err{node, node->token, formErr(Err0248, leftTypeInfo->getDisplayNameC())};
            err.addNote(Diagnostic::note(left, Diagnostic::isType(left)));
            err.addNote(Diagnostic::note(right, Diagnostic::isType(right)));
            return context->report(err);
        }

        node->typeInfo = g_TypeMgr->typeInfoBool;
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, left, right));
        YIELD();
        return true;
    }

    // Struct against struct.
    if (leftTypeInfo->isStruct() && rightTypeInfo->isStruct())
    {
        node->typeInfo = g_TypeMgr->typeInfoBool;
        g_SilentError++;
        resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, left, right);
        g_SilentError--;
        YIELD();
        if (node->hasExtraPointer(ExtraPointerKind::UserOp))
            return true;

        leftTypeInfo  = leftTypeInfo->getConstAlias();
        rightTypeInfo = rightTypeInfo->getConstAlias();
        if (!leftTypeInfo->isSame(rightTypeInfo, CAST_FLAG_CAST | CAST_FLAG_FOR_COMPARE))
            SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, left, right));

        SWAG_CHECK(Ast::generateOpEquals(context, leftTypeInfo, rightTypeInfo));
        YIELD();
    }

    return true;
}

bool Semantic::resolveCompOp3Way(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);

#define CMP3(__a, __b) ((__a) < (__b) ? -1 : ((__a) > (__b) ? 1 : 0))

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->typeInfo = g_TypeMgr->typeInfoS32;
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isPointer())
        {
            if (left->computedValue()->storageSegment == right->computedValue()->storageSegment &&
                left->computedValue()->storageOffset == right->computedValue()->storageOffset)
                node->computedValue()->reg.s32 = 0;
            else if (left->computedValue()->storageSegment < right->computedValue()->storageSegment ||
                     left->computedValue()->storageOffset < right->computedValue()->storageOffset)
                node->computedValue()->reg.s32 = -1;
            else
                node->computedValue()->reg.s32 = 1;
        }
        else
        {
            switch (leftTypeInfo->nativeType)
            {
                case NativeTypeKind::Bool:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.b, right->computedValue()->reg.b);
                    break;
                case NativeTypeKind::F32:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.f32, right->computedValue()->reg.f32);
                    break;
                case NativeTypeKind::F64:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.f64, right->computedValue()->reg.f64);
                    break;
                case NativeTypeKind::S8:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.s8, right->computedValue()->reg.s8);
                    break;
                case NativeTypeKind::S16:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.s16, right->computedValue()->reg.s16);
                    break;
                case NativeTypeKind::S32:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.s32, right->computedValue()->reg.s32);
                    break;
                case NativeTypeKind::S64:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.s64, right->computedValue()->reg.s64);
                    break;
                case NativeTypeKind::U8:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.u8, right->computedValue()->reg.u8);
                    break;
                case NativeTypeKind::U16:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.u16, right->computedValue()->reg.u16);
                    break;
                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.u32, right->computedValue()->reg.u32);
                    break;
                case NativeTypeKind::U64:
                    node->computedValue()->reg.s32 = CMP3(left->computedValue()->reg.u64, right->computedValue()->reg.u64);
                    break;

                default:
                {
                    Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
                    err.addNote(left, Diagnostic::isType(leftTypeInfo));
                    return context->report(err);
                }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoS32;
    }
    else if (!leftTypeInfo->isNativeIntegerOrRune() && !leftTypeInfo->isNativeFloat() && !leftTypeInfo->isPointer())
    {
        Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        if (leftTypeInfo->isPointer())
        {
            node->computedValue()->reg.b = left->computedValue()->storageSegment < right->computedValue()->storageSegment ||
                                           left->computedValue()->storageOffset < right->computedValue()->storageOffset;
        }
        else
        {
            switch (leftTypeInfo->nativeType)
            {
                case NativeTypeKind::Bool:
                    node->computedValue()->reg.b = left->computedValue()->reg.b < right->computedValue()->reg.b;
                    break;
                case NativeTypeKind::F32:
                    node->computedValue()->reg.b = left->computedValue()->reg.f32 < right->computedValue()->reg.f32;
                    break;
                case NativeTypeKind::F64:
                    node->computedValue()->reg.b = left->computedValue()->reg.f64 < right->computedValue()->reg.f64;
                    break;
                case NativeTypeKind::S8:
                    node->computedValue()->reg.b = left->computedValue()->reg.s8 < right->computedValue()->reg.s8;
                    break;
                case NativeTypeKind::S16:
                    node->computedValue()->reg.b = left->computedValue()->reg.s16 < right->computedValue()->reg.s16;
                    break;
                case NativeTypeKind::S32:
                    node->computedValue()->reg.b = left->computedValue()->reg.s32 < right->computedValue()->reg.s32;
                    break;
                case NativeTypeKind::S64:
                    node->computedValue()->reg.b = left->computedValue()->reg.s64 < right->computedValue()->reg.s64;
                    break;
                case NativeTypeKind::U8:
                    node->computedValue()->reg.b = left->computedValue()->reg.u8 < right->computedValue()->reg.u8;
                    break;
                case NativeTypeKind::U16:
                    node->computedValue()->reg.b = left->computedValue()->reg.u16 < right->computedValue()->reg.u16;
                    break;
                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                    node->computedValue()->reg.b = left->computedValue()->reg.u32 < right->computedValue()->reg.u32;
                    break;
                case NativeTypeKind::U64:
                    node->computedValue()->reg.b = left->computedValue()->reg.u64 < right->computedValue()->reg.u64;
                    break;

                default:
                {
                    Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
                    err.addNote(left, Diagnostic::isType(leftTypeInfo));
                    return context->report(err);
                }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }
    else if (!leftTypeInfo->isNativeIntegerOrRune() && !leftTypeInfo->isNativeFloat() && !leftTypeInfo->isPointer())
    {
        Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();
        if (leftTypeInfo->isPointer())
        {
            node->computedValue()->reg.b = left->computedValue()->storageSegment > right->computedValue()->storageSegment ||
                                           left->computedValue()->storageOffset > right->computedValue()->storageOffset;
        }
        else
        {
            switch (leftTypeInfo->nativeType)
            {
                case NativeTypeKind::Bool:
                    node->computedValue()->reg.b = left->computedValue()->reg.b > right->computedValue()->reg.b;
                    break;
                case NativeTypeKind::F32:
                    node->computedValue()->reg.b = left->computedValue()->reg.f32 > right->computedValue()->reg.f32;
                    break;
                case NativeTypeKind::F64:
                    node->computedValue()->reg.b = left->computedValue()->reg.f64 > right->computedValue()->reg.f64;
                    break;
                case NativeTypeKind::S8:
                    node->computedValue()->reg.b = left->computedValue()->reg.s8 > right->computedValue()->reg.s8;
                    break;
                case NativeTypeKind::S16:
                    node->computedValue()->reg.b = left->computedValue()->reg.s16 > right->computedValue()->reg.s16;
                    break;
                case NativeTypeKind::S32:
                    node->computedValue()->reg.b = left->computedValue()->reg.s32 > right->computedValue()->reg.s32;
                    break;
                case NativeTypeKind::S64:
                    node->computedValue()->reg.b = left->computedValue()->reg.s64 > right->computedValue()->reg.s64;
                    break;
                case NativeTypeKind::U8:
                    node->computedValue()->reg.b = left->computedValue()->reg.u8 > right->computedValue()->reg.u8;
                    break;
                case NativeTypeKind::U16:
                    node->computedValue()->reg.b = left->computedValue()->reg.u16 > right->computedValue()->reg.u16;
                    break;
                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                    node->computedValue()->reg.b = left->computedValue()->reg.u32 > right->computedValue()->reg.u32;
                    break;
                case NativeTypeKind::U64:
                    node->computedValue()->reg.b = left->computedValue()->reg.u64 > right->computedValue()->reg.u64;
                    break;

                default:
                {
                    Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
                    err.addNote(left, Diagnostic::isType(leftTypeInfo));
                    return context->report(err);
                }
            }
        }
    }
    else if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, left, right));
        node->typeInfo = g_TypeMgr->typeInfoBool;
    }
    else if (!leftTypeInfo->isNativeIntegerOrRune() && !leftTypeInfo->isNativeFloat() && !leftTypeInfo->isPointer())
    {
        Diagnostic err{context->node, context->node->token, formErr(Err0575, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveCompareExpression(SemanticContext* context)
{
    const auto node  = castAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);
    auto       left  = node->firstChild();
    auto       right = node->secondChild();

    SWAG_CHECK(checkIsConcreteOrType(context, left));
    YIELD();
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    YIELD();

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    YIELD();

    // @ConcreteRef
    const auto leftTypeInfo  = getConcreteTypeUnRef(left, CONCRETE_ALL);
    const auto rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_ALL);
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
        (node->token.is(TokenId::SymEqualEqual) || node->token.is(TokenId::SymExclamEqual)))
    {
        // This is fine to compare two lambdas
    }
    else if (!leftTypeInfo->isNative() &&
             !leftTypeInfo->isPointer() &&
             !leftTypeInfo->isStruct() &&
             !leftTypeInfo->isSlice() &&
             !leftTypeInfo->isInterface())
    {
        Diagnostic err{node->token.sourceFile, node->token, formErr(Err0576, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }
    else if (!rightTypeInfo->isNative() &&
             !rightTypeInfo->isPointer() &&
             !rightTypeInfo->isStruct() &&
             !rightTypeInfo->isInterface())
    {
        Diagnostic err{node->token.sourceFile, node->token, formErr(Err0577, node->token.cstr(), rightTypeInfo->getDisplayNameC())};
        err.addNote(right, Diagnostic::isType(rightTypeInfo));
        return context->report(err);
    }

    // Cannot compare tuples
    if (node->token.isNot(TokenId::SymEqualEqual) && node->token.isNot(TokenId::SymExclamEqual))
    {
        if (leftTypeInfo->isTuple() || rightTypeInfo->isTuple())
        {
            Diagnostic err{node->token.sourceFile, node->token, toErr(Err0574)};
            if (leftTypeInfo->isTuple())
                err.addNote(left, Diagnostic::isType(leftTypeInfo));
            else
                err.addNote(right, Diagnostic::isType(rightTypeInfo));
            return context->report(err);
        }
    }

    // @ComparedToNull
    if (!rightTypeInfo->isPointerNull())
    {
        // Slice can only be compared to null
        if (leftTypeInfo->isSlice())
        {
            Diagnostic err{node->token.sourceFile, node->token, formErr(Err0571, rightTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            err.addNote(right, Diagnostic::isType(rightTypeInfo));
            return context->report(err);
        }

        // Interface can only be compared to null ar to another interface
        if (leftTypeInfo->isInterface() && !rightTypeInfo->isInterface() && !rightTypeInfo->isPointerToTypeInfo())
        {
            Diagnostic err{node->token.sourceFile, node->token, formErr(Err0572, rightTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            err.addNote(right, Diagnostic::isType(rightTypeInfo));
            return context->report(err);
        }

        // Any can only be compared to null or to a type
        if (leftTypeInfo->isAny() && !rightTypeInfo->isPointerToTypeInfo())
        {
            Diagnostic err{node->token.sourceFile, node->token, formErr(Err0573, rightTypeInfo->getDisplayNameC())};
            err.addNote(left, leftTypeInfo->isAny() ? toNte(Nte0035) : Diagnostic::isType(leftTypeInfo));
            err.addNote(right, rightTypeInfo->isAny() ? toNte(Nte0035) : Diagnostic::isType(rightTypeInfo));
            return context->report(err);
        }
    }

    // Some types can only be compared for equality
    if (node->token.isNot(TokenId::SymEqualEqual) && node->token.isNot(TokenId::SymExclamEqual))
    {
        if (leftTypeInfo->isSlice() || leftTypeInfo->isInterface())
        {
            Diagnostic err{node->token.sourceFile, node->token, formErr(Err0596, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }

        if (leftTypeInfo->isAny())
        {
            Diagnostic err{node->token.sourceFile, node->token, formErr(Err0596, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, toNte(Nte0035));
            return context->report(err);
        }
    }

    // Return type
    if (node->token.is(TokenId::SymLowerEqualGreater))
        node->typeInfo = g_TypeMgr->typeInfoS32;
    else
        node->typeInfo = g_TypeMgr->typeInfoBool;

    // @ConcreteRef
    left->typeInfo  = getConcreteTypeUnRef(left, CONCRETE_FUNC | CONCRETE_ENUM);
    right->typeInfo = getConcreteTypeUnRef(right, CONCRETE_FUNC | CONCRETE_ENUM);

    SWAG_CHECK(TypeManager::promote(context, left, right));

    // Keep as it is when comparing an 'any' or an interface to a type, as we will generate an implicit @kindof
    if (rightTypeInfo->isPointerToTypeInfo() &&
        (leftTypeInfo->isAny() || leftTypeInfo->isInterface()))
    {
        node->addSpecFlag(AstBinaryOpNode::SPEC_FLAG_IMPLICIT_KINDOF);
    }
    // Must not make types compatible for a struct, as we can compare a struct with whatever other type in a opEquals function.
    else if (!leftTypeInfo->isStruct() && !rightTypeInfo->isStruct())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_COMMUTATIVE | CAST_FLAG_FORCE_UN_CONST | CAST_FLAG_FOR_COMPARE | CAST_FLAG_TRY_COERCE | CAST_FLAG_ACCEPT_PENDING));
        YIELD();
    }

    // Struct is on the right, so we need to inverse the test
    else if (!leftTypeInfo->isStruct())
    {
        std::swap(left, right);
        node->addSemFlag(SEMFLAG_INVERSE_PARAMS);
    }

    node->byteCodeFct = ByteCodeGen::emitCompareOp;
    node->inheritAstFlagsAnd(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritAstFlagsOr(AST_SIDE_EFFECTS);

    switch (node->token.id)
    {
        case TokenId::SymEqualEqual:
            SWAG_CHECK(resolveCompOpEqual(context, left, right));
            YIELD();
            break;
        case TokenId::SymExclamEqual:
            SWAG_CHECK(resolveCompOpEqual(context, left, right));
            YIELD();
            if (node->hasFlagComputedValue())
                node->computedValue()->reg.b = !node->computedValue()->reg.b;
            break;
        case TokenId::SymLower:
            SWAG_CHECK(resolveCompOpLower(context, left, right));
            break;
        case TokenId::SymLowerEqual:
            SWAG_CHECK(resolveCompOpGreater(context, left, right));
            if (node->hasFlagComputedValue())
                node->computedValue()->reg.b = !node->computedValue()->reg.b;
            break;
        case TokenId::SymGreater:
            SWAG_CHECK(resolveCompOpGreater(context, left, right));
            break;
        case TokenId::SymGreaterEqual:
            SWAG_CHECK(resolveCompOpLower(context, left, right));
            if (node->hasFlagComputedValue())
                node->computedValue()->reg.b = !node->computedValue()->reg.b;
            break;
        case TokenId::SymLowerEqualGreater:
            SWAG_CHECK(resolveCompOp3Way(context, left, right));
            break;
        default:
            return Report::internalError(context->node, "resolveCompareExpression, token not supported");
    }

    return true;
}
