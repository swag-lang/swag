#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"

static bool checkNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, context->errorContext.report({sourceFile, node, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(typeInfo), typeInfo->name.c_str())}));
    return true;
}

static bool notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto sourceFile = context->sourceFile;
    return context->errorContext.report({sourceFile, node, format("operation not allowed on type '%s'", typeInfo->name.c_str())});
}

bool SemanticJob::resolveAffect(SemanticContext* context)
{
    auto node       = context->node;
    auto left       = node->childs[0];
    auto right      = node->childs[1];
    auto sourceFile = context->sourceFile;
    auto tokenId    = node->token.id;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    SWAG_VERIFY(left->resolvedSymbolName, context->errorContext.report({sourceFile, left, "affect operation not allowed"}));
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, context->errorContext.report({sourceFile, left, "affect operation not allowed"}));
    SWAG_VERIFY(left->flags & AST_L_VALUE, context->errorContext.report({sourceFile, left, "affect operation not allowed, left expression is not a l-value"}));
    SWAG_VERIFY(!(left->flags & AST_IS_CONST), context->errorContext.report({sourceFile, left, "affect operation not allowed, left expression is immutable"}));

    // Special case for enum : nothing is possible, except for flags
    bool forEnumFlags  = false;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo, MakeConcrete::FlagAlias);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo, MakeConcrete::FlagAlias);

    if (node->token.id != TokenId::SymEqual)
    {
        if (leftTypeInfo->kind == TypeInfoKind::Enum || rightTypeInfo->kind == TypeInfoKind::Enum)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

            bool ok = true;
            if (!(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
                ok = false;
            if (node->token.id != TokenId::SymVerticalEqual &&
                node->token.id != TokenId::SymAmpersandEqual &&
                node->token.id != TokenId::SymCircumflexEqual)
                ok = false;
            if (!ok)
                return context->errorContext.report({sourceFile, node->token, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
            forEnumFlags = true;
        }
    }

    rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    SWAG_VERIFY(leftTypeInfo->kind != TypeInfoKind::Array, context->errorContext.report({sourceFile, left, "affect operation not allowed on array"}));

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isNative(NativeTypeKind::Any) && node->token.id != TokenId::SymEqual)
    {
        return context->errorContext.report({sourceFile, node, format("'%s' operation not allowed on type 'any'", node->token.text.c_str())});
    }

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
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST));
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == SemanticResult::Pending)
                        return true;
                }
                else if (rightTypeInfo->kind == TypeInfoKind::TypeList)
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST));
                }
                else
                {
                    SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, left, right));
                }
            }
        }
        else
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right));
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, left, right));
            break;
        }

        SWAG_CHECK(checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
			return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
    case TokenId::SymTildeEqual:
        if (forStruct)
        {
            const char* op = "&=";
            if (tokenId == TokenId::SymVerticalEqual)
                op = "|=";
            else if (tokenId == TokenId::SymCircumflexEqual)
                op = "^=";
            else if (tokenId == TokenId::SymTildeEqual)
                op = "~=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, left, right));
            break;
        }

        SWAG_CHECK(forEnumFlags || checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(forEnumFlags || checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
			return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
        if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPlusEqual ? "+=" : "-=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, left, right));
            break;
        }

        // Pointer arythmetic
        if (leftTypeInfo->kind == TypeInfoKind::Pointer)
        {
            if (rightTypeInfo->nativeType != NativeTypeKind::S32 &&
                rightTypeInfo->nativeType != NativeTypeKind::S64 &&
                rightTypeInfo->nativeType != NativeTypeKind::U32 &&
                rightTypeInfo->nativeType != NativeTypeKind::U64)
            {
                return context->errorContext.report({sourceFile, right, format("pointer operation not allowed on type '%s'", rightTypeInfo->name.c_str())});
            }

            auto leftPtrType = CastTypeInfo<TypeInfoPointer>(leftTypeInfo, TypeInfoKind::Pointer);
            SWAG_VERIFY(leftPtrType->finalType->sizeOf > 0, context->errorContext.report({sourceFile, left, format("operation not allowed on %s '%s' because size of pointed type is zero", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
            break;
        }

        SWAG_CHECK(checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
			return notAllowed(context, node, leftTypeInfo);
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

        SWAG_CHECK(checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right));
        if (leftTypeInfo->nativeType != NativeTypeKind::F32 &&
            leftTypeInfo->nativeType != NativeTypeKind::F64)
        {
			return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPercentEqual:
    case TokenId::SymAsteriskEqual:
        if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPercentEqual ? "%=" : "*=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, left, right));
            break;
        }

        SWAG_CHECK(checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    default:
        SWAG_CHECK(checkNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::Char ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;
    }

    node->byteCodeFct = &ByteCodeGenJob::emitAffect;
    return true;
}