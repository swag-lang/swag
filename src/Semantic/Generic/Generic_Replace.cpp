#include "pch.h"
#include "Backend/BackendParameters.h"
#include "Semantic/DataSegment.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool Generic::replaceGenericParameters(OneMatch&                     match,
                                       VectorNative<TypeInfoParam*>& typeGenericParameters,
                                       VectorNative<AstNode*>&       nodeGenericParameters,
                                       AstNode*                      callGenericParameters,
                                       bool                          doType,
                                       bool                          doNode)
{
    for (uint32_t i = 0; i < typeGenericParameters.size(); i++)
    {
        const auto param = typeGenericParameters[i];

        if (doType)
        {
            const auto varDecl = castAst<AstVarDecl>(nodeGenericParameters[i], AstNodeKind::FuncDeclParam);

            // If the user has specified a generic type, take it
            if (callGenericParameters && i < callGenericParameters->childCount())
            {
                const auto genParam       = callGenericParameters->children[i];
                param->typeInfo           = genParam->typeInfo;
                varDecl->genTypeComesFrom = genParam;

                if (genParam->hasComputedValue())
                {
                    param->allocateComputedValue();
                    *param->value = *genParam->computedValue();
                    if (genParam->hasFlagComputedValue() && !genParam->isConstantGenTypeInfo())
                        param->flags.add(TYPEINFOPARAM_DEFINED_VALUE);
                }
            }

            // If we have a call type filled with the match, take it
            else if (match.genericParametersCallTypes[i].typeInfoReplace)
            {
                param->typeInfo           = match.genericParametersCallTypes[i].typeInfoReplace;
                varDecl->genTypeComesFrom = match.genericParametersCallTypes[i].fromNode;
            }

            // Take the type as it is in the instantiated struct/func
            else
            {
                SWAG_ASSERT(i < nodeGenericParameters.size());
                param->typeInfo           = nodeGenericParameters[i]->typeInfo;
                varDecl->genTypeComesFrom = nodeGenericParameters[i];
            }
        }

        SWAG_ASSERT(param->typeInfo);

        // @GenericConcreteAlias
        // In case of an alias, instantiate with the concrete type
        param->typeInfo = TypeManager::concreteType(param->typeInfo, CONCRETE_ALIAS);

        // Value
        auto it1 = match.genericReplaceValues.find(param->name);
        if (it1 != match.genericReplaceValues.end() && it1->second)
        {
            param->flags.add(TYPEINFOPARAM_DEFINED_VALUE);
            param->allocateComputedValue();
            *param->value = *it1->second;
        }

        if (doNode)
        {
            const auto nodeParam = nodeGenericParameters[i];
            nodeParam->setFlagsValueIsComputed();
            nodeParam->kind = AstNodeKind::ConstDecl;
            nodeParam->addAstFlag(AST_FROM_GENERIC);
            if (param->value)
            {
                nodeParam->allocateComputedValue();
                *nodeParam->computedValue() = *param->value;

                // :SliceLiteral
                if (param->typeInfo->isListArray() && nodeParam->typeInfo->isSlice())
                {
                    SwagSlice* slice;
                    const auto storageSegment                 = nodeParam->computedValue()->storageSegment;
                    nodeParam->computedValue()->storageOffset = storageSegment->reserve(sizeof(SwagSlice), reinterpret_cast<uint8_t**>(&slice));
                    const auto typeList                       = castTypeInfo<TypeInfoList>(param->typeInfo, TypeInfoKind::TypeListArray);
                    slice->buffer                             = storageSegment->address(param->value->storageOffset);
                    slice->count                              = typeList->subTypes.size();
                }
            }
        }
    }

    return true;
}

