#include "pch.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool Semantic::resolveUnaryOpMinus(SemanticContext* context, AstNode* op, AstNode* child)
{
    const auto node     = context->node;
    const auto typeInfo = TypeManager::concreteType(child->typeInfo);

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
            break;

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        {
            Diagnostic err{node, node->token, formErr(Err0331, typeInfo->getDisplayNameC())};
            err.addNote(child, Diagnostic::isType(typeInfo));
            return context->report(err);
        }

        case NativeTypeKind::Any:
        {
            Diagnostic err{node, node->token, formErr(Err0332, typeInfo->getDisplayNameC())};
            err.addNote(child, toNte(Nte0032));
            return context->report(err);
        }

        default:
        {
            Diagnostic err{node, node->token, formErr(Err0332, typeInfo->getDisplayNameC())};
            err.addNote(child, Diagnostic::isType(typeInfo));
            return context->report(err);
        }
    }

    if (child->hasFlagComputedValue() && !child->hasSemFlag(SEMFLAG_NEG_EATEN))
    {
        context->node->addSemFlag(child->semFlags.mask(SEMFLAG_LITERAL_SUFFIX));
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                if (child->computedValue()->reg.s8 <= INT8_MIN)
                    return context->report({child, formErr(Err0424, child->computedValue()->reg.s8, -child->computedValue()->reg.s8, "s8")});
                child->computedValue()->reg.s64 = -child->computedValue()->reg.s8;
                break;
            case NativeTypeKind::S16:
                if (child->computedValue()->reg.s16 <= INT16_MIN)
                    return context->report({child, formErr(Err0424, child->computedValue()->reg.s16, -child->computedValue()->reg.s16, "s16")});
                child->computedValue()->reg.s64 = -child->computedValue()->reg.s16;
                break;
            case NativeTypeKind::S32:
                if (child->computedValue()->reg.s32 <= INT32_MIN)
                    return context->report({child, formErr(Err0424, child->computedValue()->reg.s32, -child->computedValue()->reg.s32, "s32")});
                child->computedValue()->reg.s64 = -child->computedValue()->reg.s32;
                if (typeInfo->isUntypedInteger())
                {
                    auto native          = castTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind);
                    native               = castTypeInfo<TypeInfoNative>(native->clone());
                    native->valueInteger = -native->valueInteger;
                    child->typeInfo      = TypeManager::resolveUntypedType(native, std::bit_cast<uint32_t>(native->valueInteger));
                    op->typeInfo         = child->typeInfo;
                }
                break;
            case NativeTypeKind::S64:
                if (child->computedValue()->reg.s64 <= INT64_MIN)
                    return context->report({child, formErr(Err0423, child->computedValue()->reg.s64, -child->computedValue()->reg.s64)});
                child->computedValue()->reg.s64 = -child->computedValue()->reg.s64;
                break;

            case NativeTypeKind::F32:
                child->computedValue()->reg.f32 = -child->computedValue()->reg.f32;
                if (typeInfo->isUntypedFloat())
                {
                    auto native        = castTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind);
                    native             = castTypeInfo<TypeInfoNative>(native->clone());
                    native->valueFloat = -native->valueFloat;
                    child->typeInfo    = TypeManager::resolveUntypedType(typeInfo, std::bit_cast<uint32_t>(native->valueFloat));
                    op->typeInfo       = child->typeInfo;
                }

                break;
            case NativeTypeKind::F64:
                child->computedValue()->reg.f64 = -child->computedValue()->reg.f64;
                break;
            default:
                break;
        }
    }

    return true;
}

bool Semantic::resolveUnaryOpExclam(SemanticContext* context, AstNode* child)
{
    const auto typeInfo = TypeManager::concreteType(child->typeInfo);
    if (typeInfo->isLambdaClosure() || typeInfo->isPointer() || typeInfo->isInterface() || typeInfo->isSlice())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, child, CAST_FLAG_AUTO_BOOL));
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, context->node, typeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, child, CAST_FLAG_AUTO_BOOL));

    if (child->hasFlagComputedValue())
    {
        context->node->addSemFlag(child->semFlags.mask(SEMFLAG_LITERAL_SUFFIX));
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                child->computedValue()->reg.b = !child->computedValue()->reg.b;
                break;
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                child->computedValue()->reg.b = child->computedValue()->reg.u8 ? false : true;
                break;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                child->computedValue()->reg.b = child->computedValue()->reg.u16 ? false : true;
                break;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                child->computedValue()->reg.b = child->computedValue()->reg.u32 ? false : true;
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                child->computedValue()->reg.b = child->computedValue()->reg.u64 ? false : true;
                break;
            default:
                break;
        }
    }

    return true;
}

