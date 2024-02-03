#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

bool TypeManager::tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    auto structType = toType;

    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        const auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    bool isMoveRef = false;

    // Cast to a const reference, must take the pointed struct
    if (structType->isConstPointerRef() && castFlags & CASTFLAG_PARAMS)
    {
        const auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
        toType       = structType;
    }

    // We can also match a moveref with an opAffect
    else if (structType->isPointerMoveRef() && castFlags & CASTFLAG_PARAMS)
    {
        const auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
        toType       = structType;
        isMoveRef    = true;
    }

    if (!structType->isStruct() || !(castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
        return false;
    const auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
    if (!typeStruct->declNode)
        return false;

    auto        structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol;

    bool isSuffix = false;
    if ((fromNode && fromNode->semFlags & SEMFLAG_LITERAL_SUFFIX) || castFlags & CASTFLAG_LITERAL_SUFFIX)
        isSuffix = true;

    if (isSuffix)
        symbol = structNode->scope->symbolOpAffectSuffix;
    else
        symbol = structNode->scope->symbolOpAffect;

    // Instantiated opAffect, in a generic struct, will be in the scope of the original struct, not the intantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        if (isSuffix)
            symbol = structNode->scope->symbolOpAffectSuffix;
        else
            symbol = structNode->scope->symbolOpAffect;
    }

    if (!symbol)
        return false;

    int idxMap = isSuffix ? 0 : 2;
    if (isMoveRef)
        idxMap++;

    // In the cache of possible matches
    {
        SharedLock lkOp(typeStruct->mutexCache);

        const auto it = typeStruct->mapOpAffect[idxMap].find(fromType);
        if (it != typeStruct->mapOpAffect[idxMap].end())
        {
            if (!it->second)
                return false;

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->flags |= AST_OPAFFECT_CAST;
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->resolvedUserOpSymbolOverload = it->second;
            }

            context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPAFFECT;
            if (isMoveRef)
                context->castFlagsResult |= CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT;
            return true;
        }
    }

    VectorNative<SymbolOverload*> toAffect;

    // Wait for all opAffect to be solved
    {
        ScopedLock ls(symbol->mutex);
        if (symbol->cptOverloads)
        {
            SWAG_ASSERT(context && context->baseJob);
            SWAG_ASSERT(context->result == ContextResult::Done);
            Semantic::waitSymbolNoLock(context->baseJob, symbol);
            return true;
        }
    }

    // Resolve opAffect that match
    {
        SharedLock ls(symbol->mutex);
        for (auto over : symbol->overloads)
        {
            const auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->parameters.size() <= 1)
                continue;
            if (makeCompatibles(context, typeFunc->parameters[1]->typeInfo, fromType, nullptr, nullptr, CASTFLAG_NO_LAST_MINUTE | CASTFLAG_TRY_COERCE | CASTFLAG_JUST_CHECK))
                toAffect.push_back(over);
        }
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        if (isSuffix)
            typeStruct->mapOpAffect[idxMap][fromType] = toAffect.empty() ? nullptr : toAffect[0];
        else
            typeStruct->mapOpAffect[idxMap][fromType] = toAffect.empty() ? nullptr : toAffect[0];
    }

    if (toAffect.empty())
        return false;

    // :opAffectParam
    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
    {
        fromNode->flags |= AST_OPAFFECT_CAST;
        fromNode->castedTypeInfo = fromType;
        fromNode->typeInfo       = toType;
        fromNode->allocateExtension(ExtensionKind::Misc);
        fromNode->extMisc()->resolvedUserOpSymbolOverload = toAffect[0];
    }

    context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPAFFECT;
    if (isMoveRef)
        context->castFlagsResult |= CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT;
    return true;
}

bool TypeManager::tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint64_t castFlags)
{
    toType   = TypeManager::concretePtrRef(toType);
    fromType = TypeManager::concretePtrRef(fromType);

    auto structType = fromType;
    if (castFlags & CASTFLAG_UFCS && structType->isPointerTo(TypeInfoKind::Struct))
    {
        const auto typePtr = CastTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType   = typePtr->pointedType;
    }

    if (!structType->isStruct() || !(castFlags & (CASTFLAG_EXPLICIT | CASTFLAG_AUTO_OPCAST)))
        return false;
    const auto typeStruct = CastTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
    if (!typeStruct->declNode)
        return false;

    // In the cache of possible matches
    {
        SharedLock lkOp(typeStruct->mutexCache);
        const auto it = typeStruct->mapOpCast.find(toType);
        if (it != typeStruct->mapOpCast.end())
        {
            if (!it->second)
                return false;

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromType;
                fromNode->typeInfo       = toType;
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->resolvedUserOpSymbolOverload = it->second;
                fromNode->semFlags |= SEMFLAG_USER_CAST;
            }

            context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPCAST;
            return true;
        }
    }

    auto        structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol     = structNode->scope->symbolOpCast;

    // Instantiated opCast, in a generic struct, will be in the scope of the original struct, not the intantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        symbol     = structNode->scope->symbolOpCast;
    }

    if (!symbol)
        return false;

    VectorNative<SymbolOverload*> toCast;

    // Wait for all opCast to be solved
    {
        ScopedLock ls(symbol->mutex);
        if (symbol->cptOverloads)
        {
            SWAG_ASSERT(context && context->baseJob);
            SWAG_ASSERT(context->result == ContextResult::Done);
            Semantic::waitSymbolNoLock(context->baseJob, symbol);
            return true;
        }
    }

    // Resolve opCast that match
    {
        SharedLock ls(symbol->mutex);
        for (auto over : symbol->overloads)
        {
            const auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc->isGeneric() || typeFunc->returnType->isGeneric())
                continue;
            if (!(typeFunc->declNode->attributeFlags & ATTRIBUTE_IMPLICIT) && !(castFlags & CASTFLAG_EXPLICIT))
                continue;
            if (typeFunc->returnType->isSame(toType, CASTFLAG_EXACT))
                toCast.push_back(over);
        }
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        typeStruct->mapOpCast[toType] = toCast.empty() ? nullptr : toCast[0];
    }

    if (toCast.empty())
        return false;

    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
    {
        fromNode->castedTypeInfo = fromType;
        fromNode->typeInfo       = toType;
        fromNode->allocateExtension(ExtensionKind::Misc);
        fromNode->extMisc()->resolvedUserOpSymbolOverload = toCast[0];
        fromNode->semFlags |= SEMFLAG_USER_CAST;
    }

    context->castFlagsResult |= CASTFLAG_RESULT_AUTO_OPCAST;
    return true;
}
