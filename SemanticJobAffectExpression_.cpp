#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveMove(SemanticContext* context)
{
    auto node  = context->node;
    auto right = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, right));
    node->inheritOrFlag(right, AST_NO_LEFT_DROP | AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
    node->typeInfo    = right->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    if (node->flags & AST_FORCE_MOVE)
    {
        SWAG_VERIFY(!right->typeInfo->isConst(), context->report({right, "'move' cannot be applied to a constant expression"}));
    }

    return true;
}

bool SemanticJob::resolveAffect(SemanticContext* context)
{
    auto node    = context->node;
    auto left    = node->childs[0];
    auto right   = node->childs[1];
    auto tokenId = node->token.id;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(left->resolvedSymbolName, context->report({left, "affect operation not allowed"}));
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, context->report({left, "affect operation not allowed"}));
    SWAG_VERIFY(left->flags & AST_L_VALUE, context->report({left, "affect operation not allowed, left expression is not a l-value"}));
    SWAG_VERIFY(!(left->flags & AST_IS_CONST), context->report({left, "affect operation not allowed, left expression is immutable"}));

    // Special case for enum : nothing is possible, except for flags
    bool forEnumFlags  = false;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo, CONCRETE_ALIAS);
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
                return notAllowed(context, node, leftTypeInfo);
            forEnumFlags = true;
        }
    }

    rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    SWAG_VERIFY(leftTypeInfo->kind != TypeInfoKind::Array, context->report({left, "affect operation not allowed on type array"}));
    SWAG_VERIFY(!rightTypeInfo->isNative(NativeTypeKind::Void), context->report({right, "cannot affect an expression of type 'void'"}));

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isNative(NativeTypeKind::Any) && node->token.id != TokenId::SymEqual)
    {
        return context->report({node, format("'%s' operation not allowed on type 'any'", node->token.text.c_str())});
    }

    // Is this an array like affectation ?
    AstArrayPointerIndex* arrayNode = nullptr;
    if (left->kind == AstNodeKind::IdentifierRef && left->childs.back()->kind == AstNodeKind::ArrayPointerIndex)
    {
        arrayNode = CastAst<AstArrayPointerIndex>(left->childs.back(), AstNodeKind::ArrayPointerIndex);
        if (arrayNode->array->typeInfo->kind != TypeInfoKind::Struct)
            arrayNode = nullptr;

        // Add self and value in list of parameters
        if (arrayNode)
        {
            if (!(node->doneFlags & AST_DONE_FLAT_PARAMS))
            {
                arrayNode->structFlatParams.push_back(right);
                arrayNode->structFlatParams.push_front(left);
                node->doneFlags |= AST_DONE_FLAT_PARAMS;
            }
        }
    }

    node->typeInfo = g_TypeMgr.typeInfoBool;

    bool forStruct = leftTypeInfo->kind == TypeInfoKind::Struct;
    bool forTuple  = leftTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE;

    SWAG_CHECK(evaluateConstExpression(context, right));
    if (context->result == ContextResult::Pending)
        return true;

    // Cast from struct to interface : need to wait for all interfaces to be registered
    if (tokenId == TokenId::SymEqual)
    {
        if (leftTypeInfo->kind == TypeInfoKind::Interface && rightTypeInfo->kind == TypeInfoKind::Struct)
        {
            context->job->waitForAllStructInterfaces(rightTypeInfo);
            if (context->result == ContextResult::Pending)
                return true;
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST));
        }
    }

    switch (tokenId)
    {
    case TokenId::SymEqual:
        if (leftTypeInfo->kind != TypeInfoKind::Native &&
            leftTypeInfo->kind != TypeInfoKind::Pointer &&
            leftTypeInfo->kind != TypeInfoKind::Reference &&
            leftTypeInfo->kind != TypeInfoKind::Slice &&
            leftTypeInfo->kind != TypeInfoKind::Lambda &&
            leftTypeInfo->kind != TypeInfoKind::TypeListTuple &&
            leftTypeInfo->kind != TypeInfoKind::TypeListArray &&
            leftTypeInfo->kind != TypeInfoKind::Struct &&
            leftTypeInfo->kind != TypeInfoKind::Interface &&
            leftTypeInfo->kind != TypeInfoKind::TypeSet &&
            leftTypeInfo->kind != TypeInfoKind::Enum)
            return context->report({left, format("affect not allowed on %s '%s'", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())});
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Reference &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::Lambda &&
            rightTypeInfo->kind != TypeInfoKind::Struct &&
            rightTypeInfo->kind != TypeInfoKind::Interface &&
            rightTypeInfo->kind != TypeInfoKind::TypeSet &&
            rightTypeInfo->kind != TypeInfoKind::TypeListTuple &&
            rightTypeInfo->kind != TypeInfoKind::TypeListArray)
            return context->report({right, format("affect not allowed, '%s' is %s", rightTypeInfo->name.c_str(), TypeInfo::getArticleKindName(rightTypeInfo))});

        if (forStruct)
        {
            if (arrayNode)
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, ISSAME_CAST))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else if (forTuple)
                {
                    return context->report({node, node->token, "invalid operation on a tuple type"});
                }
                else
                {
                    if (!hasUserOp(context, "opIndexAffect", left))
                    {
                        Utf8 msg = format("'%s[index] = %s' is impossible because special function 'opIndexAffect' cannot be found in '%s'", leftTypeInfo->name.c_str(), rightTypeInfo->name.c_str(), leftTypeInfo->name.c_str());
                        return context->report({node, msg});
                    }

                    SWAG_CHECK(resolveUserOp(context, "opIndexAffect", nullptr, nullptr, left, arrayNode->structFlatParams, false));
                }
            }
            else
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, ISSAME_CAST))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else if (rightTypeInfo->isInitializerList())
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST));
                }
                else
                {
                    if (leftTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
                        return context->report({node, "affect to tuple is not possible because right expression is not compatible"});

                    if (!hasUserOp(context, "opAffect", left))
                    {
                        Utf8 msg = format("'%s = %s' is impossible because special function 'opAffect' cannot be found in '%s'", leftTypeInfo->name.c_str(), rightTypeInfo->name.c_str(), leftTypeInfo->name.c_str());
                        return context->report({node, msg});
                    }

                    SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, nullptr, left, right, false));
                }
            }

            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, CASTFLAG_AUTO_BOOL | CASTFLAG_COERCE_SAMESIGN));
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        if (forTuple)
            return context->report({node, node->token, "invalid operation on a tuple type"});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, left, right, CASTFLAG_COERCE_SAMESIGN));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
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
        if (forTuple)
            return context->report({node, node->token, "invalid operation on a tuple type"});
        else if (forStruct)
        {
            const char* op = "&=";
            if (tokenId == TokenId::SymVerticalEqual)
                op = "|=";
            else if (tokenId == TokenId::SymCircumflexEqual)
                op = "^=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(forEnumFlags || checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_COERCE_SAMESIGN));
        if (/*leftTypeInfo->nativeType == NativeTypeKind::Bool ||*/
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
        if (forTuple)
            return context->report({node, node->token, "invalid operation on a tuple type"});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPlusEqual ? "+=" : "-=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, nullptr, left, right, false));
            break;
        }

        // Pointer arithmetic
        if (leftTypeInfo->kind == TypeInfoKind::Pointer)
        {
            SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, "pointer arithmetic not allowed on 'TypeInfo'"}));

            SWAG_CHECK(TypeManager::promoteOne(context, right));
            rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

            if (rightTypeInfo->nativeType != NativeTypeKind::S32 &&
                rightTypeInfo->nativeType != NativeTypeKind::S64 &&
                rightTypeInfo->nativeType != NativeTypeKind::U32 &&
                rightTypeInfo->nativeType != NativeTypeKind::U64 &&
                rightTypeInfo->nativeType != NativeTypeKind::Int &&
                rightTypeInfo->nativeType != NativeTypeKind::UInt)
            {
                return context->report({right, format("pointer arithmetic not allowed with type '%s'", rightTypeInfo->name.c_str())});
            }

            auto leftPtrType = CastTypeInfo<TypeInfoPointer>(leftTypeInfo, TypeInfoKind::Pointer);
            SWAG_VERIFY(leftPtrType->pointedType->sizeOf > 0, context->report({left, format("operation not allowed on %s '%s' because size of pointed type is zero", TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->name.c_str())}));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_COERCE_FULL));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymSlashEqual:
        if (forTuple)
            return context->report({node, node->token, "invalid operation on a tuple type"});
        else if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "/=", nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "/=", nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_COERCE_SAMESIGN));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPercentEqual:
    case TokenId::SymAsteriskEqual:
        if (forTuple)
            return context->report({node, node->token, "invalid operation on a tuple type"});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPercentEqual ? "%=" : "*=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_COERCE_FULL));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    default:
        return internalError(context, "resolveAffect, invalid token");
    }

    node->byteCodeFct = ByteCodeGenJob::emitAffect;
    return true;
}