bool Semantic::resolveUnaryOpInvert(SemanticContext* context, AstNode* child)
{
    const auto node     = context->node;
    const auto typeInfo = TypeManager::concreteType(child->typeInfo);

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;

        case NativeTypeKind::Any:
        {
            Diagnostic err{node, node->token, formErr(Err0345, typeInfo->getDisplayNameC())};
            err.addNote(child, toNte(Nte0032));
            return context->report(err);
        }

        default:
        {
            Diagnostic err{node, node->token, formErr(Err0345, typeInfo->getDisplayNameC())};
            err.addNote(child, Diagnostic::isType(child));
            return context->report(err);
        }
    }

    if (child->hasFlagComputedValue())
    {
        context->node->addSemFlag(child->semFlags.mask(SEMFLAG_LITERAL_SUFFIX));
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                child->computedValue()->reg.u64 = child->computedValue()->reg.u8;
                child->computedValue()->reg.u8  = ~child->computedValue()->reg.u8;
                break;

            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                child->computedValue()->reg.u64 = child->computedValue()->reg.u16;
                child->computedValue()->reg.u16 = ~child->computedValue()->reg.u16;
                break;

            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
                child->computedValue()->reg.u64 = child->computedValue()->reg.u32;
                child->computedValue()->reg.u32 = ~child->computedValue()->reg.u32;
                break;

            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                child->computedValue()->reg.u64 = ~child->computedValue()->reg.u64;
                break;

            default:
                break;
        }
    }

    return true;
}

bool Semantic::resolveUnaryOp(SemanticContext* context)
{
    const auto op    = context->node;
    const auto child = op->children[0];

    op->typeInfo    = child->typeInfo;
    op->byteCodeFct = ByteCodeGen::emitUnaryOp;

    op->inheritAstFlagsOr(child, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    SWAG_CHECK(checkIsConcrete(context, child));
    op->addAstFlag(AST_R_VALUE);

    // Special case for enum : nothing is possible, except for flags
    auto typeInfo = child->typeInfo->getConcreteAlias();
    if (typeInfo->isEnum())
    {
        if (!typeInfo->hasFlag(TYPEINFO_ENUM_FLAGS))
            return SemanticError::notAllowedError(context, op, typeInfo, "because the enum is not marked with [[#[Swag.EnumFlags]]]");
    }

    // :ConcreteRef
    child->typeInfo = getConcreteTypeUnRef(child, CONCRETE_ALL);
    typeInfo        = child->typeInfo;

    if (typeInfo->isStruct())
    {
        switch (op->token.id)
        {
            case TokenId::SymExclam:
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "!", nullptr, child, nullptr));
                break;
            case TokenId::SymMinus:
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "-", nullptr, child, nullptr));
                break;
            case TokenId::SymTilde:
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "~", nullptr, child, nullptr));
                break;
            default:
                break;
        }

        op->typeInfo = typeInfo;
        op->addAstFlag(AST_TRANSIENT);

        // :SpecFuncConstExpr
        if (op->hasSpecialFuncCall() && op->hasAstFlag(AST_CONST_EXPR))
        {
            if (!typeInfo->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
                op->removeAstFlag(AST_CONST_EXPR);
        }

        return true;
    }

    switch (op->token.id)
    {
        case TokenId::SymExclam:
            SWAG_CHECK(resolveUnaryOpExclam(context, child));
            context->node->typeInfo = g_TypeMgr->typeInfoBool;
            break;
        case TokenId::SymMinus:
            SWAG_CHECK(checkTypeIsNative(context, op, typeInfo));
            SWAG_CHECK(resolveUnaryOpMinus(context, op, child));
            break;
        case TokenId::SymTilde:
            SWAG_CHECK(checkTypeIsNative(context, op, typeInfo));
            SWAG_CHECK(resolveUnaryOpInvert(context, child));
            break;
    }

    op->inheritComputedValue(child);
    return true;
}
