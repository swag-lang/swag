#include "pch.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Semantic::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto       node          = castAst<AstOp>(context->node, AstNodeKind::FactorOp);
    const auto sourceFile    = context->sourceFile;
    auto       leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    const auto module        = sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opBinary, "+", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    // :PointerArithmetic
    if (leftTypeInfo->isPointer())
    {
        node->typeInfo = leftTypeInfo;
        if (!leftTypeInfo->isPointerArithmetic() && !context->forDebugger)
        {
            Diagnostic err{node, node->token, toErr(Err0289)};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            err.addNote(toNte(Nte0110));
            return context->report(err);
        }

        if (leftTypeInfo->isPointerTo(NativeTypeKind::Void))
        {
            Diagnostic err{node, node->token, toErr(Err0287)};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }

        SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, formErr(Err0288, rightTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, left, right, CAST_FLAG_TRY_COERCE));
        return true;
    }

    // :PointerArithmetic
    if (rightTypeInfo->isPointer())
    {
        if (!rightTypeInfo->isPointerArithmetic() && !context->forDebugger)
        {
            Diagnostic err{node, node->token, toErr(Err0289)};
            err.addNote(right, Diagnostic::isType(rightTypeInfo));
            err.addNote(toNte(Nte0110));
            return context->report(err);
        }

        if (rightTypeInfo->isPointerTo(NativeTypeKind::Void))
        {
            Diagnostic err{node, node->token, toErr(Err0287)};
            err.addNote(right, Diagnostic::isType(rightTypeInfo));
            return context->report(err);
        }

        node->typeInfo = rightTypeInfo;
        SWAG_VERIFY(leftTypeInfo->isNativeInteger(), context->report({left, formErr(Err0288, leftTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, right, left, CAST_FLAG_TRY_COERCE));
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
        case NativeTypeKind::Rune:
            break;
        case NativeTypeKind::String:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            if (left->hasFlagComputedValue() || right->hasFlagComputedValue())
                err.addNote(toNte(Nte0041));
            return context->report(err);
        }
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    node->typeInfo = leftTypeInfo;
    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.s8, right->computedValue()->reg.s8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS8)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 + right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S16:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.s16, right->computedValue()->reg.s16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS16)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 + right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S32:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.s32, right->computedValue()->reg.s32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS32)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 + right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.s64, right->computedValue()->reg.s64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoS64)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 + right->computedValue()->reg.s64;
                break;
            case NativeTypeKind::U8:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.u8, right->computedValue()->reg.u8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU8)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 + right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U16:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.u16, right->computedValue()->reg.u16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU16)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 + right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.u32, right->computedValue()->reg.u32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU32)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 + right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U64:
                if (addWillOverflow(nullptr, node, left->computedValue()->reg.u64, right->computedValue()->reg.u64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Plus, g_TypeMgr->typeInfoU64)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 + right->computedValue()->reg.u64;
                break;
            case NativeTypeKind::F32:
                node->computedValue()->reg.f32 = left->computedValue()->reg.f32 + right->computedValue()->reg.f32;
                break;
            case NativeTypeKind::F64:
                node->computedValue()->reg.f64 = left->computedValue()->reg.f64 + right->computedValue()->reg.f64;
                break;
            default:
                return Report::internalError(context->node, "resolveBinaryOpPlus, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // 0 + something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            left->release();
        }
        // something + 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
    }

    // Mul+add
    // Must be float, without a dynamic cast
    if (leftTypeInfo->isNativeFloat() && !left->castedTypeInfo && !node->castedTypeInfo)
    {
        if (left->is(AstNodeKind::FactorOp) && left->token.is(TokenId::SymAsterisk))
        {
            left->addSpecFlag(AstOp::SPEC_FLAG_FMA);
            node->addSpecFlag(AstOp::SPEC_FLAG_FMA);
        }
    }

    return true;
}

