#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Concat.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "Math.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"

bool SemanticJob::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    auto module        = sourceFile->module;

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
        SWAG_VERIFY(leftTypeInfo->flags & TYPEINFO_POINTER_ARITHMETIC || context->forDebugger, context->report({left, Err(Err0192), Diagnostic::isType(leftTypeInfo)}));
        SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, Err(Err0144)}));
        SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, Fmt(Err(Err0579), rightTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, left, right, CASTFLAG_TRY_COERCE));
        return true;
    }

    // :PointerArithmetic
    if (rightTypeInfo->isPointer())
    {
        node->typeInfo = rightTypeInfo;
        SWAG_VERIFY(rightTypeInfo->flags & TYPEINFO_POINTER_ARITHMETIC, context->report({right, Err(Err0192), Diagnostic::isType(rightTypeInfo)}));
        SWAG_VERIFY((rightTypeInfo->isPointerToTypeInfo()) == 0, context->report({right, Err(Err0144)}));
        SWAG_VERIFY(leftTypeInfo->isNativeInteger(), context->report({left, Fmt(Err(Err0579), leftTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, right, left, CASTFLAG_TRY_COERCE));
        return true;
    }

    SWAG_VERIFY(rightTypeInfo->isNative(), context->report({right, Fmt(Err(Err0142), rightTypeInfo->getDisplayNameC())}));
    PushErrContext ec(context, nullptr, ErrorContextKind::MsgPrio, Fmt(Err(Err0196), "add", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));

    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::String:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0143), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        Diagnostic note{Hlp(Hlp0040)};
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag, &note);
    }
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0143), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (addWillOverflow(node, left->computedValue->reg.s32, right->computedValue->reg.s32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS32)});
            node->computedValue->reg.s64 = left->computedValue->reg.s32 + right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (addWillOverflow(node, left->computedValue->reg.s64, right->computedValue->reg.s64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoS64)});
            node->computedValue->reg.s64 = left->computedValue->reg.s64 + right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (addWillOverflow(node, left->computedValue->reg.u32, right->computedValue->reg.u32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU32)});
            node->computedValue->reg.u64 = left->computedValue->reg.u32 + right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (addWillOverflow(node, left->computedValue->reg.u64, right->computedValue->reg.u64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFPlus, g_TypeMgr->typeInfoU64)});
            node->computedValue->reg.u64 = left->computedValue->reg.u64 + right->computedValue->reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.f32 = left->computedValue->reg.f32 + right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.f64 = left->computedValue->reg.f64 + right->computedValue->reg.f64;
            break;
        default:
            return Report::internalError(context->node, "resolveBinaryOpPlus, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 + something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
        }
        // something + 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    auto module        = sourceFile->module;

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

        // Substract two pointers
        if (rightTypeInfo->isPointer())
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
            node->typeInfo = g_TypeMgr->typeInfoInt;
            return true;
        }

        SWAG_VERIFY(leftTypeInfo->flags & TYPEINFO_POINTER_ARITHMETIC, context->report({left, Err(Err0192), Diagnostic::isType(leftTypeInfo)}));
        SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, Err(Err0144)}));
        SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, Fmt(Err(Err0579), rightTypeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, left, right, CASTFLAG_TRY_COERCE));
        return true;
    }

    SWAG_VERIFY(rightTypeInfo->isNative(), context->report({right, Fmt(Err(Err0146), rightTypeInfo->getDisplayNameC())}));
    PushErrContext ec(context, nullptr, ErrorContextKind::MsgPrio, Fmt(Err(Err0196), "substract", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));
    leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0143), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (subWillOverflow(node, left->computedValue->reg.s32, right->computedValue->reg.s32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS32)});
            node->computedValue->reg.s64 = left->computedValue->reg.s32 - right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (subWillOverflow(node, left->computedValue->reg.s64, right->computedValue->reg.s64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoS64)});
            node->computedValue->reg.s64 = left->computedValue->reg.s64 - right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (subWillOverflow(node, left->computedValue->reg.u32, right->computedValue->reg.u32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU32)});
            node->computedValue->reg.u64 = left->computedValue->reg.u32 - right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (subWillOverflow(node, left->computedValue->reg.u64, right->computedValue->reg.u64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMinus, g_TypeMgr->typeInfoU64)});
            node->computedValue->reg.u64 = left->computedValue->reg.u64 - right->computedValue->reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.f32 = left->computedValue->reg.f32 - right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.f64 = left->computedValue->reg.f64 - right->computedValue->reg.f64;
            break;
        default:
            return Report::internalError(context->node, "resolveBinaryOpMinus, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something - 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);
    auto module        = sourceFile->module;

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opBinary, "*", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
    PushErrContext ec(context, nullptr, ErrorContextKind::MsgPrio, Fmt(Err(Err0196), "multiply", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0143), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (mulWillOverflow(node, left->computedValue->reg.s32, right->computedValue->reg.s32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS32)});
            node->computedValue->reg.s64 = left->computedValue->reg.s32 * right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (mulWillOverflow(node, left->computedValue->reg.s64, right->computedValue->reg.s64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoS64)});
            node->computedValue->reg.s64 = left->computedValue->reg.s64 * right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (mulWillOverflow(node, left->computedValue->reg.u32, right->computedValue->reg.u32))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU32)});
            node->computedValue->reg.u64 = left->computedValue->reg.u32 * right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (mulWillOverflow(node, left->computedValue->reg.u64, right->computedValue->reg.u64))
                return context->report({node, ByteCodeGenJob::safetyMsg(SafetyMsg::IFMul, g_TypeMgr->typeInfoU64)});
            node->computedValue->reg.u64 = left->computedValue->reg.u64 * right->computedValue->reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue->reg.f32 = left->computedValue->reg.f32 * right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue->reg.f64 = left->computedValue->reg.f64 * right->computedValue->reg.f64;
            break;
        default:
            return Report::internalError(context->node, "resolveBinaryOpMul, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something * 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.s64 = 0;
        }
        // 1 * something => something
        else if (left->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
        }
        // something * 1 => something
        else if (right->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "/", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
    PushErrContext ec(context, nullptr, ErrorContextKind::MsgPrio, Fmt(Err(Err0196), "divide", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0143), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue->reg.s32 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.s64 = left->computedValue->reg.s32 / right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue->reg.s64 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.s64 = left->computedValue->reg.s64 / right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (right->computedValue->reg.u32 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.u64 = left->computedValue->reg.u32 / right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue->reg.u64 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.u64 = left->computedValue->reg.u64 / right->computedValue->reg.u64;
            break;
        case NativeTypeKind::F32:
            if (right->computedValue->reg.f32 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.f32 = left->computedValue->reg.f32 / right->computedValue->reg.f32;
            break;
        case NativeTypeKind::F64:
            if (right->computedValue->reg.f64 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.f64 = left->computedValue->reg.f64 / right->computedValue->reg.f64;
            break;
        default:
            return Report::internalError(context->node, "resolveBinaryOpDiv, type not supported");
        }
    }
    else if (right->isConstant0())
        return context->report({right, Err(Err0150), Hnt(Hnt0033)});

    return true;
}

bool SemanticJob::resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "%", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
    PushErrContext ec(context, nullptr, ErrorContextKind::MsgPrio, Fmt(Err(Err0196), "make a modulo on", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::Rune:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, Fmt(Err(Err0157), leftTypeInfo->getDisplayNameC())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue->reg.s32 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.s64 = left->computedValue->reg.s32 % right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue->reg.s64 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.s64 = left->computedValue->reg.s64 % right->computedValue->reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            if (right->computedValue->reg.u32 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.u64 = left->computedValue->reg.u32 % right->computedValue->reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue->reg.u64 == 0)
                return context->report({right, Err(Err0150), Hnt(Hnt0033)});
            node->computedValue->reg.u64 = left->computedValue->reg.u64 % right->computedValue->reg.u64;
            break;
        default:
            return Report::internalError(context->node, "resolveBinaryOpModulo, type not supported");
        }
    }
    else if (right->isConstant0())
        return context->report({right, Err(Err0150)});

    return true;
}

bool SemanticJob::resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    auto module       = node->sourceFile->module;

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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0163), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.b = left->computedValue->reg.b || right->computedValue->reg.b;
            break;

        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue->reg.u64 = left->computedValue->reg.u8 | right->computedValue->reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue->reg.u64 = left->computedValue->reg.u16 | right->computedValue->reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.u64 = left->computedValue->reg.u32 | right->computedValue->reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue->reg.u64 = left->computedValue->reg.u64 | right->computedValue->reg.u64;
            break;
        default:
            return Report::internalError(context->node, "resolveBitmaskOr, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 | something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
        }
        // something | 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
        // something | 0xff (type) => 0xff (type)
        else if (right->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
        // 0xff (type) | something => 0xff (type)
        else if (left->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
    }

    return true;
}

