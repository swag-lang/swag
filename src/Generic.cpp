#include "pch.h"
#include "Generic.h"
#include "Ast.h"
#include "DataSegment.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "SemanticJob.h"
#include "TypeManager.h"

bool Generic::updateGenericParameters(SemanticContext*              context,
                                      bool                          doType,
                                      bool                          doNode,
                                      VectorNative<TypeInfoParam*>& typeGenericParameters,
                                      VectorNative<AstNode*>&       nodeGenericParameters,
                                      AstNode*                      callGenericParameters,
                                      OneGenericMatch&              match)
{
    for (size_t i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        if (doType)
        {
            auto varDecl = CastAst<AstVarDecl>(nodeGenericParameters[i], AstNodeKind::FuncDeclParam);

            // If the user has specified a generic type, take it
            if (callGenericParameters && i < callGenericParameters->childs.size())
            {
                auto genParam             = callGenericParameters->childs[i];
                param->typeInfo           = genParam->typeInfo;
                varDecl->genTypeComesFrom = genParam;

                if (genParam->computedValue)
                {
                    param->allocateComputedValue();
                    *param->value = *genParam->computedValue;
                    if (genParam->hasComputedValue() && !genParam->isConstantGenTypeInfo())
                        param->flags |= TYPEINFOPARAM_DEFINED_VALUE;
                }
            }

            // If we have a calltype filled with the match, take it
            else if (match.genericParametersCallTypes[i])
            {
                param->typeInfo = match.genericParametersCallTypes[i];
                if (match.genericParametersCallFrom.size() > i)
                    varDecl->genTypeComesFrom = match.genericParametersCallFrom[i];
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

        // :GenericConcreteAlias
        // In case of an alias, instantiate with the concrete type
        param->typeInfo = TypeManager::concreteType(param->typeInfo, CONCRETE_ALIAS);

        // Value
        auto it1 = match.genericReplaceValues.find(param->name);
        if (it1 != match.genericReplaceValues.end() && it1->second)
        {
            param->flags |= TYPEINFOPARAM_DEFINED_VALUE;
            param->allocateComputedValue();
            *param->value = *it1->second;
        }

        // We should not instantiate with unresolved types
        auto genGen = match.genericParametersGenTypes[i];
        if (genGen->isKindGeneric())
        {
            if (param->typeInfo->isUntypedInteger() || param->typeInfo->isUntypedFloat())
            {
                auto symbol  = match.symbolName;
                auto errNode = context->node;

                TypeInfo* errType = nullptr;
                for (auto& v : match.genericReplaceFrom)
                {
                    if (v.second->typeInfo->isUntypedInteger() || v.second->typeInfo->isUntypedFloat())
                    {
                        errType = v.second->typeInfo;
                        errNode = v.second;
                        break;
                    }

                    if (v.second->typeInfo->isListArray())
                    {
                        auto listArr = CastTypeInfo<TypeInfoList>(v.second->typeInfo, TypeInfoKind::TypeListArray);
                        if (listArr->subTypes[0]->typeInfo->isUntypedInteger() || listArr->subTypes[0]->typeInfo->isUntypedFloat())
                        {
                            errType = v.second->typeInfo = listArr->subTypes[0]->typeInfo;
                            if (v.second->kind == AstNodeKind::FuncCallParam)
                                errNode = v.second->childs.front()->childs.front();
                            else
                                errNode = v.second->childs.front();
                            break;
                        }
                    }
                }

                Diagnostic diag{errNode, errNode->token, Utf8::format(Err(Err0808), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};
                if (errType && errType->isUntypedInteger())
                    diag.hint = Nte(Nte1052);
                else if (errType && errType->isUntypedFloat())
                    diag.hint = Nte(Nte1052);
                return context->report(diag);
            }
        }

        if (doNode)
        {
            auto nodeParam = nodeGenericParameters[i];
            nodeParam->setFlagsValueIsComputed();
            nodeParam->kind = AstNodeKind::ConstDecl;
            nodeParam->flags |= AST_FROM_GENERIC;
            if (param->value)
            {
                nodeParam->allocateComputedValue();
                *nodeParam->computedValue = *param->value;

                // :SliceLiteral
                if (param->typeInfo->isListArray() && nodeParam->typeInfo->isSlice())
                {
                    SwagSlice* slice;
                    auto       storageSegment               = nodeParam->computedValue->storageSegment;
                    nodeParam->computedValue->storageOffset = storageSegment->reserve(sizeof(SwagSlice), (uint8_t**) &slice);
                    auto typeList                           = CastTypeInfo<TypeInfoList>(param->typeInfo, TypeInfoKind::TypeListArray);
                    slice->buffer                           = storageSegment->address(param->value->storageOffset);
                    slice->count                            = typeList->subTypes.size();
                }
            }
        }
    }

    return true;
}

TypeInfo* Generic::doTypeSubstitution(VectorMap<Utf8, TypeInfo*>& replaceTypes, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (replaceTypes.empty())
        return typeInfo;
    if (!typeInfo->isGeneric())
        return typeInfo;

    // Search if the type has a corresponding replacement
    auto it = replaceTypes.find(typeInfo->name);
    if (it != replaceTypes.end())
    {
        // We can have a match on a lambda for a function attribute, when function has been generated
        // In that case, we want to be sure that the kind is function
        if (typeInfo->isFuncAttr() && it->second->isLambdaClosure())
        {
            auto t  = it->second->clone();
            t->kind = TypeInfoKind::FuncAttr;
            return t;
        }

        // Do not substitute with unconverted TypeList
        if (!it->second->isListArray())
            return it->second;
        else if (typeInfo->isKindGeneric())
            return TypeManager::convertTypeListToArray(nullptr, (TypeInfoList*) it->second, true);
    }

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypedVariadic:
    {
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        auto newType      = doTypeSubstitution(replaceTypes, typeVariadic->rawType);
        if (newType != typeVariadic->rawType)
        {
            typeVariadic          = CastTypeInfo<TypeInfoVariadic>(typeVariadic->clone(), TypeInfoKind::TypedVariadic);
            typeVariadic->rawType = newType;
            typeVariadic->removeGenericFlag();
            typeVariadic->forceComputeName();
            return typeVariadic;
        }

        break;
    }

    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto newType   = doTypeSubstitution(replaceTypes, typeAlias->rawType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = CastTypeInfo<TypeInfoAlias>(typeAlias->clone(), TypeInfoKind::Alias);
            typeAlias->rawType = newType;
            typeAlias->removeGenericFlag();
            typeAlias->forceComputeName();
            return typeAlias;
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto newType     = doTypeSubstitution(replaceTypes, typePointer->pointedType);
        if (newType != typePointer->pointedType)
        {
            typePointer = g_TypeMgr->makePointerTo(newType, (typePointer->flags & ~TYPEINFO_GENERIC) | TYPEINFO_FROM_GENERIC);
            return typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto      typeArray      = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto      newPointedType = doTypeSubstitution(replaceTypes, typeArray->pointedType);
        TypeInfo* newFinalType;
        if (typeArray->pointedType == typeArray->finalType)
            newFinalType = newPointedType;
        else
            newFinalType = doTypeSubstitution(replaceTypes, typeArray->finalType);
        if (newPointedType != typeArray->pointedType || newFinalType != typeArray->finalType)
        {
            typeArray              = CastTypeInfo<TypeInfoArray>(typeArray->clone(), TypeInfoKind::Array);
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
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto newType   = doTypeSubstitution(replaceTypes, typeSlice->pointedType);
        if (newType != typeSlice->pointedType)
        {
            typeSlice              = CastTypeInfo<TypeInfoSlice>(typeSlice->clone(), TypeInfoKind::Slice);
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
        auto              typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

        auto newType = doTypeSubstitution(replaceTypes, typeLambda->returnType);
        if (newType != typeLambda->returnType)
        {
            newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), typeLambda->kind);
            newLambda->removeGenericFlag();
            newLambda->returnType   = newType;
            newLambda->replaceTypes = replaceTypes;
        }

        auto numParams = typeLambda->parameters.size();
        for (size_t idx = 0; idx < numParams; idx++)
        {
            auto param = typeLambda->parameters[idx];
            newType    = doTypeSubstitution(replaceTypes, param->typeInfo);

            // If generic parameter is a closure, but the instantiated type is a lambda, then a conversion will
            // take place.
            // But we want to be sure that the closure stays a closure in ordre for the conversion to take place.
            // So transform the replaced lambda type by a closure one.
            if (param->typeInfo->isClosure() && newType->isLambda())
            {
                newType         = newType->clone();
                newType->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
                newType->flags |= TYPEINFO_CLOSURE;

                auto newParam      = g_TypeMgr->makeParam();
                newParam->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
                auto newTypeFct    = CastTypeInfo<TypeInfoFuncAttr>(newType, newType->kind);
                newTypeFct->parameters.push_front(newParam);
            }

            if (newType != param->typeInfo)
            {
                if (!newLambda)
                {
                    newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                    newLambda->removeGenericFlag();
                    newLambda->replaceTypes = replaceTypes;
                }

                auto newParam      = newLambda->parameters[idx];
                newParam->typeInfo = newType;
            }
        }

        if (newLambda)
        {
            newLambda->forceComputeName();
            return newLambda;
        }

        break;
    }

    default:
        break;
    }

    return typeInfo;
}

Job* Generic::end(SemanticContext* context, Job* job, SymbolName* symbol, AstNode* newNode, bool waitSymbol, VectorMap<Utf8, TypeInfo*>& replaceTypes)
{
    // Need to wait for the struct/function to be semantic resolved
    symbol->cptOverloads++;
    symbol->cptOverloadsInit++;
    symbol->nodes.push_back(newNode);

    if (waitSymbol && context->result == ContextResult::Done)
        Semantic::waitSymbolNoLock(job, symbol);

    // Run semantic on that struct/function
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(job->dependentJob, sourceFile, newNode, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->errCxtSteps.insert(destCxt->errCxtSteps.end(), srcCxt->errCxtSteps.begin(), srcCxt->errCxtSteps.end());

    // New context
    ErrorCxtStep expNode;
    expNode.node         = context->node;
    expNode.replaceTypes = replaceTypes;
    if (expNode.node->hasExtMisc() && expNode.node->extMisc()->exportNode)
        expNode.node = expNode.node->extMisc()->exportNode;
    expNode.type = ErrCxtStepKind::Generic;
    destCxt->errCxtSteps.push_back(expNode);

    return newJob;
}

void Generic::setUserGenericTypeReplacement(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters)
{
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();
    if (numGenericParams == 0 && wantedNumGenericParams == 0)
        return;

    context.genericParametersCallTypes.expand_clear(wantedNumGenericParams);
    context.genericParametersCallValues.expand_clear(wantedNumGenericParams);
    context.genericParametersCallFrom.expand_clear(wantedNumGenericParams);
    context.genericParametersGenTypes.set_size_clear(wantedNumGenericParams);

    if (numGenericParams > wantedNumGenericParams)
    {
        context.badSignatureInfos.badSignatureNum1 = numGenericParams;
        context.badSignatureInfos.badSignatureNum2 = wantedNumGenericParams;
        context.result                             = MatchResult::TooManyGenericParameters;
        return;
    }

    for (int i = 0; i < wantedNumGenericParams; i++)
    {
        auto genType = genericParameters[i];

        context.mapGenericTypesIndex[genType->typeInfo->name] = i;
        context.genericParametersGenTypes[i]                  = genType->typeInfo;
    }

    /* if (numGenericParams == wantedNumGenericParams)
    {
        context.genericReplaceTypes.clear();
        context.genericReplaceValues.clear();
        context.genericReplaceFrom.clear();
    }*/

    for (int i = 0; i < numGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto& genTypeName = genericParameters[i]->typeInfo->name;
        auto        genNode     = context.genericParameters[i];
        if (!context.genericParametersCallTypes[i])
        {
            context.genericReplaceTypes[genTypeName] = genNode->typeInfo;
            context.genericReplaceValues[genName]    = genNode->computedValue;
            context.genericReplaceFrom[genTypeName]  = genNode;

            context.genericParametersCallTypes[i]  = genNode->typeInfo;
            context.genericParametersCallValues[i] = genNode->computedValue;
            context.genericParametersCallFrom[i]   = genNode;
        }
        else
        {
            context.genericReplaceTypes[genTypeName] = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
            context.genericReplaceFrom[genTypeName]  = context.genericParametersCallFrom[i];
        }
    }

    for (auto i = numGenericParams; i < wantedNumGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto& genTypeName = genericParameters[i]->typeInfo->name;
        if (context.genericParametersCallTypes[i])
        {
            context.genericReplaceTypes[genTypeName] = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
            context.genericReplaceFrom[genTypeName]  = context.genericParametersCallFrom[i];
        }
        else
        {
            auto it = context.genericReplaceTypes.find(genTypeName);
            if (it != context.genericReplaceTypes.end())
                context.genericParametersCallTypes[i] = it->second;

            auto it1 = context.genericReplaceValues.find(genName);
            if (it1 != context.genericReplaceValues.end())
                context.genericParametersCallValues[i] = it1->second;

            auto it2 = context.genericReplaceFrom.find(genTypeName);
            if (it2 != context.genericReplaceFrom.end())
                context.genericParametersCallFrom[i] = it2->second;
        }
    }
}

void Generic::setContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags)
{
    auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypesIndex.clear();

    auto& toCheck = context->tmpNodes;
    toCheck.clear();

    // If we are inside a struct, then we can inherit the generic concrete types of that struct
    if (node->ownerStructScope)
        toCheck.push_back(node->ownerStructScope->owner);

    // If function A in a struct calls function B in the same struct, then we can inherit the match types of function A
    // when instantiating function B
    if (node->ownerFct && node->ownerStructScope && node->ownerFct->ownerStructScope == symOverload->node->ownerStructScope)
        toCheck.push_back(node->ownerFct);

    // We do not want function to deduce their generic type from context, as the generic type can be deduced from the
    // parameters
    if (node->ownerFct && !symOverload->typeInfo->isFuncAttr())
        toCheck.push_back(node->ownerFct);

    // Except for a second try
    if (node->ownerFct && symOverload->typeInfo->isFuncAttr() && flags & MIP_SECOND_GENERIC_TRY)
        toCheck.push_back(node->ownerFct);

    // With A.B form, we try to get generic parameters from A if they exist
    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef()->startScope)
            toCheck.push_back(identifier->identifierRef()->startScope->owner);
    }

    // Except that with using, B could be in fact in another struct than A.
    // In that case we have a dependentVar, so replace what needs to be replaced.
    // What a mess...
    if (oneTryMatch.dependentVarLeaf)
    {
        if (oneTryMatch.dependentVarLeaf->typeInfo && oneTryMatch.dependentVarLeaf->typeInfo->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(oneTryMatch.dependentVarLeaf->typeInfo, TypeInfoKind::Struct);
            toCheck.push_back(typeStruct->declNode);
        }
    }

    // Collect from the owner structure
    for (auto one : toCheck)
    {
        if (one->kind == AstNodeKind::FuncDecl)
        {
            auto nodeFunc = CastAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->typeInfo, TypeInfoKind::FuncAttr);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeFunc->replaceTypes.size());
            for (auto oneReplace : typeFunc->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeFunc->replaceValues.size());
            for (auto oneReplace : typeFunc->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceFrom.reserve(typeFunc->replaceFrom.size());
            for (auto oneReplace : typeFunc->replaceFrom)
                oneTryMatch.symMatchContext.genericReplaceFrom[oneReplace.first] = oneReplace.second;
        }
        else if (one->kind == AstNodeKind::StructDecl)
        {
            auto nodeStruct = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(nodeStruct->typeInfo, TypeInfoKind::Struct);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeStruct->replaceTypes.size());
            for (auto oneReplace : typeStruct->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeStruct->replaceValues.size());
            for (auto oneReplace : typeStruct->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceFrom.reserve(typeStruct->replaceFrom.size());
            for (auto oneReplace : typeStruct->replaceFrom)
                oneTryMatch.symMatchContext.genericReplaceFrom[oneReplace.first] = oneReplace.second;
        }
    }
}
