#include "pch.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool TypeManager::tryOpAffect(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    auto structType = toType;

    if (castFlags.has(CAST_FLAG_UFCS) && structType->isPointerTo(TypeInfoKind::Struct))
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType         = typePtr->pointedType;
    }

    bool isMoveRef = false;

    // Cast to a const reference, must take the pointed struct
    if (structType->isConstPointerRef() && castFlags.has(CAST_FLAG_PARAMS))
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType         = typePtr->pointedType;
        toType             = structType;
    }

    // We can also match a moveref with an opAffect
    else if (structType->isPointerMoveRef() && castFlags.has(CAST_FLAG_PARAMS))
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType         = typePtr->pointedType;
        toType             = structType;
        isMoveRef          = true;
    }

    if (!structType->isStruct() || !castFlags.has(CAST_FLAG_EXPLICIT | CAST_FLAG_AUTO_OP_CAST))
        return false;
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
    if (!typeStruct->declNode)
        return false;

    bool isSuffix = false;
    if ((fromNode && fromNode->hasSemFlag(SEMFLAG_LITERAL_SUFFIX)) || castFlags.has(CAST_FLAG_LITERAL_SUFFIX))
        isSuffix = true;

    auto        structNode = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol     = isSuffix ? structNode->scope->symbolOpAffectSuffix : structNode->scope->symbolOpAffect;

    // Instantiated opAffect, in a generic struct, will be in the scope of the original struct, not the instantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = castAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        symbol     = isSuffix ? structNode->scope->symbolOpAffectSuffix : structNode->scope->symbolOpAffect;
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

            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->addAstFlag(AST_OP_AFFECT_CAST);
                fromNode->typeInfoCast = fromType;
                fromNode->typeInfo     = toType;
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->addExtraPointer(ExtraPointerKind::UserOp, it->second);
            }

            context->castFlagsResult.add(CAST_RESULT_AUTO_OP_AFFECT);
            if (isMoveRef)
                context->castFlagsResult.add(CAST_RESULT_AUTO_MOVE_OP_AFFECT);
            return true;
        }
    }

    VectorNative<SymbolOverload*> toAffect;
    VectorNative<SymbolOverload*> overloads;

    // Wait for all opAffect to be solved
    Semantic::waitForOverloads(context->baseJob, symbol);
    YIELD();

    {
        SharedLock ls(symbol->mutex);
        overloads = symbol->overloads;
    }

    // Resolve opAffect that match
    for (auto over : overloads)
    {
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->declNode->hasAttribute(ATTRIBUTE_IMPLICIT) && !castFlags.has(CAST_FLAG_EXPLICIT))
            continue;
        if (typeFunc->parameters.size() <= 1)
            continue;
        if (makeCompatibles(context, typeFunc->parameters[1]->typeInfo, fromType, nullptr, nullptr, CAST_FLAG_NO_LAST_MINUTE | CAST_FLAG_TRY_COERCE | CAST_FLAG_JUST_CHECK))
            toAffect.push_back(over);
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        typeStruct->mapOpAffect[idxMap][fromType] = toAffect.empty() ? nullptr : toAffect[0];
    }

    if (toAffect.empty())
        return false;

    // @opAffectParam
    if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        fromNode->addAstFlag(AST_OP_AFFECT_CAST);
        fromNode->typeInfoCast = fromType;
        fromNode->typeInfo     = toType;
        fromNode->addExtraPointer(ExtraPointerKind::UserOp, toAffect[0]);
    }

    context->castFlagsResult.add(CAST_RESULT_AUTO_OP_AFFECT);
    if (isMoveRef)
        context->castFlagsResult.add(CAST_RESULT_AUTO_MOVE_OP_AFFECT);
    return true;
}

bool TypeManager::tryOpCast(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    toType   = concretePtrRef(toType);
    fromType = concretePtrRef(fromType);

    auto structType = fromType;
    if (castFlags.has(CAST_FLAG_UFCS) && structType->isPointerTo(TypeInfoKind::Struct))
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(structType, TypeInfoKind::Pointer);
        structType         = typePtr->pointedType;
    }

    if (!structType->isStruct() || !castFlags.has(CAST_FLAG_EXPLICIT | CAST_FLAG_AUTO_OP_CAST))
        return false;
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(structType, TypeInfoKind::Struct);
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

            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromType;
                fromNode->typeInfo     = toType;
                fromNode->addExtraPointer(ExtraPointerKind::UserOp, it->second);
                fromNode->addSemFlag(SEMFLAG_USER_CAST);
            }

            context->castFlagsResult.add(CAST_RESULT_AUTO_OP_CAST);
            return true;
        }
    }

    auto        structNode = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SymbolName* symbol     = structNode->scope->symbolOpCast;

    // Instantiated opCast, in a generic struct, will be in the scope of the original struct, not the instantiated one
    if (!symbol && typeStruct->fromGeneric)
    {
        structNode = castAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
        symbol     = structNode->scope->symbolOpCast;
    }

    if (!symbol)
        return false;

    // Wait for all opCast to be solved
    Semantic::waitForOverloads(context->baseJob, symbol);
    YIELD();

    VectorNative<SymbolOverload*> toCast;
    VectorNative<SymbolOverload*> overloads;

    {
        SharedLock ls(symbol->mutex);
        overloads = symbol->overloads;
    }

    // Resolve opCast that match
    for (auto over : overloads)
    {
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
        if (typeFunc->isGeneric() || typeFunc->returnType->isGeneric())
            continue;
        if (!typeFunc->declNode->hasAttribute(ATTRIBUTE_IMPLICIT) && !castFlags.has(CAST_FLAG_EXPLICIT))
            continue;
        if (typeFunc->returnType->isSame(toType, CAST_FLAG_EXACT))
            toCast.push_back(over);
    }

    // Add in the cache of possible matches
    {
        ScopedLock lkOp(typeStruct->mutexCache);
        typeStruct->mapOpCast[toType] = toCast.empty() ? nullptr : toCast[0];
    }

    if (toCast.empty())
        return false;

    if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        fromNode->typeInfoCast = fromType;
        fromNode->typeInfo     = toType;
        fromNode->addExtraPointer(ExtraPointerKind::UserOp, toCast[0]);
        fromNode->addSemFlag(SEMFLAG_USER_CAST);
    }

    context->castFlagsResult.add(CAST_RESULT_AUTO_OP_CAST);
    return true;
}

bool TypeManager::tryCode(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    toType   = concretePtrRef(toType);
    fromType = concretePtrRef(fromType);
    if (!toType->isCode())
        return false;

    if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        const auto typeCode = makeType<TypeInfoCode>();
        typeCode->content   = fromNode;
        typeCode->content->addAstFlag(AST_NO_SEMANTIC);
        fromNode->typeInfo = typeCode;
        fromNode->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
    }

    return true;
}