TypeInfo* Generic::replaceGenericTypes(VectorMap<Utf8, GenericReplaceType>& replaceTypes, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (replaceTypes.empty())
        return typeInfo;
    if (!typeInfo->isGeneric())
        return typeInfo;

    // Search if the type has a corresponding replacement
    const auto it = replaceTypes.find(typeInfo->name);
    if (it != replaceTypes.end())
    {
        auto typeReplace = it->second.typeInfoReplace;

        // Be sure to keep the nullable state of the generic type, if specified
        if (typeInfo->isNullable())
            typeReplace = g_TypeMgr->makeNullable(typeReplace);

        // We can have a match on a lambda for a function attribute, when function has been generated
        // In that case, we want to be sure that the kind is function
        if (typeInfo->isFuncAttr() && typeReplace->isLambdaClosure())
        {
            const auto t = typeReplace->clone();
            t->kind      = TypeInfoKind::FuncAttr;
            return t;
        }

        // Do not substitute with unconverted TypeList
        if (!typeReplace->isListArray())
            return typeReplace;
        if (typeInfo->isKindGeneric())
            return TypeManager::convertTypeListToArray(nullptr, castTypeInfo<TypeInfoList>(typeReplace), true);
    }

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
        case TypeInfoKind::TypedVariadic:
        {
            auto       typeVariadic = castTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
            const auto newType      = replaceGenericTypes(replaceTypes, typeVariadic->rawType);
            if (newType != typeVariadic->rawType)
            {
                typeVariadic          = castTypeInfo<TypeInfoVariadic>(typeVariadic->clone(), TypeInfoKind::TypedVariadic);
                typeVariadic->rawType = newType;
                typeVariadic->removeGenericFlag();
                typeVariadic->forceComputeName();
                return typeVariadic;
            }

            break;
        }

        case TypeInfoKind::Alias:
        {
            auto       typeAlias = castTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
            const auto newType   = replaceGenericTypes(replaceTypes, typeAlias->rawType);
            if (newType != typeAlias->rawType)
            {
                typeAlias          = castTypeInfo<TypeInfoAlias>(typeAlias->clone(), TypeInfoKind::Alias);
                typeAlias->rawType = newType;
                typeAlias->removeGenericFlag();
                typeAlias->forceComputeName();
                return typeAlias;
            }

            break;
        }

        case TypeInfoKind::Pointer:
        {
            auto       typePointer = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            const auto newType     = replaceGenericTypes(replaceTypes, typePointer->pointedType);
            if (newType != typePointer->pointedType)
            {
                auto newFlags = typePointer->flags;
                newFlags.remove(TYPEINFO_GENERIC);
                newFlags.add(TYPEINFO_FROM_GENERIC);
                typePointer = g_TypeMgr->makePointerTo(newType, newFlags);
                return typePointer;
            }

            break;
        }

        case TypeInfoKind::Array:
        {
            auto       typeArray      = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            const auto newPointedType = replaceGenericTypes(replaceTypes, typeArray->pointedType);
            TypeInfo*  newFinalType   = newPointedType;
            if (typeArray->pointedType != typeArray->finalType)
                newFinalType = replaceGenericTypes(replaceTypes, typeArray->finalType);
            if (newPointedType != typeArray->pointedType || newFinalType != typeArray->finalType)
            {
                typeArray              = castTypeInfo<TypeInfoArray>(typeArray->clone(), TypeInfoKind::Array);
                typeArray->pointedType = newPointedType;
                typeArray->finalType   = newFinalType;
                if (typeArray->count)
                    typeArray->removeGenericFlag();
                typeArray->sizeOf = typeArray->count * newPointedType->sizeOf;
                typeArray->forceComputeName();
                return typeArray;
            }

            break;
        }

        case TypeInfoKind::Slice:
        {
            auto       typeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
            const auto newType   = replaceGenericTypes(replaceTypes, typeSlice->pointedType);
            if (newType != typeSlice->pointedType)
            {
                typeSlice              = castTypeInfo<TypeInfoSlice>(typeSlice->clone(), TypeInfoKind::Slice);
                typeSlice->pointedType = newType;
                typeSlice->removeGenericFlag();
                typeSlice->forceComputeName();
                return typeSlice;
            }

            break;
        }

        case TypeInfoKind::LambdaClosure:
        case TypeInfoKind::FuncAttr:
        {
            TypeInfoFuncAttr* newLambda  = nullptr;
            const auto        typeLambda = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

            auto newType = replaceGenericTypes(replaceTypes, typeLambda->returnType);
            if (newType != typeLambda->returnType)
            {
                newLambda = castTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), typeLambda->kind);
                newLambda->removeGenericFlag();
                newLambda->returnType   = newType;
                newLambda->replaceTypes = replaceTypes;
            }

            const auto numParams = typeLambda->parameters.size();
            for (uint32_t idx = 0; idx < numParams; idx++)
            {
                const auto param = typeLambda->parameters[idx];
                newType          = replaceGenericTypes(replaceTypes, param->typeInfo);

                // If generic parameter is a closure, but the instantiated type is a lambda, then a conversion will
                // take place.
                // But we want to be sure that the closure stays a closure in order for the conversion to take place.
                // So transform the replaced lambda type by a closure one.
                if (param->typeInfo->isClosure() && newType->isLambda())
                {
                    newType         = newType->clone();
                    newType->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
                    newType->addFlag(TYPEINFO_CLOSURE);

                    auto newParam         = TypeManager::makeParam();
                    newParam->typeInfo    = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
                    const auto newTypeFct = castTypeInfo<TypeInfoFuncAttr>(newType, newType->kind);
                    newTypeFct->parameters.push_front(newParam);
                }

                if (newType != param->typeInfo)
                {
                    if (!newLambda)
                    {
                        newLambda = castTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                        newLambda->removeGenericFlag();
                        newLambda->replaceTypes = replaceTypes;
                    }

                    const auto newParam = newLambda->parameters[idx];
                    newParam->typeInfo  = newType;
                    newParam->flags.add(TYPEINFOPARAM_FROM_GENERIC);
                }
            }

            if (newLambda)
            {
                newLambda->forceComputeName();
                return newLambda;
            }

            break;
        }
    }

    return typeInfo;
}
