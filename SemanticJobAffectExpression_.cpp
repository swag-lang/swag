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
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, sourceFile->report({sourceFile, left, format("affect operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(leftTypeInfo->kind != TypeInfoKind::Array, sourceFile->report({sourceFile, left, "affect not allowed on array"}));
    SWAG_VERIFY(left->flags & AST_L_VALUE, sourceFile->report({sourceFile, left, "affect operation not allowed, left expression is not a l-value"}));
	SWAG_VERIFY(!(left->resolvedSymbolOverload->flags & OVERLOAD_CONST), sourceFile->report({ sourceFile, left, "affect operation not allowed, left expression is constant" }));

    node->inheritLocation();
    node->typeInfo = g_TypeMgr.typeInfoBool;

    switch (node->token.id)
    {
    case TokenId::SymEqual:
        if (leftTypeInfo->kind != TypeInfoKind::Native &&
            leftTypeInfo->kind != TypeInfoKind::Pointer &&
            leftTypeInfo->kind != TypeInfoKind::Slice)
            return sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())});
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::TypeList)
            return sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())});
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoU32, right));
        if (leftTypeInfo->nativeType == NativeType::Bool ||
            leftTypeInfo->nativeType == NativeType::Char ||
            leftTypeInfo->nativeType == NativeType::String ||
            leftTypeInfo->nativeType == NativeType::F32 ||
            leftTypeInfo->nativeType == NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymSlashEqual:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType != NativeType::F32 && leftTypeInfo->nativeType != NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
    case TokenId::SymTildeEqual:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType == NativeType::Bool ||
            leftTypeInfo->nativeType == NativeType::Char ||
            leftTypeInfo->nativeType == NativeType::String ||
            leftTypeInfo->nativeType == NativeType::F32 ||
            leftTypeInfo->nativeType == NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
        if (leftTypeInfo->kind == TypeInfoKind::Pointer)
        {
            if (rightTypeInfo->nativeType != NativeType::S32 &&
                rightTypeInfo->nativeType != NativeType::S64 &&
                rightTypeInfo->nativeType != NativeType::U32 &&
                rightTypeInfo->nativeType != NativeType::U64)
            {
                return sourceFile->report({sourceFile, node, format("pointer operation not allowed with type '%s'", leftTypeInfo->name.c_str())});
            }
        }
        else
        {
            SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
            SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
            if (leftTypeInfo->nativeType == NativeType::Bool ||
                leftTypeInfo->nativeType == NativeType::Char ||
                leftTypeInfo->nativeType == NativeType::String)
            {
                return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
            }
        }
        break;

    default:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));
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