bool Semantic::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto       node          = context->node;
    const auto sourceFile    = context->sourceFile;
    auto       leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    const auto module        = sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "-", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    // :PointerArithmetic
    if (leftTypeInfo->isPointer())
    {
        node->typeInfo = leftTypeInfo;

        // Subtract two pointers
        if (rightTypeInfo->isPointer())
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
            node->typeInfo = g_TypeMgr->typeInfoS64;
            return true;
        }

        if (!leftTypeInfo->isPointerArithmetic() && !context->forDebugger)
        {
            Diagnostic err{node, node->token, toErr(Err0289)};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            err.addNote(toNte(Nte0110));
            return context->report(err);
        }

        SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, formErr(Err0288, rightTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, left, right, CAST_FLAG_TRY_COERCE));
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    node->typeInfo = leftTypeInfo;
    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.s8, right->computedValue()->reg.s8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS8)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 - right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S16:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.s16, right->computedValue()->reg.s16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS16)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 - right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S32:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.s32, right->computedValue()->reg.s32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS32)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 - right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.s64, right->computedValue()->reg.s64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoS64)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 - right->computedValue()->reg.s64;
                break;
            case NativeTypeKind::U8:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.u8, right->computedValue()->reg.u8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU8)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 - right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U16:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.u16, right->computedValue()->reg.u16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU16)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 - right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.u32, right->computedValue()->reg.u32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU32)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 - right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U64:
                if (subWillOverflow(nullptr, node, left->computedValue()->reg.u64, right->computedValue()->reg.u64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Minus, g_TypeMgr->typeInfoU64)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 - right->computedValue()->reg.u64;
                break;
            case NativeTypeKind::F32:
                node->computedValue()->reg.f32 = left->computedValue()->reg.f32 - right->computedValue()->reg.f32;
                break;
            case NativeTypeKind::F64:
                node->computedValue()->reg.f64 = left->computedValue()->reg.f64 - right->computedValue()->reg.f64;
                break;
            default:
                return Report::internalError(context->node, "resolveBinaryOpMinus, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // something - 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
    }

    return true;
}

