#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"

bool SemanticJob::resolveAffect(SemanticContext* context)
{
    auto node       = context->node;
    auto left       = node->childs[0];
    auto right      = node->childs[1];
    auto sourceFile = context->sourceFile;

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));

    node->inheritLocation();
    node->typeInfo = g_TypeMgr.typeInfoBool;

    switch (node->token.id)
    {
    case TokenId::SymEqual:
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoU32, right));
        if (leftTypeInfo->nativeType == NativeType::Bool ||
            leftTypeInfo->nativeType == NativeType::Char ||
            leftTypeInfo->nativeType == NativeType::String ||
            leftTypeInfo->nativeType == NativeType::F32 ||
            leftTypeInfo->nativeType == NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymSlashEqual:
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType != NativeType::F32 && leftTypeInfo->nativeType != NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
    case TokenId::SymTildeEqual:
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType == NativeType::Bool ||
            leftTypeInfo->nativeType == NativeType::Char ||
            leftTypeInfo->nativeType == NativeType::String ||
            leftTypeInfo->nativeType == NativeType::F32 ||
            leftTypeInfo->nativeType == NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    default:
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType == NativeType::Bool ||
            leftTypeInfo->nativeType == NativeType::Char ||
            leftTypeInfo->nativeType == NativeType::String)
        {
            return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;
    }

    node->byteCodeFct = &ByteCodeGenJob::emitAffect;
    return true;
}