bool SemanticJob::resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    auto module       = node->sourceFile->module;

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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0164), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue->reg.b = left->computedValue->reg.b && right->computedValue->reg.b;
            break;

        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue->reg.u64 = left->computedValue->reg.u8 & right->computedValue->reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue->reg.u64 = left->computedValue->reg.u16 & right->computedValue->reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.u64 = left->computedValue->reg.u32 & right->computedValue->reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue->reg.u64 = left->computedValue->reg.u64 & right->computedValue->reg.u64;
            break;
        default:
            return Report::internalError(context->node, "resolveBitmaskAnd, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 & something => 0
        // something & 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.s64 = 0;
        }

        // something & 0xFF (type) => something
        else if (right->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
            }
        }

        // 0xFF (type) & something => something
        else if (left->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue->reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue->reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue->reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue->reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
            }
        }
    }

    return true;
}

bool SemanticJob::resolveAppend(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    SWAG_CHECK(checkIsConstExpr(context, left->flags & AST_VALUE_COMPUTED, left));

    if (!left->typeInfo->isString())
        left->computedValue->text = Ast::literalToString(left->typeInfo, *left->computedValue);
    if (!right->typeInfo->isString())
        right->computedValue->text = Ast::literalToString(right->typeInfo, *right->computedValue);

    node->setFlagsValueIsComputed();
    node->computedValue->text = left->computedValue->text;
    node->computedValue->text += right->computedValue->text;
    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool SemanticJob::resolveXor(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0164), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.setRange2(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue->reg.s8 = left->computedValue->reg.s8 ^ right->computedValue->reg.s8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue->reg.s16 = left->computedValue->reg.s16 ^ right->computedValue->reg.s16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            node->computedValue->reg.s64 = left->computedValue->reg.s32 ^ right->computedValue->reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue->reg.s64 = left->computedValue->reg.s64 ^ right->computedValue->reg.s64;
            break;
        default:
            return Report::internalError(context->node, "resolveXor, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveFactorExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    if (context->result == ContextResult::Pending)
        return true;

    // Determin if we must promote to 32/64 bits.
    // Bit manipulations to not promote.
    bool mustPromote = true;
    if (node->token.id == TokenId::SymVertical ||
        node->token.id == TokenId::SymAmpersand ||
        node->token.id == TokenId::SymCircumflex)
    {
        mustPromote = false;
    }

    // Special case for enum : nothing is possible, except for flags
    bool isEnumFlags   = false;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo, CONCRETE_ALIAS | CONCRETE_FUNC);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo, CONCRETE_ALIAS | CONCRETE_FUNC);
    if (leftTypeInfo->isEnum() || rightTypeInfo->isEnum())
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

        if (node->token.id != TokenId::SymVertical &&
            node->token.id != TokenId::SymAmpersand &&
            node->token.id != TokenId::SymCircumflex)
            return notAllowed(context, node, leftTypeInfo);

        if (leftTypeInfo->isEnum() && !(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS) && rightTypeInfo == leftTypeInfo)
            return notAllowed(context, node, leftTypeInfo, "because the enum is not marked with `Swag.EnumFlags`");

        if (leftTypeInfo->isEnum() && !(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
            return context->report({node, Fmt(Err(Err0037), node->token.ctext(), leftTypeInfo->getDisplayNameC())});
        if (rightTypeInfo->isEnum() && !(rightTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
            return context->report({node, Fmt(Err(Err0037), node->token.ctext(), rightTypeInfo->getDisplayNameC())});

        isEnumFlags = true;
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

    // Cannot compare tuples
    if (leftTypeInfo->isTuple())
        return context->report({left, Fmt(Err(Err0168), node->token.ctext())});
    if (rightTypeInfo->isTuple())
        return context->report({right, Fmt(Err(Err0168), node->token.ctext())});

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    TypeInfo* promotedFrom = nullptr;
    if (mustPromote)
    {
        TypeInfo* saveLeftTypeInfo        = left->typeInfo;
        TypeInfo* saveRightTypeInfo       = right->typeInfo;
        TypeInfo* saveCastedLeftTypeInfo  = left->castedTypeInfo;
        TypeInfo* saveCastedRightTypeInfo = right->castedTypeInfo;

        // Special case for promotion of s8/s16/u8/u16. We want to keep trace of the original type
        // in order to be able to make implicit conversions (with safety checks)
        if (!(leftTypeInfo->flags & TYPEINFO_CAN_PROMOTE_816) && leftTypeInfo->promotedFrom)
            leftTypeInfo = left->typeInfo = leftTypeInfo->promotedFrom;

        if (!(rightTypeInfo->flags & TYPEINFO_CAN_PROMOTE_816) && rightTypeInfo->promotedFrom)
            rightTypeInfo = right->typeInfo = rightTypeInfo->promotedFrom;

        if (leftTypeInfo->flags & TYPEINFO_CAN_PROMOTE_816 || rightTypeInfo->flags & TYPEINFO_CAN_PROMOTE_816)
        {
            TypeManager::promote816(left, right);
            promotedFrom = left->typeInfo;
            if (!(promotedFrom->flags & TYPEINFO_CAN_PROMOTE_816))
                promotedFrom = nullptr;

            left->typeInfo        = saveLeftTypeInfo;
            right->typeInfo       = saveRightTypeInfo;
            left->castedTypeInfo  = saveCastedLeftTypeInfo;
            right->castedTypeInfo = saveCastedRightTypeInfo;
            leftTypeInfo          = TypeManager::concretePtrRefType(left->typeInfo);
            rightTypeInfo         = TypeManager::concretePtrRefType(right->typeInfo);
        }

        TypeManager::promote3264(left, right);
    }

    // Must do move and not copy
    if (leftTypeInfo->isStruct() || rightTypeInfo->isStruct())
        node->flags |= AST_TRANSIENT;

    // Must invert if commutative operation
    if (!leftTypeInfo->isStruct() && rightTypeInfo->isStruct())
    {
        switch (node->token.id)
        {
        case TokenId::SymPlus:
        case TokenId::SymAsterisk:
            swap(left, right);
            swap(leftTypeInfo, rightTypeInfo);
            node->semFlags |= AST_SEM_INVERSE_PARAMS;
            break;
        }
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
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_TRY_COERCE));
            node->typeInfo = left->typeInfo;
        }

        if (node->token.id == TokenId::SymVertical)
            SWAG_CHECK(resolveBitmaskOr(context, left, right));
        else if (node->token.id == TokenId::SymAmpersand)
            SWAG_CHECK(resolveBitmaskAnd(context, left, right));
        else
            SWAG_CHECK(resolveXor(context, left, right));
        break;

    default:
        return Report::internalError(context->node, "resolveFactorExpression, token not supported");
    }

    // :SpecFuncConstExpr
    if (node->hasSpecialFuncCall() && (node->flags & AST_CONST_EXPR))
    {
        if (leftTypeInfo->isStruct() && !(leftTypeInfo->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
            node->flags &= ~AST_CONST_EXPR;
    }

    if (promotedFrom)
    {
        node->typeInfo               = node->typeInfo->clone();
        node->typeInfo->promotedFrom = promotedFrom;
    }

    return true;
}

bool SemanticJob::resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = CastAst<AstOp>(context->node, AstNodeKind::FactorOp);
    auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, "<<", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, right, CASTFLAG_TRY_COERCE));
    auto module = node->sourceFile->module;

    if (!leftTypeInfo->isNativeIntegerOrRune())
    {
        Diagnostic diag{left, Fmt(Err(Err0170), leftTypeInfo->getDisplayNameC())};
        diag.hint = Diagnostic::isType(leftTypeInfo);
        diag.setRange2(node->token, Hnt(Hnt0061));
        return context->report(diag);
    }

    bool isSmall = node->specFlags & AST_SPEC_OP_SMALL;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOp, leftTypeInfo);
        auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOf, leftTypeInfo);

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 8, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 8)
                    return context->report({right, msg});
                if ((node->computedValue->reg.s8 & 0x80) != (left->computedValue->reg.s8 & 0x80))
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::S16:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 16, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 16)
                    return context->report({right, msg});
                if ((node->computedValue->reg.s16 & 0x8000) != (left->computedValue->reg.s16 & 0x8000))
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::S32:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 32, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 32)
                    return context->report({right, msg});
                if ((node->computedValue->reg.s32 & 0x80000000) != (left->computedValue->reg.s32 & 0x80000000))
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 64, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 64)
                    return context->report({right, msg});
                if ((node->computedValue->reg.s64 & 0x80000000'00000000) != (left->computedValue->reg.s64 & 0x80000000'00000000))
                    return context->report({node, msg1});
            }
            break;

        case NativeTypeKind::U8:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 8, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 8)
                    return context->report({right, msg});
                if (node->computedValue->reg.u8 >> right->computedValue->reg.u32 != left->computedValue->reg.u8)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U16:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 16, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 16)
                    return context->report({right, msg});
                if (node->computedValue->reg.u16 >> right->computedValue->reg.u32 != left->computedValue->reg.u16)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 32, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 32)
                    return context->report({right, msg});
                if (node->computedValue->reg.u32 >> right->computedValue->reg.u32 != left->computedValue->reg.u32)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            ByteCodeRun::executeShiftLeft(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 64, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 64)
                    return context->report({right, msg});
                if (node->computedValue->reg.u64 >> right->computedValue->reg.u32 != left->computedValue->reg.u64)
                    return context->report({node, msg1});
            }
            break;
        default:
            return Report::internalError(context->node, "resolveShiftLeft, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something << 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
        // 0 << something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.s64 = 0;
        }
    }

    return true;
}

