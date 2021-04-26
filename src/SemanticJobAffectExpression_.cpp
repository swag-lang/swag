#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"

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
        SWAG_VERIFY(!right->typeInfo->isConst(), context->report({right, Msg0559}));
    }

    return true;
}

bool SemanticJob::checkRelativePointerOverflow(SemanticContext* context, TypeInfo* typeInfo, AstNode* right)
{
    if (typeInfo->kind != TypeInfoKind::Pointer)
        return true;
    if (!(typeInfo->flags & TYPEINFO_RELATIVE))
        return true;
    if (!(right->flags & AST_VALUE_COMPUTED))
        return true;

    ComputedValue& value = right->computedValue;
    switch (typeInfo->sizeOf)
    {
    case 1:
        SWAG_VERIFY(value.reg.s64 >= INT8_MIN && value.reg.s64 <= INT8_MAX, context->report({right, format(Msg0560, right->computedValue.reg.s64)}));
        break;
    case 2:
        SWAG_VERIFY(value.reg.s64 >= INT16_MIN && value.reg.s64 <= INT16_MAX, context->report({right, format(Msg0561, right->computedValue.reg.s64)}));
        break;
    case 4:
        SWAG_VERIFY(value.reg.s64 >= INT32_MIN && value.reg.s64 <= INT32_MAX, context->report({right, format(Msg0562, right->computedValue.reg.s64)}));
        break;
    case 8:
        SWAG_VERIFY(value.reg.s64 >= INT64_MIN && value.reg.s64 <= INT64_MAX, context->report({right, format(Msg0563, right->computedValue.reg.s64)}));
        break;
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

    if (!(left->flags & AST_L_VALUE))
    {
        if (left->resolvedSymbolOverload && left->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
            return context->report({left, Msg0564});
        return context->report({left, Msg0565});
    }

    SWAG_VERIFY(left->resolvedSymbolName, context->report({left, Msg0566}));
    SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, context->report({left, Msg0567}));
    SWAG_VERIFY(!(left->flags & AST_IS_CONST), context->report({left, Msg0568}));

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

    SWAG_VERIFY(!rightTypeInfo->isNative(NativeTypeKind::Void), context->report({right, Msg0569}));

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isNative(NativeTypeKind::Any) && node->token.id != TokenId::SymEqual)
    {
        return context->report({node, format(Msg0570, node->token.text.c_str())});
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
            leftTypeInfo->kind != TypeInfoKind::Array &&
            leftTypeInfo->kind != TypeInfoKind::Enum)
            return context->report({left, format(Msg0571, TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->getDisplayName().c_str())});
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Reference &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::Lambda &&
            rightTypeInfo->kind != TypeInfoKind::Struct &&
            rightTypeInfo->kind != TypeInfoKind::Interface &&
            rightTypeInfo->kind != TypeInfoKind::TypeSet &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::TypeListTuple &&
            rightTypeInfo->kind != TypeInfoKind::TypeListArray)
            return context->report({right, format(Msg0572, rightTypeInfo->getDisplayName().c_str(), TypeInfo::getArticleKindName(rightTypeInfo))});

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
                    return context->report({node, node->token, Msg0573});
                }
                else
                {
                    if (!hasUserOp(context, "opIndexAffect", left))
                    {
                        Utf8 msg = format("'%s[index] = %s' is impossible because special function 'opIndexAffect' cannot be found in '%s'", leftTypeInfo->getDisplayName().c_str(), rightTypeInfo->getDisplayName().c_str(), leftTypeInfo->getDisplayName().c_str());
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
                        return context->report({node, Msg0574});

                    if (!hasUserOp(context, "opAffect", left))
                    {
                        Utf8 msg = format("'%s = %s' is impossible because special function 'opAffect' cannot be found in '%s'", leftTypeInfo->getDisplayName().c_str(), rightTypeInfo->getDisplayName().c_str(), leftTypeInfo->getDisplayName().c_str());
                        return context->report({node, msg});
                    }

                    SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, nullptr, left, right, false));
                }
            }

            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, CASTFLAG_AUTO_BOOL | CASTFLAG_TRY_COERCE));
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        if (forTuple)
            return context->report({node, node->token, Msg0575});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", op, nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", op, nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, left, right, CASTFLAG_TRY_COERCE));
        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return notAllowed(context, node, leftTypeInfo);
        }

        // Shift operand too big
        if (right->flags & AST_VALUE_COMPUTED)
        {
            if (node->sourceFile->module->mustEmitSafetyOF(node))
            {
                if (right->computedValue.reg.u32 >= left->typeInfo->sizeOf * 8)
                {
                    if (tokenId == TokenId::SymLowerLowerEqual)
                    {
                        auto msg = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOp, left->typeInfo);
                        return context->report({right, msg});
                    }
                    else
                    {
                        auto msg = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOp, left->typeInfo);
                        return context->report({right, msg});
                    }
                }
            }
        }

        break;

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
        if (forTuple)
            return context->report({node, node->token, Msg0576});
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
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));
        if (leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
        if (forTuple)
            return context->report({node, node->token, Msg0577});
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
            SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report({left, Msg0578}));
            rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
            SWAG_VERIFY(rightTypeInfo->flags & TYPEINFO_INTEGER, context->report({right, format(Msg0579, rightTypeInfo->getDisplayName().c_str())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, left, right, CASTFLAG_TRY_COERCE));
            SWAG_CHECK(checkRelativePointerOverflow(context, leftTypeInfo, right));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymSlashEqual:
        if (forTuple)
            return context->report({node, node->token, Msg0580});
        else if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, "opIndexAssign", "/=", nullptr, left, arrayNode->structFlatParams, false));
            else
                SWAG_CHECK(resolveUserOp(context, "opAssign", "/=", nullptr, left, right, false));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;

    case TokenId::SymPercentEqual:
    case TokenId::SymAsteriskEqual:
        if (forTuple)
            return context->report({node, node->token, Msg0581});
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
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));
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