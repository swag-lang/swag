#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Concat.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "Math.h"

bool SemanticJob::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
    auto module        = sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, "opBinary", "+", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    // Pointer arithmetic
    if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        node->typeInfo = leftTypeInfo;
        SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, "pointer arithmetic not allowed on 'typeinfo'"}));
        SWAG_VERIFY(rightTypeInfo->flags & TYPEINFO_INTEGER, context->report({right, format("pointer arithmetic not allowed with operand type '%s'", rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, left, right, CASTFLAG_COERCE_FULL));
        return true;
    }

    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({right, format("operator '+' not allowed with operand type '%s'", rightTypeInfo->name.c_str())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_FULL));
    leftTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({left, format("operator '+' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (addOverflow(node, left->computedValue.reg.s32, right->computedValue.reg.s32))
                return context->report({node, node->token, "[safety] (s32) '+' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 + right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (addOverflow(node, left->computedValue.reg.s64, right->computedValue.reg.s64))
                return context->report({node, node->token, "[safety] (s64) '+' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 + right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (addOverflow(node, left->computedValue.reg.u32, right->computedValue.reg.u32))
                return context->report({node, node->token, "[safety] (u32) '+' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 + right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (addOverflow(node, left->computedValue.reg.u64, right->computedValue.reg.u64))
                return context->report({node, node->token, "[safety] (u64) '+' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 + right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 + right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpPlus, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 + something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            Ast::releaseNode(left);
        }
        // something + 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
    auto module        = sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "-", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        node->typeInfo = leftTypeInfo;

        // Substract two pointers
        if (rightTypeInfo->kind == TypeInfoKind::Pointer)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
            node->typeInfo = g_TypeMgr.typeInfoInt;
            return true;
        }

        // Pointer arithmetic
        SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, "pointer arithmetic not allowed on 'typeinfo'"}));
        SWAG_VERIFY(rightTypeInfo->flags & TYPEINFO_INTEGER, context->report({right, format("pointer arithmetic not allowed with operand type '%s'", rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, left, right, CASTFLAG_COERCE_FULL));
        return true;
    }

    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({right, format("operator '-' not allowed with operand type '%s'", rightTypeInfo->name.c_str())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_FULL));
    leftTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({left, format("operator '-' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (subOverflow(node, left->computedValue.reg.s32, right->computedValue.reg.s32))
                return context->report({node, node->token, "[safety] (s32) '-' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 - right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (subOverflow(node, left->computedValue.reg.s64, right->computedValue.reg.s64))
                return context->report({node, node->token, "[safety] (s64) '-' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 - right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (subOverflow(node, left->computedValue.reg.u32, right->computedValue.reg.u32))
                return context->report({node, node->token, "[safety] (u32) '-' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 - right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (subOverflow(node, left->computedValue.reg.u64, right->computedValue.reg.u64))
                return context->report({node, node->token, "[safety] (u64) '-' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 - right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 - right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpMinus, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something - 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
    auto module        = sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOpCommutative(context, "opBinary", "*", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_FULL));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, format("operator '*' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (mulOverflow(node, left->computedValue.reg.s32, right->computedValue.reg.s32))
                return context->report({node, node->token, "[safety] (s32) '*' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 * right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (mulOverflow(node, left->computedValue.reg.s64, right->computedValue.reg.s64))
                return context->report({node, node->token, "[safety] (s64) '*' integer overflow"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 * right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (mulOverflow(node, left->computedValue.reg.u32, right->computedValue.reg.u32))
                return context->report({node, node->token, "[safety] (u32) '*' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 * right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (mulOverflow(node, left->computedValue.reg.u64, right->computedValue.reg.u64))
                return context->report({node, node->token, "[safety] (u64) '*' integer overflow"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 * right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 * right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpMul, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something * 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.s64 = 0;
        }
        // 1 * something => something
        else if (left->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            Ast::releaseNode(left);
        }
        // something * 1 => something
        else if (right->isConstant1())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "/", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_SAMESIGN));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, format("operator '/' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue.reg.s32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 / right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue.reg.s64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 / right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (right->computedValue.reg.u32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 / right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue.reg.u64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 / right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            if (right->computedValue.reg.f32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.f32 = left->computedValue.reg.f32 / right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            if (right->computedValue.reg.f64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.f64 = left->computedValue.reg.f64 / right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpDiv, type not supported");
        }
    }
    else if (right->isConstant0())
        return context->report({right, right->token, "division by zero"});

    return true;
}

bool SemanticJob::resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "%", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_FULL));
    node->typeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, format("operator '%' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue.reg.s32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 % right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue.reg.s64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 % right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (right->computedValue.reg.u32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 % right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue.reg.u64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 % right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBinaryOpModulo, type not supported");
        }
    }
    else if (right->isConstant0())
        return context->report({right, right->token, "division by zero"});

    return true;
}

bool SemanticJob::resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    auto module       = node->sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "|", nullptr, left, right, false));
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
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, format("operator '|' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b || right->computedValue.reg.b;
            break;

        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue.reg.u64 = left->computedValue.reg.u8 | right->computedValue.reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue.reg.u64 = left->computedValue.reg.u16 | right->computedValue.reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.u64 = left->computedValue.reg.u32 | right->computedValue.reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 | right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBitmaskOr, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 | something => something
        if (left->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(left);
            Ast::releaseNode(left);
        }
        // something | 0 => something
        else if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
        // something | 0xff (type) => 0xff (type)
        else if (right->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue.reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue.reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue.reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue.reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue.reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
        // 0xff (type) | something => 0xff (type)
        else if (left->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue.reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue.reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue.reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue.reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setFlagsValueIsComputed();
                node->computedValue.reg.u64 = 0xFFFFFFFFFFFFFFFF;
            }
        }
    }

    return true;
}

bool SemanticJob::resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    auto module       = node->sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "&", nullptr, left, right, false));
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
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({node, format("operator '&' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b && right->computedValue.reg.b;
            break;

        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            node->computedValue.reg.u64 = left->computedValue.reg.u8 & right->computedValue.reg.u8;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            node->computedValue.reg.u64 = left->computedValue.reg.u16 & right->computedValue.reg.u16;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.u64 = left->computedValue.reg.u32 & right->computedValue.reg.u32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 & right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBitmaskAnd, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // 0 & something => 0
        // something & 0 => 0
        if (left->isConstant0() || right->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.s64 = 0;
        }

        // something & 0xFF (type) => something
        else if (right->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && right->computedValue.reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && right->computedValue.reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && right->computedValue.reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && right->computedValue.reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                Ast::releaseNode(right);
            }
        }

        // 0xFF (type) & something => something
        else if (left->flags & AST_VALUE_COMPUTED)
        {
            if ((leftTypeInfo->sizeOf == 1 && left->computedValue.reg.u8 == 0xFF) ||
                (leftTypeInfo->sizeOf == 2 && left->computedValue.reg.u16 == 0xFFFF) ||
                (leftTypeInfo->sizeOf == 4 && left->computedValue.reg.u32 == 0xFFFFFFFF) ||
                (leftTypeInfo->sizeOf == 8 && left->computedValue.reg.u64 == 0xFFFFFFFFFFFFFFFF))
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                Ast::releaseNode(left);
            }
        }
    }

    return true;
}

bool SemanticJob::resolveTilde(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    SWAG_VERIFY(left->flags & AST_VALUE_COMPUTED, context->report({left, "expression cannot be evaluated at compile time"}));
    SWAG_VERIFY(right->flags & AST_VALUE_COMPUTED, context->report({right, "expression cannot be evaluated at compile time"}));
    left->computedValue.text  = Ast::literalToString(left->typeInfo, left->computedValue.text, left->computedValue.reg);
    right->computedValue.text = Ast::literalToString(right->typeInfo, right->computedValue.text, right->computedValue.reg);
    node->computedValue.text  = left->computedValue.text + right->computedValue.text.c_str();
    node->typeInfo            = g_TypeMgr.typeInfoString;
    node->setFlagsValueIsComputed();
    return true;
}

bool SemanticJob::resolveXor(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteReferenceType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "^", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::Char:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({context->node, format("operator '^' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            node->computedValue.reg.s64 = left->computedValue.reg.s32 ^ right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 ^ right->computedValue.reg.s64;
            break;
        default:
            return internalError(context, "resolveXor, type not supported");
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
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo, CONCRETE_ALIAS);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo, CONCRETE_ALIAS);
    if (leftTypeInfo->kind == TypeInfoKind::Enum || rightTypeInfo->kind == TypeInfoKind::Enum)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

        bool ok = true;
        if (!(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
            ok = false;
        if (node->token.id != TokenId::SymVertical &&
            node->token.id != TokenId::SymAmpersand &&
            node->token.id != TokenId::SymCircumflex)
            ok = false;
        if (!ok)
            return notAllowed(context, node, leftTypeInfo);
        isEnumFlags = true;
    }

    leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

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

    // Cannot compare tuples
    if (leftTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({left, format("invalid operation '%s' on a tuple type", node->token.text.c_str())});
    if (rightTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
        return context->report({right, format("invalid operation '%s' on a tuple type", node->token.text.c_str())});

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    if (mustPromote)
        TypeManager::promote(left, right);

    // Must do move and not copy
    if (leftTypeInfo->kind == TypeInfoKind::Struct || rightTypeInfo->kind == TypeInfoKind::Struct)
        node->flags |= AST_TRANSIENT;

    // Must invert if commutative operation
    if (leftTypeInfo->kind != TypeInfoKind::Struct && rightTypeInfo->kind == TypeInfoKind::Struct)
    {
        switch (node->token.id)
        {
        case TokenId::SymPlus:
        case TokenId::SymPlusPercent:
        case TokenId::SymAsterisk:
        case TokenId::SymAsteriskPercent:
            swap(left, right);
            swap(leftTypeInfo, rightTypeInfo);
            node->semFlags |= AST_SEM_INVERSE_PARAMS;
            break;
        }
    }

    switch (node->token.id)
    {
    case TokenId::SymPlus:
    case TokenId::SymPlusPercent:
        SWAG_CHECK(resolveBinaryOpPlus(context, left, right));
        break;
    case TokenId::SymMinus:
    case TokenId::SymMinusPercent:
        SWAG_CHECK(resolveBinaryOpMinus(context, left, right));
        break;
    case TokenId::SymAsterisk:
    case TokenId::SymAsteriskPercent:
        SWAG_CHECK(resolveBinaryOpMul(context, left, right));
        break;
    case TokenId::SymSlash:
        SWAG_CHECK(resolveBinaryOpDiv(context, left, right));
        break;
    case TokenId::SymPercent:
        SWAG_CHECK(resolveBinaryOpModulo(context, left, right));
        break;
    case TokenId::SymTilde:
        SWAG_CHECK(resolveTilde(context, left, right));
        break;

    case TokenId::SymVertical:
    case TokenId::SymAmpersand:
    case TokenId::SymCircumflex:
        if (leftTypeInfo->kind != TypeInfoKind::Struct)
        {
            SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right, CASTFLAG_COERCE_SAMESIGN));
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
        return internalError(context, "resolveFactorExpression, token not supported");
    }

    return true;
}

bool SemanticJob::resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
    auto module        = node->sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "<<", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({left, format("operator '<<' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeTypeKind::U32:
        break;
    default:
        return context->report({right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue.reg.u32 >= 32)
                return context->report({node, "[safety] (32 bits) '<<' shift operand is greater than '31'"});
            node->computedValue.reg.u64 = left->computedValue.reg.s32 << right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (right->computedValue.reg.u32 >= 32)
                return context->report({node, "[safety] (32 bits) '<<' shift operand is greater than '31'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 << right->computedValue.reg.u32;
            if (module->mustEmitSafetyOF(node))
            {
                if (node->computedValue.reg.u32 >> right->computedValue.reg.u32 != left->computedValue.reg.u32)
                    return context->report({node, "[safety] (32 bits) '<<' shift overflow"});
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue.reg.u32 >= 64)
                return context->report({node, "[safety] (64 bits) '<<' shift operand is greater than '64'"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 << right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue.reg.u32 >= 64)
                return context->report({node, "[safety] (64 bits) '<<' shift operand is greater than '64'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 << right->computedValue.reg.u32;
            if (module->mustEmitSafetyOF(node))
            {
                if (node->computedValue.reg.u64 >> right->computedValue.reg.u32 != left->computedValue.reg.u64)
                    return context->report({node, "[safety] (64 bits) '<<' shift overflow"});
            }
            break;
        default:
            return internalError(context, "resolveShiftLeft, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something << 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
        // 0 << something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.s64 = 0;
        }
    }

    return true;
}

bool SemanticJob::resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
    auto module        = node->sourceFile->module;

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", ">>", nullptr, left, right, false));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({left, format("operator '>>' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeTypeKind::U32:
        break;
    default:
        return context->report({right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            if (right->computedValue.reg.u32 >= 32)
                return context->report({node, "[safety] (32 bits) '>>' shift operand is greater than '31'"});
            node->computedValue.reg.s64 = left->computedValue.reg.s32 >> right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (right->computedValue.reg.u32 >= 32)
                return context->report({node, "[safety] (32 bits) '>>' shift operand is greater than '31'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u32 >> right->computedValue.reg.u32;
            if (module->mustEmitSafetyOF(node))
            {
                if (node->computedValue.reg.u32 << right->computedValue.reg.u32 != left->computedValue.reg.u32)
                    return context->report({node, "[safety] (32 bits) '>>' shift overflow"});
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (right->computedValue.reg.u32 >= 64)
                return context->report({node, "[safety] (64 bits) '>>' shift operand is greater then '63'"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 >> right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            if (right->computedValue.reg.u32 >= 64)
                return context->report({node, "[safety] (64 bits) '>>' shift operand is greater then '63'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 >> right->computedValue.reg.u32;
            if (module->mustEmitSafetyOF(node))
            {
                if (node->computedValue.reg.u64 << right->computedValue.reg.u32 != left->computedValue.reg.u64)
                    return context->report({node, "[safety] (64 bits) '>>' shift overflow"});
            }
            break;
        default:
            return internalError(context, "resolveShiftRight, type not supported");
        }
    }
    else if (module->mustOptimizeBC(node))
    {
        // something >> 0 => something
        if (right->isConstant0())
        {
            node->setPassThrough();
            Ast::removeFromParent(right);
            Ast::releaseNode(right);
        }
        // 0 >> something => 0
        else if (left->isConstant0())
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.s64 = 0;
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

    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

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

    SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, nullptr, right, CASTFLAG_COERCE_SAMESIGN));
    TypeManager::promoteOne(left, right);

    node->typeInfo = left->typeInfo;

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    switch (node->token.id)
    {
    case TokenId::SymLowerLower:
    case TokenId::SymLowerLowerPercent:
        SWAG_CHECK(resolveShiftLeft(context, left, right));
        break;
    case TokenId::SymGreaterGreater:
    case TokenId::SymGreaterGreaterPercent:
        SWAG_CHECK(resolveShiftRight(context, left, right));
        break;
    default:
        return internalError(context, "resolveShiftExpression, token not supported");
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

    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

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

    node->typeInfo = g_TypeMgr.typeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, left, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, right, CASTFLAG_AUTO_BOOL));

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;

    // In case of && or ||, this is special cause we do not want to evaluate the right part if the left part
    // fails. So we need to do some work once the left part has been emitted
    switch (node->token.id)
    {
    case TokenId::SymAmpersandAmpersand:
        left->allocateExtension();
        left->extension->byteCodeAfterFct = ByteCodeGenJob::emitLogicalAndAfterLeft;
        break;
    case TokenId::SymVerticalVertical:
        left->allocateExtension();
        left->extension->byteCodeAfterFct = ByteCodeGenJob::emitLogicalOrAfterLeft;
        break;
    }

    node->inheritAndFlag2(AST_CONST_EXPR, AST_R_VALUE);
    node->inheritOrFlag(AST_SIDE_EFFECTS);

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->setFlagsValueIsComputed();
        switch (node->token.id)
        {
        case TokenId::SymAmpersandAmpersand:
            node->computedValue.reg.b = left->computedValue.reg.b && right->computedValue.reg.b;
            break;
        case TokenId::SymVerticalVertical:
            node->computedValue.reg.b = left->computedValue.reg.b || right->computedValue.reg.b;
            break;
        default:
            return internalError(context, "resolveBoolExpression, token not supported");
        }
    }

    else if (node->token.id == TokenId::SymAmpersandAmpersand)
    {
        if (module->mustOptimizeBC(node))
        {
            // false && something => false
            if (left->isConstantFalse())
            {
                node->computedValue.reg.b = false;
                node->setFlagsValueIsComputed();
            }
            // something && false => false
            else if (right->isConstantFalse())
            {
                node->computedValue.reg.b = false;
                node->setFlagsValueIsComputed();
            }
            // true && something => something
            else if (left->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                Ast::releaseNode(left);
            }
            // something && true => something
            else if (right->isConstantTrue())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                Ast::releaseNode(right);
            }
        }
    }

    else if (node->token.id == TokenId::SymVerticalVertical)
    {
        if (module->mustOptimizeBC(node))
        {
            // true || something => true
            if (left->isConstantTrue())
            {
                node->computedValue.reg.b = true;
                node->setFlagsValueIsComputed();
            }
            // something || true => true
            else if (right->isConstantTrue())
            {
                node->computedValue.reg.b = true;
                node->setFlagsValueIsComputed();
            }
            // false || something => something
            else if (left->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(left);
                Ast::releaseNode(left);
            }
            // something || false => something
            else if (right->isConstantFalse())
            {
                node->setPassThrough();
                Ast::removeFromParent(right);
                Ast::releaseNode(right);
            }
        }
    }

    return true;
}
