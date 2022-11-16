#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"

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
        SWAG_VERIFY(!right->typeInfo->isConst(), context->report(right, Fmt(Err(Err0559), right->typeInfo->getDisplayNameC())));
    }

    return true;
}

bool SemanticJob::resolveAfterAffectLeft(SemanticContext* context)
{
    // :DeduceLambdaType
    auto node     = context->node;
    auto typeInfo = TypeManager::concreteReferenceType(node->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Lambda || typeInfo->kind == TypeInfoKind::Struct)
    {
        auto op = CastAst<AstOp>(node->parent, AstNodeKind::AffectOp);
        if (op->dependentLambda)
        {
            // Cannot cast from closure to lambda
            if (node->typeInfo->isLambda() && op->dependentLambda->typeInfo->isClosure())
            {
                PushErrContext ec(context, node->parent, JobContext::ErrorContextType::Node);
                Diagnostic     diag{op->childs.back(), Err(Err0185)};
                return context->report(diag);
            }

            ScopedLock lk(op->dependentLambda->mutex);
            op->dependentLambda->flags &= ~AST_SPEC_SEMANTIC2;
            launchResolveSubDecl(context, op->dependentLambda);
        }
    }

    return true;
}

bool SemanticJob::resolveAffect(SemanticContext* context)
{
    auto node    = CastAst<AstOp>(context->node, AstNodeKind::AffectOp);
    auto left    = node->childs[0];
    auto right   = node->childs[1];
    auto tokenId = node->token.id;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(left->resolvedSymbolName && left->resolvedSymbolOverload, context->report(left, Err(Err0566)));
    //SWAG_VERIFY(left->resolvedSymbolName->kind == SymbolKind::Variable, context->report(left, Err(Err0567)));

    // Check that left type is mutable
    // If not, try to find the culprit type
    if ((left->flags & AST_IS_CONST) || !(left->flags & AST_L_VALUE) || (left->typeInfo->isPointerRef() && left->typeInfo->isConst()))
    {
        Utf8 hint;
        if (left->typeInfo->isConst())
        {
            if (left->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM)
            {
                Diagnostic note{Hlp(Hlp0016), DiagnosticLevel::Help};
                Diagnostic diag{left, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str())};
                return context->report(diag, &note);
            }

            hint = Hint::isType(left->typeInfo);
        }
        else if (left->kind == AstNodeKind::IdentifierRef)
        {
            for (int i = left->childs.count - 1; i >= 0; i--)
            {
                auto typeChild = TypeManager::concreteType(left->childs[i]->typeInfo, CONCRETE_ALIAS);
                if (typeChild && typeChild->isConst())
                {
                    left = left->childs[i];
                    hint = Hint::isType(left->typeInfo);
                    break;
                }

                if (left->childs[i]->kind == AstNodeKind::ArrayPointerIndex)
                {
                    auto arr = CastAst<AstArrayPointerIndex>(left->childs[i], AstNodeKind::ArrayPointerIndex);
                    if (arr->array->typeInfo->isNative(NativeTypeKind::String))
                    {
                        left = arr->array;
                        hint = Hint::isType(left->typeInfo);
                        break;
                    }
                }

                if (left->childs[i]->kind == AstNodeKind::FuncCall && left->childs[i]->typeInfo->kind == TypeInfoKind::Struct)
                {
                    left = left->childs[i];
                    hint = Fmt(Hnt(Hnt0039), left->typeInfo->getDisplayNameC());
                    break;
                }
            }
        }

        if (left->flags & AST_L_VALUE)
            return context->report(hint, {left, Err(Err0564)});
    }

    if (!(left->flags & AST_L_VALUE))
    {
        if (left->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
            return context->report(Hnt(Hnt0018), {left, Err(Err0564)});
        return context->report(left, Err(Err0565));
    }

    // Dereference
    if (left->typeInfo->isPointerRef())
        left->childs.back()->semFlags |= AST_SEM_FROM_REF;
    if (right->typeInfo->isPointerRef())
        right->childs.back()->semFlags |= AST_SEM_FROM_REF;

    // Special case for enum : nothing is possible, except for flags
    bool forEnumFlags  = false;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo, CONCRETE_ALIAS);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo, CONCRETE_ALIAS);
    if (node->token.id != TokenId::SymEqual)
    {
        if (leftTypeInfo->kind == TypeInfoKind::Enum || rightTypeInfo->kind == TypeInfoKind::Enum)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

            if (node->token.id != TokenId::SymVerticalEqual &&
                node->token.id != TokenId::SymAmpersandEqual &&
                node->token.id != TokenId::SymCircumflexEqual)
                return notAllowed(context, node, leftTypeInfo);
            if (!(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
                return notAllowed(context, node, leftTypeInfo, "because the enum is not marked with `Swag.EnumFlags`");
            forEnumFlags = true;
        }
    }

    rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    SWAG_VERIFY(!rightTypeInfo->isNative(NativeTypeKind::Void), context->report(right, Err(Err0569)));

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isNative(NativeTypeKind::Any) && node->token.id != TokenId::SymEqual)
    {
        return context->report(node, Fmt(Err(Err0570), node->token.ctext()));
    }

    // Is this an array like affectation ?
    AstArrayPointerIndex* arrayNode = nullptr;
    if (left->kind == AstNodeKind::IdentifierRef && left->childs.back()->kind == AstNodeKind::ArrayPointerIndex)
    {
        arrayNode      = CastAst<AstArrayPointerIndex>(left->childs.back(), AstNodeKind::ArrayPointerIndex);
        auto arrayType = TypeManager::concretePtrRefType(arrayNode->array->typeInfo);
        if (arrayType->kind != TypeInfoKind::Struct)
            arrayNode = nullptr;

        // Add self and value in list of parameters
        if (arrayNode)
        {
            if (!(node->doneFlags & AST_DONE_FLAT_PARAMS))
            {
                auto leftNode = arrayNode;
                while (leftNode->array->kind == AstNodeKind::ArrayPointerIndex)
                    leftNode = CastAst<AstArrayPointerIndex>(leftNode->array, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.push_back(right);
                arrayNode->structFlatParams.push_front(leftNode->array);
                node->doneFlags |= AST_DONE_FLAT_PARAMS;
            }
        }
    }

    node->typeInfo = g_TypeMgr->typeInfoBool;

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
            context->job->waitAllStructInterfaces(rightTypeInfo);
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
            leftTypeInfo->kind != TypeInfoKind::Array &&
            leftTypeInfo->kind != TypeInfoKind::Alias &&
            leftTypeInfo->kind != TypeInfoKind::Enum)
            return context->report(left, Fmt(Err(Err0571), TypeInfo::getNakedKindName(leftTypeInfo), leftTypeInfo->getDisplayNameC()));
        if (rightTypeInfo->kind != TypeInfoKind::Native &&
            rightTypeInfo->kind != TypeInfoKind::Pointer &&
            rightTypeInfo->kind != TypeInfoKind::Reference &&
            rightTypeInfo->kind != TypeInfoKind::Slice &&
            rightTypeInfo->kind != TypeInfoKind::Lambda &&
            rightTypeInfo->kind != TypeInfoKind::TypeListTuple &&
            rightTypeInfo->kind != TypeInfoKind::TypeListArray &&
            rightTypeInfo->kind != TypeInfoKind::Struct &&
            rightTypeInfo->kind != TypeInfoKind::Interface &&
            rightTypeInfo->kind != TypeInfoKind::Array &&
            rightTypeInfo->kind != TypeInfoKind::Alias)
            return context->report(right, Fmt(Err(Err0572), rightTypeInfo->getDisplayNameC(), TypeInfo::getArticleKindName(rightTypeInfo)));

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
                    return context->report(node, Err(Err0573));
                }
                else
                {
                    SymbolName* symbol = nullptr;
                    SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndexAffect, left, &symbol));
                    if (!symbol)
                    {
                        if (context->result != ContextResult::Done)
                            return true;

                        Utf8 msg = Fmt(Err(Err0225), leftTypeInfo->getDisplayNameC(), rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC());
                        return context->report(node, msg);
                    }

                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAffect, nullptr, nullptr, left, arrayNode->structFlatParams));
                }
            }
            else
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, ISSAME_CAST | ISSAME_FOR_AFFECT))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else if (rightTypeInfo->isInitializerList())
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST | CASTFLAG_ACCEPT_PENDING));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else
                {
                    if (leftTypeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
                        return context->report(node, Err(Err0574));
                    SWAG_CHECK(resolveUserOpAffect(context, leftTypeInfo, rightTypeInfo, left, right));
                    if (context->result != ContextResult::Done)
                        return true;
                }
            }

            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, CASTFLAG_AUTO_BOOL | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT | CASTFLAG_ACCEPT_PENDING));
        if (context->result == ContextResult::Pending)
            return true;
        break;

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
        if (forTuple)
            return context->report(node, Err(Err0573));
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, left, right, CASTFLAG_TRY_COERCE));
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
                if (right->computedValue->reg.u32 >= left->typeInfo->sizeOf * 8)
                {
                    if (tokenId == TokenId::SymLowerLowerEqual)
                    {
                        auto msg = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOp, left->typeInfo);
                        return context->report(right, msg);
                    }
                    else
                    {
                        auto msg = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOp, left->typeInfo);
                        return context->report(right, msg);
                    }
                }
            }
        }

        break;

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
        if (forTuple)
            return context->report(node, Err(Err0573));
        else if (forStruct)
        {
            const char* op = "&=";
            if (tokenId == TokenId::SymVerticalEqual)
                op = "|=";
            else if (tokenId == TokenId::SymCircumflexEqual)
                op = "^=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
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
            return context->report(node, Err(Err0573));
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPlusEqual ? "+=" : "-=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        // :PointerArithmetic
        if (leftTypeInfo->kind == TypeInfoKind::Pointer)
        {
            SWAG_VERIFY(leftTypeInfo->flags & TYPEINFO_POINTER_ARITHMETIC, context->report(Hint::isType(leftTypeInfo), {node, Err(Err0192)}));
            SWAG_VERIFY((leftTypeInfo->isPointerToTypeInfo()) == 0, context->report(left, Err(Err0144)));
            rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);
            SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report(right, Fmt(Err(Err0579), rightTypeInfo->getDisplayNameC())));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, left, right, CASTFLAG_TRY_COERCE));
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
            return context->report(node, Err(Err0573));
        else if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, "/=", nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, "/=", nullptr, left, right));
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
            return context->report(node, Err(Err0573));
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPercentEqual ? "%=" : "*=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
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
        return context->internalError("resolveAffect, invalid token");
    }

    node->byteCodeFct = ByteCodeGenJob::emitAffect;
    return true;
}