bool Semantic::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto       node          = context->node;
    const auto sourceFile    = context->sourceFile;
    auto       leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    const auto module        = sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opBinary, "*", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    node->typeInfo = leftTypeInfo;
    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.s8, right->computedValue()->reg.s8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS8)});
                node->computedValue()->reg.s64 = static_cast<int64_t>(left->computedValue()->reg.s8) * right->computedValue()->reg.s8;
                break;
            case NativeTypeKind::S16:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.s16, right->computedValue()->reg.s16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS16)});
                node->computedValue()->reg.s64 = static_cast<int64_t>(left->computedValue()->reg.s16) * right->computedValue()->reg.s16;
                break;
            case NativeTypeKind::S32:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.s32, right->computedValue()->reg.s32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS32)});
                node->computedValue()->reg.s64 = static_cast<int64_t>(left->computedValue()->reg.s32) * right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.s64, right->computedValue()->reg.s64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoS64)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 * right->computedValue()->reg.s64;
                break;
            case NativeTypeKind::U8:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.u8, right->computedValue()->reg.u8))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU8)});
                node->computedValue()->reg.u64 = static_cast<uint64_t>(left->computedValue()->reg.u8) * right->computedValue()->reg.u8;
                break;
            case NativeTypeKind::U16:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.u16, right->computedValue()->reg.u16))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU16)});
                node->computedValue()->reg.u64 = static_cast<uint64_t>(left->computedValue()->reg.u16) * right->computedValue()->reg.u16;
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.u32, right->computedValue()->reg.u32))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU32)});
                node->computedValue()->reg.u64 = static_cast<uint64_t>(left->computedValue()->reg.u32) * right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U64:
                if (mulWillOverflow(nullptr, node, left->computedValue()->reg.u64, right->computedValue()->reg.u64))
                    return context->report({node, ByteCodeGen::safetyMsg(SafetyMsg::Mul, g_TypeMgr->typeInfoU64)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 * right->computedValue()->reg.u64;
                break;
            case NativeTypeKind::F32:
                node->computedValue()->reg.f32 = left->computedValue()->reg.f32 * right->computedValue()->reg.f32;
                break;
            case NativeTypeKind::F64:
                node->computedValue()->reg.f64 = left->computedValue()->reg.f64 * right->computedValue()->reg.f64;
                break;
            default:
                return Report::internalError(context->node, "resolveBinaryOpMul, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // something * 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.s64 = 0;
        }
        // 1 * something => something
        else if (left->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            left->release();
        }
        // something * 1 => something
        else if (right->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
    }

    return true;
}

bool Semantic::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node          = context->node;
    const auto sourceFile    = context->sourceFile;
    const auto module        = sourceFile->module;
    auto       leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "/", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::F32:
        case NativeTypeKind::F64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    node->typeInfo = leftTypeInfo;
    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
                if (right->computedValue()->reg.s32 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 / right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
                if (right->computedValue()->reg.s64 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 / right->computedValue()->reg.s64;
                break;
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (right->computedValue()->reg.u32 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 / right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U64:
                if (right->computedValue()->reg.u64 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 / right->computedValue()->reg.u64;
                break;
            case NativeTypeKind::F32:
                if (std::bit_cast<uint32_t>(right->computedValue()->reg.f32) == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.f32 = left->computedValue()->reg.f32 / right->computedValue()->reg.f32;
                break;
            case NativeTypeKind::F64:
                if (std::bit_cast<uint64_t>(right->computedValue()->reg.f64) == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.f64 = left->computedValue()->reg.f64 / right->computedValue()->reg.f64;
                break;
            default:
                return Report::internalError(context->node, "resolveBinaryOpDiv, type not supported");
        }
    }
    else if (right->isConstant0())
    {
        return context->report({right, toErr(Err0057)});
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // something / 1 => something
        if (right->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
    }

    return true;
}

bool Semantic::resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node          = context->node;
    auto       leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "%", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
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
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    node->typeInfo = leftTypeInfo;
    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
                if (right->computedValue()->reg.s32 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 % right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
                if (right->computedValue()->reg.s64 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 % right->computedValue()->reg.s64;
                break;
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (right->computedValue()->reg.u32 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 % right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::U64:
                if (right->computedValue()->reg.u64 == 0)
                    return context->report({right, toErr(Err0057)});
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 % right->computedValue()->reg.u64;
                break;
            default:
                return Report::internalError(context->node, "resolveBinaryOpModulo, type not supported");
        }
    }
    else if (right->isConstant0())
    {
        return context->report({right, toErr(Err0057)});
    }

    return true;
}

bool Semantic::resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    const auto module       = node->token.sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "|", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                node->computedValue()->reg.b = left->computedValue()->reg.b || right->computedValue()->reg.b;
                break;

            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u8 | right->computedValue()->reg.u8;
                break;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u16 | right->computedValue()->reg.u16;
                break;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 | right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 | right->computedValue()->reg.u64;
                break;
            default:
                return Report::internalError(context->node, "resolveBitmaskOr, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // 0 | something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            left->release();
        }
        // something | 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
        // something | 0xff (type) => 0xff (type)
        else if (right->hasFlagComputedValue())
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue()->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue()->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue()->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue()->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
        // 0xff (type) | something => 0xff (type)
        else if (left->hasFlagComputedValue())
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue()->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue()->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue()->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue()->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
    }

    return true;
}

bool Semantic::resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    const auto module       = node->token.sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "&", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                node->computedValue()->reg.b = left->computedValue()->reg.b && right->computedValue()->reg.b;
                break;

            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u8 & right->computedValue()->reg.u8;
                break;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u16 & right->computedValue()->reg.u16;
                break;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u32 & right->computedValue()->reg.u32;
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                node->computedValue()->reg.u64 = left->computedValue()->reg.u64 & right->computedValue()->reg.u64;
                break;
            default:
                return Report::internalError(context->node, "resolveBitmaskAnd, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // 0 & something => 0
        // something & 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.s64 = 0;
        }

        // something & 0xFF (type) => something
        else if (right->hasFlagComputedValue())
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue()->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue()->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue()->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue()->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                right->release();
            }
        }

        // 0xFF (type) & something => something
        else if (left->hasFlagComputedValue())
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue()->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue()->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue()->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue()->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                left->release();
            }
        }
    }

    return true;
}

bool Semantic::resolveAppend(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node = context->node;

    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::Note, [] { return toNte(Nte0144); }, true);
        SWAG_CHECK(checkIsConstExpr(context, left->hasFlagComputedValue(), left));
        SWAG_CHECK(checkIsConstExpr(context, right->hasFlagComputedValue(), right));
    }

    if (!left->typeInfo->isString())
        left->computedValue()->text = Ast::literalToString(left->typeInfo, *left->computedValue());
    if (!right->typeInfo->isString())
        right->computedValue()->text = Ast::literalToString(right->typeInfo, *right->computedValue());

    node->setFlagsValueIsComputed();
    node->computedValue()->text = left->computedValue()->text;
    node->computedValue()->text += right->computedValue()->text;
    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveXor(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = context->node;
    const auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "^", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::Rune:
            break;
        default:
        {
            Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
            err.addNote(left, Diagnostic::isType(leftTypeInfo));
            return context->report(err);
        }
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                node->computedValue()->reg.s8 = static_cast<int8_t>(left->computedValue()->reg.s8 ^ right->computedValue()->reg.s8);
                break;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                node->computedValue()->reg.s16 = static_cast<int16_t>(left->computedValue()->reg.s16 ^ right->computedValue()->reg.s16);
                break;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                node->computedValue()->reg.s64 = left->computedValue()->reg.s32 ^ right->computedValue()->reg.s32;
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                node->computedValue()->reg.s64 = left->computedValue()->reg.s64 ^ right->computedValue()->reg.s64;
                break;
            default:
                return Report::internalError(context->node, "resolveXor, type not supported");
        }
    }

    return true;
}

