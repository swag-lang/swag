#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"

bool SemanticJob::resolveUnaryOpMinus(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(op->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, op->token, format("minus operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            op->computedValue.reg.s64 = -op->computedValue.reg.s64;
            break;
        case NativeType::F32:
            op->computedValue.reg.f32 = -op->computedValue.reg.f32;
            break;
        case NativeType::F64:
            op->computedValue.reg.f64 = -op->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpExclam(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(op->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        break;
    default:
        return sourceFile->report({sourceFile, op->token, format("boolean inversion not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeType::Bool:
            op->computedValue.reg.b = !op->computedValue.reg.b;
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
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, op->token, format("bit inversion operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
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
    node->inheritLocation();

    auto typeInfo = TypeManager::concreteType(op->typeInfo);
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, node, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(typeInfo), typeInfo->name.c_str())}));

    node->typeInfo = op->typeInfo;
    node->inheritAndFlag(op, AST_CONST_EXPR);
    node->byteCodeFct = &ByteCodeGenJob::emitUnaryOp;

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

    node->inheritComputedValue(op);
    return true;
}
