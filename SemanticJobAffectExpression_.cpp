#include "pch.h"
#include "Ast.h"
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

    auto leftTypeInfo  = left->typeInfo;
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, sourceFile->report({sourceFile, left, format("affect operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(leftTypeInfo->kind != TypeInfoKind::Array, sourceFile->report({sourceFile, left, "affect not allowed on array"}));
    SWAG_VERIFY(left->flags & AST_L_VALUE, sourceFile->report({sourceFile, left, "affect operation not allowed, left expression is not a l-value"}));
    SWAG_VERIFY(!(left->resolvedSymbolOverload->flags & OVERLOAD_CONST), sourceFile->report({sourceFile, left, "affect operation not allowed, left expression is constant"}));

	// Is this an array like affectation ?
    AstPointerDeRef* arrayNode = nullptr;
    if (left->kind == AstNodeKind::IdentifierRef && left->childs.front()->kind == AstNodeKind::ArrayPointerIndex)
    {
        arrayNode = CastAst<AstPointerDeRef>(left->childs.front(), AstNodeKind::ArrayPointerIndex);

		// Add self and value in list of parameters
        if (node->semanticState != AstNodeResolveState::SecondTry)
        {
            arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), right);
            arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), left);
        }
    }

    node->inheritLocation();
    node->typeInfo = g_TypeMgr.typeInfoBool;

    auto tokenId   = node->token.id;
    bool forStruct = leftTypeInfo->kind == TypeInfoKind::Struct;

    switch (tokenId)
    {
    case TokenId::SymEqual:
        if (leftTypeInfo->kind != TypeInfoKind::Native &&
            leftTypeInfo->kind != TypeInfoKind::Pointer &&
            leftTypeInfo->kind != TypeInfoKind::Slice &&
            leftTypeInfo->kind != TypeInfoKind::Lambda &&
            leftTypeInfo->kind != TypeInfoKind::TypeList &&
            leftTypeInfo->kind != TypeInfoKind::Struct &&
            leftTypeInfo->kind != TypeInfoKind::Enum)
            return sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::Lambda &&
            rightTypeInfo->kind != TypeInfoKind::Struct &&
            rightTypeInfo->kind != TypeInfoKind::TypeList)
            return sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())});
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        break;

    case TokenId::SymLowerLowerEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "<<=", left, right));
            break;
        }

    case TokenId::SymGreaterGreaterEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", ">>=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
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
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "/=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
        if (leftTypeInfo->nativeType != NativeType::F32 && leftTypeInfo->nativeType != NativeType::F64)
        {
            return sourceFile->report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymAmpersandEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "&=", left, right));
            break;
        }

    case TokenId::SymVerticalEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "|=", left, right));
            break;
        }

    case TokenId::SymCircumflexEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "^=", left, right));
            break;
        }

    case TokenId::SymTildeEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "~=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
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
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "+=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "+=", left, right));
            break;
        }
    case TokenId::SymMinusEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "-=", left, right));
            break;
        }

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
            SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
            SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftTypeInfo, right));
            if (leftTypeInfo->nativeType == NativeType::Bool ||
                leftTypeInfo->nativeType == NativeType::Char ||
                leftTypeInfo->nativeType == NativeType::String)
            {
                return sourceFile->report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
            }
        }
        break;

    case TokenId::SymPercentEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "%=", left, right));
            break;
        }

    case TokenId::SymAsteriskEqual:
        if (forStruct)
        {
            SWAG_CHECK(resolveUserOp(context, "opAssign", "*=", left, right));
            break;
        }

    default:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
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