bool SemanticJob::resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = CastAst<AstOp>(context->node, AstNodeKind::FactorOp);
    auto leftTypeInfo = TypeManager::concretePtrRefType(left->typeInfo);

    if (leftTypeInfo->isStruct())
    {
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opBinary, ">>", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, right, CASTFLAG_TRY_COERCE));
    auto module = node->sourceFile->module;

    if (!leftTypeInfo->isNativeIntegerOrRune())
    {
        Diagnostic diag{left, Fmt(Err(Err0172), leftTypeInfo->getDisplayNameC())};
        diag.hint = Diagnostic::isType(leftTypeInfo);
        diag.setRange2(node->token, Hnt(Hnt0061));
        return context->report(diag);
    }

    bool isSmall = node->specFlags & AST_SPEC_OP_SMALL;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOp, leftTypeInfo);
        auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOf, leftTypeInfo);

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 8, true, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 8)
                    return context->report({right, msg});
            }
            break;
        case NativeTypeKind::S16:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 16, true, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 16)
                    return context->report({right, msg});
            }
            break;
        case NativeTypeKind::S32:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 32, true, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 32)
                    return context->report({right, msg});
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 64, true, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 64)
                    return context->report({right, msg});
            }
            break;

        case NativeTypeKind::U8:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 8, false, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 8)
                    return context->report({right, msg});
                if (node->computedValue->reg.u8 << right->computedValue->reg.u32 != left->computedValue->reg.u8)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U16:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 16, false, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 16)
                    return context->report({right, msg});
                if (node->computedValue->reg.u16 << right->computedValue->reg.u32 != left->computedValue->reg.u16)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 32, false, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 32)
                    return context->report({right, msg});
                if (node->computedValue->reg.u32 << right->computedValue->reg.u32 != left->computedValue->reg.u32)
                    return context->report({node, msg1});
            }
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            ByteCodeRun::executeShiftRight(&node->computedValue->reg, left->computedValue->reg, right->computedValue->reg, 64, false, isSmall);
            if (module->mustEmitSafetyOF(node))
            {
                if (right->computedValue->reg.u32 >= 64)
                    return context->report({right, msg});
                if (node->computedValue->reg.u64 << right->computedValue->reg.u32 != left->computedValue->reg.u64)
                    return context->report({node, msg1});
            }
            break;
        default:
            return Report::internalError(context->node, "resolveShiftRight, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something >> 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
        }
        // 0 >> something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.s64 = 0;
        }
    }

    return true;
}

