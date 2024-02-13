#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Module.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool TypeManager::makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint64_t castFlags)
{
    SWAG_CHECK(makeCompatibles(context, leftNode->typeInfo, leftNode, rightNode, castFlags));
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    if (!(castFlags & CASTFLAG_NO_TUPLE_TO_STRUCT))
    {
        SWAG_ASSERT(fromNode->typeInfo);
        const auto fromType = concreteType(fromNode->typeInfo, CONCRETE_ALIAS);
        if (fromType->isListTuple())
        {
            bool convert = false;
            if (toType->isStruct())
                convert = true;

            if (toType->isPointerRef() && toType->isConst())
            {
                const auto ptrRef = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
                if (ptrRef->pointedType->isStruct())
                {
                    toType  = ptrRef->pointedType;
                    convert = true;
                }
            }

            if (fromNode->parent->kind == AstNodeKind::FuncDeclParam)
                convert = false;

            if (convert)
            {
                SWAG_CHECK(Ast::convertLiteralTupleToStructVar(context, toType, fromNode));
                return true;
            }
        }
    }

    SWAG_CHECK(makeCompatibles(context, toType, fromNode->typeInfo, toNode, fromNode, castFlags));
    if (!fromNode)
        return true;

    // Be sure to keep the TYPEINFO_SPREAD flag from the original type
    if (fromNode->castedTypeInfo && fromNode->castedTypeInfo->hasFlag(TYPEINFO_SPREAD))
    {
        fromNode->typeInfo = fromNode->typeInfo->clone();
        fromNode->typeInfo->addFlag(TYPEINFO_SPREAD);
    }

    // auto cast
    if (fromNode->typeInfo->hasFlag(TYPEINFO_AUTO_CAST) && !fromNode->castedTypeInfo)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
        }
    }

    // Store constant expression in the constant segment
    if (!(castFlags & CASTFLAG_NO_COLLECT))
    {
        if (fromNode->typeInfo->isListTuple() || fromNode->typeInfo->isListArray())
        {
            while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
                fromNode = fromNode->childs.empty() ? nullptr : fromNode->childs.front();

            if (fromNode && (fromNode->hasAstFlag(AST_CONST_EXPR)))
            {
#ifdef SWAG_HAS_ASSERT
                const TypeInfoList* typeList = castTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
                SWAG_ASSERT(typeList->subTypes.size() == fromNode->childs.size());
#endif
                const auto exprList = castAst<AstExpressionList>(fromNode, AstNodeKind::ExpressionList);
                if (exprList && !exprList->hasSemFlag(SEMFLAG_EXPR_LIST_CST))
                {
                    exprList->addSemFlag(SEMFLAG_EXPR_LIST_CST);

                    // Test sizeof because {} is legit to initialize a struct (for default values in function arguments)
                    if (fromNode->typeInfo->sizeOf)
                    {
                        const auto constSegment = Semantic::getConstantSegFromContext(exprList);
                        exprList->allocateComputedValue();
                        exprList->computedValue->storageSegment = constSegment;
                        SWAG_CHECK(Semantic::reserveAndStoreToSegment(context, exprList->computedValue->storageSegment, exprList->computedValue->storageOffset, nullptr, fromNode->typeInfo, exprList));
                    }
                }
            }
        }
    }

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint64_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    const auto realFromType = concreteType(fromType, CONCRETE_ALIAS);
    const auto realToType   = concreteType(toType, CONCRETE_ALIAS);
    SWAG_ASSERT(realFromType && realToType);
    if (realFromType->isListTuple())
    {
        if (realToType->isStruct())
            return true;

        if (realToType->isConstPointerRef())
        {
            const auto ptrRef = castTypeInfo<TypeInfoPointer>(realToType, TypeInfoKind::Pointer);
            if (ptrRef->pointedType->isStruct())
                return true;
        }
    }

    SWAG_ASSERT(toType && fromType);

    if (fromType->hasFlag(TYPEINFO_AUTO_CAST))
        castFlags |= CASTFLAG_EXPLICIT;
    if (toType->isGeneric())
        castFlags |= CASTFLAG_NO_IMPLICIT;
    if (toType->isFromGeneric())
        castFlags |= CASTFLAG_NO_IMPLICIT;

    if (toType->isFuncAttr())
        toType = concreteType(toType, CONCRETE_FUNC);
    if (fromType->isFuncAttr())
        fromType = concreteType(fromType, CONCRETE_FUNC);
    if (!toType->isLambdaClosure() && fromType->isLambdaClosure())
        fromType = concreteType(fromType, CONCRETE_FUNC);

    // Transform typealias to related type
    if (toType->isAlias())
        toType = concreteType(toType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCE_ALIAS : CONCRETE_ALIAS);
    if (fromType->isAlias())
        fromType = concreteType(fromType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCE_ALIAS : CONCRETE_ALIAS);

    // Transform enum to underlying type
    if ((castFlags & CASTFLAG_CONCRETE_ENUM) || (castFlags & CASTFLAG_EXPLICIT) || toType->hasFlag(TYPEINFO_INCOMPLETE) || fromType->hasFlag(TYPEINFO_INCOMPLETE))
    {
        toType   = concreteType(toType, CONCRETE_ENUM);
        fromType = concreteType(fromType, CONCRETE_ENUM);
    }

    if ((castFlags & CASTFLAG_INDEX) && fromType->hasFlag(TYPEINFO_ENUM_INDEX))
    {
        fromType = concreteType(fromType, CONCRETE_ENUM);
    }

    // Transform typealias to related type
    fromType = concreteType(fromType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCE_ALIAS : CONCRETE_ALIAS);
    toType   = concreteType(toType, (castFlags & CASTFLAG_EXPLICIT) ? CONCRETE_FORCE_ALIAS : CONCRETE_ALIAS);

    if (fromType->isListTuple() || fromType->isListArray())
    {
        toType = toType->getConcreteAlias();
    }

    if (toNode && toNode->resolvedSymbolName && toNode->resolvedSymbolName->kind == SymbolKind::EnumValue)
    {
        toType   = toType->getConcreteAlias();
        fromType = fromType->getConcreteAlias();
    }

    if (fromNode && fromNode->resolvedSymbolName)
    {
        if (fromNode->resolvedSymbolName->kind == SymbolKind::EnumValue ||
            fromNode->resolvedSymbolName->kind == SymbolKind::Function)
        {
            toType   = toType->getConcreteAlias();
            fromType = fromType->getConcreteAlias();
        }
    }

    if (fromType == toType)
        return true;
    if (fromType->isUndefined() || toType->isUndefined())
        return true;

    // Everything can be casted to or from type 'any'
    if (toType->isAny() || fromType->isAny())
        return castToFromAny(context, toType, fromType, toNode, fromNode, castFlags);

    // Variadic
    if (fromType->isTypedVariadic())
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->isTypedVariadic())
        toType = ((TypeInfoVariadic*) toType)->rawType;

    bool result = false;

    // To/From a moveref
    if (!fromType->hasFlag(TYPEINFO_POINTER_ACCEPT_MOVE_REF) && toType->hasFlag(TYPEINFO_POINTER_MOVE_REF))
    {
        if ((castFlags & CASTFLAG_PARAMS) && !(castFlags & CASTFLAG_ACCEPT_MOVE_REF))
            return castError(context, toType, fromType, fromNode, castFlags);
    }
    else if (fromType->hasFlag(TYPEINFO_POINTER_ACCEPT_MOVE_REF) && !toType->hasFlag(TYPEINFO_POINTER_MOVE_REF))
    {
        return castError(context, toType, fromType, fromNode, castFlags);
    }

    // From a reference
    if (fromType->isPointerRef() ||
        (fromNode && fromNode->kind == AstNodeKind::KeepRef && fromType->isPointer()))
    {
        const auto fromTypeRef = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer)->pointedType;
        if (fromTypeRef == toType)
        {
            castFlags |= CASTFLAG_FORCE_UN_CONST;
            result = true;
        }
        else if (fromTypeRef->isSame(toType, castFlags | CASTFLAG_CAST))
        {
            castFlags |= CASTFLAG_FORCE_UN_CONST;
            result = true;
        }
    }

    if (toType->isFromGeneric())
        castFlags |= CASTFLAG_EXACT_TUPLE_STRUCT;
    if (fromType->isFromGeneric())
        castFlags |= CASTFLAG_EXACT_TUPLE_STRUCT;

    if (toNode && toNode->hasSemFlag(SEMFLAG_FROM_REF) && toType->isPointerRef())
        toType = concretePtrRef(toType);
    if (fromNode && fromNode->hasSemFlag(SEMFLAG_FROM_REF) && fromType->isPointerRef())
        fromType = concretePtrRef(fromType);

    // If not already ok, call 'same'
    if (!result && fromType->isSame(toType, castFlags | CASTFLAG_CAST))
        result = true;

    // Always match against a generic
    if (!result)
    {
        if (toType->isKindGeneric())
            result = true;
    }

    if (!result)
    {
        if (!toType->isPointer() && fromType->isPointerRef())
        {
            context->castFlagsResult |= CASTFLAG_RESULT_FROM_REF;
            fromType = concretePtrRef(fromType);
        }

        switch (toType->kind)
        {
        // Cast to pointer
        case TypeInfoKind::Pointer:
            if (toType->isPointerRef())
                SWAG_CHECK(castToPointerRef(context, toType, fromType, fromNode, castFlags));
            else
                SWAG_CHECK(castToPointer(context, toType, fromType, fromNode, castFlags));
            break;

        // Cast to native type
        case TypeInfoKind::Native:
            SWAG_CHECK(castToNative(context, toType, fromType, toNode, fromNode, castFlags));
            break;

        // Cast to enum
        case TypeInfoKind::Enum:
            SWAG_CHECK(castToEnum(context, toType, fromType, toNode, fromNode, castFlags));
            break;

        // Cast to array
        case TypeInfoKind::Array:
            SWAG_CHECK(castToArray(context, toType, fromType, fromNode, castFlags));
            break;

        // Cast to slice
        case TypeInfoKind::Slice:
            SWAG_CHECK(castToSlice(context, toType, fromType, fromNode, castFlags));
            break;

        // Cast to interface
        case TypeInfoKind::Interface:
            SWAG_CHECK(castToInterface(context, toType, fromType, fromNode, castFlags));
            break;

        // Cast to lambda
        case TypeInfoKind::LambdaClosure:
            if (toType->isClosure())
                SWAG_CHECK(castToClosure(context, toType, fromType, fromNode, castFlags));
            else
                SWAG_CHECK(castToLambda(context, toType, fromType, fromNode, castFlags));
            break;

        default:
            return castError(context, toType, fromType, fromNode, castFlags);
        }
    }

    if (context->result != ContextResult::Done)
    {
        SWAG_ASSERT(castFlags & CASTFLAG_ACCEPT_PENDING);
        return true;
    }

    // Const mismatch
    if (!toType->isKindGeneric() && !toType->isLambdaClosure() && !(castFlags & CASTFLAG_FORCE_UN_CONST))
    {
        if (!(castFlags & CASTFLAG_PARAMS) || !toType->isStruct())
        {
            if (!toType->isPointerNull() &&
                !toType->isString() &&
                !toType->isInterface() &&
                (!toType->isBool() || !(castFlags & CASTFLAG_AUTO_BOOL)) &&
                (!toType->isNative(NativeTypeKind::U64) || !fromType->isPointer()))
            {
                const bool toConst   = toType->isConst();
                const bool fromConst = fromType->isConst();
                bool       diff      = false;
                if (castFlags & CASTFLAG_FOR_GENERIC)
                    diff = toConst != fromConst;
                else
                    diff = !toConst && fromConst;
                if (diff)
                {
                    if (!(castFlags & CASTFLAG_UN_CONST))
                        return castError(context, toType, fromType, fromNode, castFlags, CastErrorType::Const);

                    // We can affect a const to an un-const if type is by copy, and we are in an affectation
                    if (!fromType->isStruct() &&
                        !toType->isStruct() &&
                        !fromType->isArray() &&
                        !toType->isArray())
                    {
                        return castError(context, toType, fromType, fromNode, castFlags, CastErrorType::Const);
                    }
                }
            }
        }
    }

    return true;
}
