#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"

bool SemanticJob::resolveUnaryOpMinus(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(op->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
        break;
    default:
        return context->errorContext.report({sourceFile, op->token, format("minus operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            op->computedValue.reg.s64 = -op->computedValue.reg.s64;
            if (typeInfo->flags & TYPEINFO_UNTYPED_VALUE)
                static_cast<TypeInfoNative*>(typeInfo)->valueInteger = -static_cast<TypeInfoNative*>(typeInfo)->valueInteger;
            break;
        case NativeTypeKind::F32:
            op->computedValue.reg.f32 = -op->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            op->computedValue.reg.f64 = -op->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpExclam(SemanticContext* context, AstNode* op)
{
    auto typeInfo = TypeManager::concreteType(op->typeInfo);
    SWAG_CHECK(TypeManager::makeCompatibles(&context->errorContext, g_TypeMgr.typeInfoBool, op, CASTFLAG_AUTO_BOOL));

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            op->computedValue.reg.b = !op->computedValue.reg.b;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            op->computedValue.reg.b = op->computedValue.reg.u8 ? false : true;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            op->computedValue.reg.b = op->computedValue.reg.u16 ? false : true;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            op->computedValue.reg.b = op->computedValue.reg.u32 ? false : true;
            break;
		case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            op->computedValue.reg.b = op->computedValue.reg.u64 ? false : true;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpInvert(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(op->typeInfo);

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
    default:
        return context->errorContext.report({sourceFile, op->token, format("bit inversion operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
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
            op->computedValue.reg.u64 = ~op->computedValue.reg.u64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOp(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto op         = node->childs[0];

    node->typeInfo    = op->typeInfo;
    node->byteCodeFct = &ByteCodeGenJob::emitUnaryOp;

    node->inheritOrFlag(op, AST_CONST_EXPR);
    SWAG_CHECK(checkIsConcrete(context, op));
    node->flags |= AST_R_VALUE;

    auto typeInfo = TypeManager::concreteType(op->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        switch (node->token.id)
        {
        case TokenId::SymExclam:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "!", op, nullptr));
            break;
        case TokenId::SymMinus:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "-", op, nullptr));
            break;
        case TokenId::SymTilde:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "~", op, nullptr));
            break;
        }

        node->typeInfo = typeInfo;
        node->flags |= AST_TRANSIENT;
        return true;
    }

    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, node, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(typeInfo), typeInfo->name.c_str())}));

    switch (node->token.id)
    {
    case TokenId::SymExclam:
        SWAG_CHECK(resolveUnaryOpExclam(context, op));
        break;
    case TokenId::SymMinus:
        SWAG_CHECK(resolveUnaryOpMinus(context, op));
        break;
    case TokenId::SymTilde:
        SWAG_CHECK(resolveUnaryOpInvert(context, op));
        break;
    }

    node->typeInfo = op->typeInfo;
	node->castedTypeInfo = op->castedTypeInfo;
    node->inheritComputedValue(op);
    return true;
}