bool SemanticJob::resolveShiftExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    if (context->result == ContextResult::Pending)
        return true;

    // :ConcreteRef
    left->typeInfo     = getConcreteTypeUnRef(left, CONCRETE_ALL);
    right->typeInfo    = getConcreteTypeUnRef(right, CONCRETE_ALL);
    auto leftTypeInfo  = left->typeInfo;
    auto rightTypeInfo = right->typeInfo;

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

    node->typeInfo    = g_TypeMgr->promoteUntyped(left->typeInfo);
    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

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
    if (node->hasSpecialFuncCall() && (node->flags & AST_CONST_EXPR))
    {
        if (leftTypeInfo->isStruct() && !(leftTypeInfo->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
            node->flags &= ~AST_CONST_EXPR;
    }

    return true;
}

bool SemanticJob::resolveBoolExpression(SemanticContext* context)
{
    auto node   = context->node;
    auto left   = node->childs[0];
    auto right  = node->childs[1];
    auto module = context->sourceFile->module;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_CHECK(evaluateConstExpression(context, left, right));
    if (context->result == ContextResult::Pending)
        return true;

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
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, left, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, right, CASTFLAG_AUTO_BOOL));

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;

    // In case of && or ||, this is special cause we do not want to evaluate the right part if the left part
    // fails. So we need to do some work once the left part has been emitted
    switch (node->token.id)
    {
    case TokenId::KwdAnd:
        left->allocateExtension(ExtensionKind::ByteCode);
        left->extension->bytecode->byteCodeAfterFct = ByteCodeGenJob::emitLogicalAndAfterLeft;
        break;
    case TokenId::KwdOr:
        left->allocateExtension(ExtensionKind::ByteCode);
        left->extension->bytecode->byteCodeAfterFct = ByteCodeGenJob::emitLogicalOrAfterLeft;
        break;
    }

    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();
        switch (node->token.id)
        {
        case TokenId::KwdAnd:
            node->computedValue->reg.b = left->computedValue->reg.b && right->computedValue->reg.b;
            break;
        case TokenId::KwdOr:
            node->computedValue->reg.b = left->computedValue->reg.b || right->computedValue->reg.b;
            break;
        default:
            return Report::internalError(context->node, "resolveBoolExpression, token not supported");
        }
    }

    else if (node->token.id == TokenId::KwdAnd)
    {
        if (module->mustOptimizeBC(node))
        {
            // false && something => false
            if (left->isConstantFalse())
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.b = false;
            }
            // something && false => false
            else if (right->isConstantFalse())
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.b = false;
            }
            // true && something => something
            else if (left->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
            }
            // something && true => something
            else if (right->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
            }
        }
    }

    else if (node->token.id == TokenId::KwdOr)
    {
        if (module->mustOptimizeBC(node))
        {
            // true || something => true
            if (left->isConstantTrue())
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.b = true;
            }
            // something || true => true
            else if (right->isConstantTrue())
            {
                node->setFlagsValueIsComputed();
                node->computedValue->reg.b = true;
            }
            // false || something => something
            else if (left->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
            }
            // something || false => something
            else if (right->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
            }
        }
    }

    return true;
}