bool Semantic::resolveFactorExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->firstChild();
    auto right = node->secondChild();

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    YIELD();

    // Special case for enum : nothing is possible, except for flags
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
    TypeInfo* rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_FUNC | CONCRETE_ALIAS);
    if (right->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_FUNC | CONCRETE_ALIAS);
    else if (leftTypeInfo->isPointerRef() && !rightTypeInfo->isPointerRef())
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_FUNC | CONCRETE_ALIAS);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    if (leftTypeInfo->isEnum() || rightTypeInfo->isEnum())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

        if (node->token.isNot(TokenId::SymVertical) &&
            node->token.isNot(TokenId::SymAmpersand) &&
            node->token.isNot(TokenId::SymCircumflex))
            return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);

        if (leftTypeInfo->isEnum() && !leftTypeInfo->hasFlag(TYPEINFO_ENUM_FLAGS) && rightTypeInfo == leftTypeInfo)
            return SemanticError::notAllowedError(context, node, leftTypeInfo, "because the enum is not marked with [[#[Swag.EnumFlags]]]");

        if (leftTypeInfo->isEnum() && !leftTypeInfo->hasFlag(TYPEINFO_ENUM_FLAGS))
            return context->report({node, formErr(Err0645, node->token.c_str(), leftTypeInfo->getDisplayNameC())});
        if (rightTypeInfo->isEnum() && !rightTypeInfo->hasFlag(TYPEINFO_ENUM_FLAGS))
            return context->report({node, formErr(Err0646, node->token.c_str(), rightTypeInfo->getDisplayNameC())});
    }

    // :ConcreteRef
    leftTypeInfo  = getConcreteTypeUnRef(left, CONCRETE_ALL);
    rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_ALL);
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

    // Cannot factor tuples
    if (leftTypeInfo->isTuple() || leftTypeInfo->isListTuple())
    {
        Diagnostic err{node, node->token, formErr(Err0651, node->token.c_str())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    if (rightTypeInfo->isTuple() || rightTypeInfo->isListTuple())
    {
        Diagnostic err{node, node->token, formErr(Err0651, node->token.c_str())};
        err.addNote(right, Diagnostic::isType(rightTypeInfo));
        return context->report(err);
    }

    if (node->token.is(TokenId::SymPercent) && rightTypeInfo->isNativeFloat())
    {
        Diagnostic err{node, node->token, formErr(Err0649, node->token.c_str(), rightTypeInfo->getDisplayNameC())};
        err.addNote(right, Diagnostic::isType(rightTypeInfo));
        return context->report(err);
    }

    node->byteCodeFct = ByteCodeGen::emitBinaryOp;
    node->inheritAstFlagsAnd(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritAstFlagsOr(AST_SIDE_EFFECTS);

    // Determine if we must promote.
    if (node->token.isNot(TokenId::SymVertical) &&
        node->token.isNot(TokenId::SymAmpersand) &&
        node->token.isNot(TokenId::SymCircumflex))
    {
        SWAG_CHECK(TypeManager::promote(context, left, right));
        if (node->hasSpecFlag(AstOp::SPEC_FLAG_UP))
        {
            TypeManager::promote32(context, left);
            TypeManager::promote32(context, right);
        }
    }

    // Must do move and not copy
    if (leftTypeInfo->isStruct() || rightTypeInfo->isStruct())
        node->addAstFlag(AST_TRANSIENT);

    // Must invert if commutative operation
    if (!leftTypeInfo->isStruct() && rightTypeInfo->isStruct())
    {
        switch (node->token.id)
        {
            case TokenId::SymPlus:
            case TokenId::SymAsterisk:
                std::swap(left, right);
                std::swap(leftTypeInfo, rightTypeInfo);
                node->addSemFlag(SEMFLAG_INVERSE_PARAMS);
                break;
            default:
                break;
        }
    }

    if (leftTypeInfo->isAny())
    {
        Diagnostic err{node, node->token, formErr(Err0648, node->token.c_str(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, toNte(Nte0035));
        return context->report(err);
    }

    if (rightTypeInfo->isAny())
    {
        Diagnostic err{node, node->token, formErr(Err0649, node->token.c_str(), rightTypeInfo->getDisplayNameC())};
        err.addNote(right, toNte(Nte0035));
        return context->report(err);
    }

    switch (node->token.id)
    {
        case TokenId::SymPlus:
            SWAG_CHECK(resolveBinaryOpPlus(context, left, right));
            break;
        case TokenId::SymMinus:
            SWAG_CHECK(resolveBinaryOpMinus(context, left, right));
            break;
        case TokenId::SymAsterisk:
            SWAG_CHECK(resolveBinaryOpMul(context, left, right));
            break;
        case TokenId::SymSlash:
            SWAG_CHECK(resolveBinaryOpDiv(context, left, right));
            break;
        case TokenId::SymPercent:
            SWAG_CHECK(resolveBinaryOpModulo(context, left, right));
            break;
        case TokenId::SymPlusPlus:
            SWAG_CHECK(resolveAppend(context, left, right));
            break;

        case TokenId::SymVertical:
        case TokenId::SymAmpersand:
        case TokenId::SymCircumflex:
            if (!leftTypeInfo->isStruct())
            {
                SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
                SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CAST_FLAG_TRY_COERCE));
                node->typeInfo = TypeManager::concretePtrRef(left->typeInfo);
            }

            if (node->token.is(TokenId::SymVertical))
                SWAG_CHECK(resolveBitmaskOr(context, left, right));
            else if (node->token.is(TokenId::SymAmpersand))
                SWAG_CHECK(resolveBitmaskAnd(context, left, right));
            else
                SWAG_CHECK(resolveXor(context, left, right));
            break;

        default:
            return Report::internalError(context->node, "resolveFactorExpression, token not supported");
    }

    // :SpecFuncConstExpr
    if (node->hasSpecialFuncCall() && node->hasAstFlag(AST_CONST_EXPR))
    {
        if (leftTypeInfo->isStruct() && !leftTypeInfo->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
            node->removeAstFlag(AST_CONST_EXPR);
    }

    return true;
}

bool Semantic::resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = castAst<AstOp>(context->node, AstNodeKind::FactorOp);
    const auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "<<", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, right, CAST_FLAG_TRY_COERCE));
    const auto module = node->token.sourceFile->module;

    if (!leftTypeInfo->isNativeIntegerOrRune())
    {
        Diagnostic err{node, node->token, formErr(Err0647, "<<", leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(left));
        return context->report(err);
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 8, true);
                break;
            case NativeTypeKind::S16:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 16, true);
                break;
            case NativeTypeKind::S32:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 32, true);
                break;
            case NativeTypeKind::S64:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 64, true);
                break;

            case NativeTypeKind::U8:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 8, false);
                break;
            case NativeTypeKind::U16:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 16, false);
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 32, false);
                break;
            case NativeTypeKind::U64:
                ByteCodeRun::executeLeftShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 64, false);
                break;
            default:
                return Report::internalError(context->node, "resolveShiftLeft, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // something << 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
        // 0 << something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.s64 = 0;
        }
    }

    return true;
}

