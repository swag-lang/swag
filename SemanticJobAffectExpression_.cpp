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

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo, MakeConcrete::FlagAlias);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_VERIFY(left->resolvedSymbolName, context->errorContext.report({sourceFile, left, format("affect operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, context->errorContext.report({sourceFile, left, format("affect operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(leftTypeInfo->kind != TypeInfoKind::Array, context->errorContext.report({sourceFile, left, "affect operation not allowed on array"}));
    SWAG_VERIFY(left->flags & AST_L_VALUE, context->errorContext.report({sourceFile, left, "affect operation not allowed, left expression is not a l-value"}));
    SWAG_VERIFY(!(left->flags & AST_IS_CONST), context->errorContext.report({sourceFile, left, "affect operation not allowed, left expression is immutable"}));

    // Is this an array like affectation ?
    AstPointerDeRef* arrayNode = nullptr;
    if (left->kind == AstNodeKind::IdentifierRef && left->childs.front()->kind == AstNodeKind::ArrayPointerIndex)
    {
        arrayNode = CastAst<AstPointerDeRef>(left->childs.front(), AstNodeKind::ArrayPointerIndex);

        // Add self and value in list of parameters
        if (node->semanticPass == 0)
        {
            arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), right);
            arrayNode->structFlatParams.insert(arrayNode->structFlatParams.begin(), left);
        }
    }

    node->typeInfo = g_TypeMgr.typeInfoBool;

    auto tokenId   = node->token.id;
    bool forStruct = leftTypeInfo->kind == TypeInfoKind::Struct;

    SWAG_CHECK(evaluateConstExpression(context, right));
    if (context->result == SemanticResult::Pending)
        return true;

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
            return context->errorContext.report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::Lambda &&
            rightTypeInfo->kind != TypeInfoKind::Struct &&
            rightTypeInfo->kind != TypeInfoKind::TypeList)
            return context->errorContext.report({sourceFile, right, format("affect not allowed, '%s' is %s", rightTypeInfo->name.c_str(), TypeInfo::getArticleKindName(rightTypeInfo))});
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "=", left, arrayNode->structFlatParams));
            else
            {
                if (rightTypeInfo->kind == TypeInfoKind::Struct)
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right, CASTFLAG_UNCONST));
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == SemanticResult::Pending)
                        return true;
                }
                else
                {
                    SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, left, right));
                }
            }
        }
        else
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right));
        break;

    case TokenId::SymLowerLowerEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "<<=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "<<=", left, right));
            break;
        }

    case TokenId::SymGreaterGreaterEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", ">>=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", ">>=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return context->errorContext.report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymSlashEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "/=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "/=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right));
        if (leftTypeInfo->nativeType != NativeTypeKind::F32 && leftTypeInfo->nativeType != NativeTypeKind::F64)
        {
            return context->errorContext.report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;

    case TokenId::SymAmpersandEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "&=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "&=", left, right));
            break;
        }

    case TokenId::SymVerticalEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "|=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "|=", left, right));
            break;
        }

    case TokenId::SymCircumflexEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "^=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "^=", left, right));
            break;
        }

    case TokenId::SymTildeEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "~=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "~=", left, right));
            break;
        }

        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, right, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return context->errorContext.report({sourceFile, node, format("affect not allowed on type '%s'", leftTypeInfo->name.c_str())});
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
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "-=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "-=", left, right));
            break;
        }

        if (leftTypeInfo->kind == TypeInfoKind::Pointer)
        {
            if (rightTypeInfo->nativeType != NativeTypeKind::S32 &&
                rightTypeInfo->nativeType != NativeTypeKind::S64 &&
                rightTypeInfo->nativeType != NativeTypeKind::U32 &&
                rightTypeInfo->nativeType != NativeTypeKind::U64)
            {
                return context->errorContext.report({sourceFile, node, format("pointer operation not allowed with type '%s'", leftTypeInfo->name.c_str())});
            }

            auto leftPtrType = CastTypeInfo<TypeInfoPointer>(leftTypeInfo, TypeInfoKind::Pointer);
            SWAG_VERIFY(leftPtrType->pointedType->sizeOf > 0, context->errorContext.report({sourceFile, left, format("operation not allowed on %s '%s' because size of pointed type is zero", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        }
        else
        {
            SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
            SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right));
            if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
                leftTypeInfo->nativeType == NativeTypeKind::Char ||
                leftTypeInfo->nativeType == NativeTypeKind::String)
            {
                return context->errorContext.report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
            }
        }
        break;

    case TokenId::SymPercentEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "%=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "%=", left, right));
            break;
        }

    case TokenId::SymAsteriskEqual:
        if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "*=", left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "*=", left, right));
            break;
        }

    default:
        SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(rightTypeInfo), rightTypeInfo->name.c_str())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return context->errorContext.report({sourceFile, node, format("operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
        }
        break;
    }

    node->byteCodeFct = &ByteCodeGenJob::emitAffect;

    SWAG_CHECK(postProcessLeftRight(context, left, right));
    if (context->result == SemanticResult::Pending)
        return true;

    return true;
}