bool Semantic::resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right)
{
    const auto node         = castAst<AstOp>(context->node, AstNodeKind::FactorOp);
    const auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, ">>", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, right, CAST_FLAG_TRY_COERCE));
    const auto module = node->token.sourceFile->module;

    if (!leftTypeInfo->isNativeIntegerOrRune())
    {
        Diagnostic err{node, node->token, formErr(Err0647, ">>", leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(left));
        return context->report(err);
    }

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 8, true);
                break;
            case NativeTypeKind::S16:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 16, true);
                break;
            case NativeTypeKind::S32:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 32, true);
                break;
            case NativeTypeKind::S64:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 64, true);
                break;

            case NativeTypeKind::U8:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 8, false);
                break;
            case NativeTypeKind::U16:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 16, false);
                break;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 32, false);
                break;
            case NativeTypeKind::U64:
                ByteCodeRun::executeRightShift(&node->computedValue()->reg, left->computedValue()->reg, right->computedValue()->reg, 64, false);
                break;
            default:
                return Report::internalError(context->node, "resolveShiftRight, type not supported");
        }
    }
    else if (module->mustOptimizeBytecode(node))
    {
        // something >> 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            right->release();
        }
        // 0 >> something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.s64 = 0;
        }
    }

    return true;
}

bool Semantic::resolveShiftExpression(SemanticContext* context)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->secondChild();

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    YIELD();

    // :ConcreteRef
    left->typeInfo           = getConcreteTypeUnRef(left, CONCRETE_ALL);
    right->typeInfo          = getConcreteTypeUnRef(right, CONCRETE_ALL);
    const auto leftTypeInfo  = left->typeInfo;
    const auto rightTypeInfo = right->typeInfo;

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

    if (leftTypeInfo->isStruct())
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
    else
        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

    node->typeInfo    = TypeManager::promoteUntyped(left->typeInfo);
    node->byteCodeFct = ByteCodeGen::emitBinaryOp;
    node->inheritAstFlagsAnd(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritAstFlagsOr(AST_SIDE_EFFECTS);

    switch (node->token.id)
    {
        case TokenId::SymLowerLower:
            SWAG_CHECK(resolveShiftLeft(context, left, right));
            break;
        case TokenId::SymGreaterGreater:
            SWAG_CHECK(resolveShiftRight(context, left, right));
            break;
        default:
            return Report::internalError(context->node, "resolveShiftExpression, token not supported");
    }

    // :SpecFuncConstExpr
    if (node->hasSpecialFuncCall() && node->hasAstFlag(AST_CONST_EXPR))
    {
        if (leftTypeInfo->isStruct() && !leftTypeInfo->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
            node->removeAstFlag(AST_CONST_EXPR);
    }

    return true;
}

bool Semantic::resolveBoolExpression(SemanticContext* context)
{
    const auto node   = context->node;
    const auto left   = node->firstChild();
    const auto right  = node->secondChild();
    const auto module = context->sourceFile->module;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    YIELD();

    // :ConcreteRef
    auto leftTypeInfo  = getConcreteTypeUnRef(left, CONCRETE_ALL);
    auto rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_ALL);
    leftTypeInfo       = left->typeInfo;
    rightTypeInfo      = right->typeInfo;

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

    node->typeInfo = g_TypeMgr->typeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, left, CAST_FLAG_AUTO_BOOL));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, right, CAST_FLAG_AUTO_BOOL));

    node->byteCodeFct = ByteCodeGen::emitBinaryOp;

    // In case of && or ||, this is special cause we do not want to evaluate the right part if the left part
    // fails. So we need to do some work once the left part has been emitted
    switch (node->token.id)
    {
        case TokenId::KwdAnd:
            left->setBcNotifyAfter(ByteCodeGen::emitLogicalAndAfterLeft);
            break;
        case TokenId::KwdOr:
            left->setBcNotifyAfter(ByteCodeGen::emitLogicalOrAfterLeft);
            break;
    }

    node->inheritAstFlagsAnd(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritAstFlagsOr(AST_SIDE_EFFECTS);

    if (left->hasFlagComputedValue() && right->hasFlagComputedValue())
    {
        node->setFlagsValueIsComputed();
        switch (node->token.id)
        {
            case TokenId::KwdAnd:
                node->computedValue()->reg.b = left->computedValue()->reg.b && right->computedValue()->reg.b;
                break;
            case TokenId::KwdOr:
                node->computedValue()->reg.b = left->computedValue()->reg.b || right->computedValue()->reg.b;
                break;
            default:
                return Report::internalError(context->node, "resolveBoolExpression, token not supported");
        }
    }
    else if (node->token.is(TokenId::KwdAnd))
    {
        if (module->mustOptimizeBytecode(node))
        {
            // false && something => false
            if (left->isConstantFalse())
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.b = false;
            }
            // something && false => false
            else if (right->isConstantFalse())
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.b = false;
            }
            // true && something => something
            else if (left->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                left->release();
            }
            // something && true => something
            else if (right->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                right->release();
            }
        }
    }
    else if (node->token.is(TokenId::KwdOr))
    {
        if (module->mustOptimizeBytecode(node))
        {
            // true || something => true
            if (left->isConstantTrue())
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.b = true;
            }
            // something || true => true
            else if (right->isConstantTrue())
            {
                node->setFlagsValueIsComputed();
                node->computedValue()->reg.b = true;
            }
            // false || something => something
            else if (left->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                left->release();
            }
            // something || false => something
            else if (right->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                right->release();
            }
        }
    }

    return true;
}
