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
                for (auto& v : match.genericReplaceTypes)
                {
                    auto fromNode = v.second.fromNode;
                    if (!fromNode)
                        continue;

                    if (fromNode->typeInfo->isUntypedInteger() || fromNode->typeInfo->isUntypedFloat())
                    {
                        errType = fromNode->typeInfo;
                        errNode = fromNode;
                        break;
                    }

                    if (fromNode->typeInfo->isListArray())
                    {
                        auto listArr = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListArray);
                        if (listArr->subTypes[0]->typeInfo->isUntypedInteger() || listArr->subTypes[0]->typeInfo->isUntypedFloat())
                        {
                            errType = fromNode->typeInfo = listArr->subTypes[0]->typeInfo;
                            if (fromNode->kind == AstNodeKind::FuncCallParam)
                                errNode = fromNode->childs.front()->childs.front();
                            else
                                errNode = fromNode->childs.front();
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

TypeInfo* Generic::doTypeSubstitution(VectorMap<Utf8, GenericReplaceType>& replaceTypes, TypeInfo* typeInfo)
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
        if (typeInfo->isFuncAttr() && it->second.typeInfoReplace->isLambdaClosure())
        {
            auto t  = it->second.typeInfoReplace->clone();
            t->kind = TypeInfoKind::FuncAttr;
            return t;
        }

        // Do not substitute with unconverted TypeList
        if (!it->second.typeInfoReplace->isListArray())
            return it->second.typeInfoReplace;
        else if (typeInfo->isKindGeneric())
            return TypeManager::convertTypeListToArray(nullptr, (TypeInfoList*) it->second.typeInfoReplace, true);
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

Job* Generic::end(SemanticContext* context, Job* job, SymbolName* symbol, AstNode* newNode, bool waitSymbol, VectorMap<Utf8, GenericReplaceType>& replaceTypes)
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

void Generic::deduceGenericParam(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, uint64_t castFlags)
{
    SWAG_ASSERT(wantedTypeInfo->isGeneric());

    // Need to register inside types when the generic type is a compound
    VectorNative<TypeInfo*> symbolTypeInfos;
    symbolTypeInfos.push_back(wantedTypeInfo);
    VectorNative<TypeInfo*> typeInfos;
    typeInfos.push_back(callTypeInfo);
    while (symbolTypeInfos.size())
    {
        wantedTypeInfo = symbolTypeInfos.back();

        // When we have a reference, we match with the real type, as we do not want a generic function/struct to have a
        // reference as a concrete type
        callTypeInfo = TypeManager::concretePtrRef(typeInfos.back());

        symbolTypeInfos.pop_back();
        typeInfos.pop_back();

        if (!callTypeInfo)
            continue;

        // Do we already have mapped the generic parameter to something ?
        auto it = context.genericReplaceTypes.find(wantedTypeInfo->name);
        if (it != context.genericReplaceTypes.end())
        {
            bool same = false;

            // If user type is undefined, then we consider this is ok, because the undefined type will be changed to the generic one
            // Match is in fact the other way
            if (callTypeInfo->isNative(NativeTypeKind::Undefined))
                same = true;

            // Yes, and the Map is not the same, then this is an error
            else
            {
                same = TypeManager::makeCompatibles(context.semContext, it->second.typeInfoReplace, callTypeInfo, nullptr, nullptr, CASTFLAG_JUST_CHECK | CASTFLAG_PARAMS | castFlags);
                if (context.semContext->result != ContextResult::Done)
                    return;
            }

            if (!same)
            {
                context.badSignatureInfos.badSignatureParameterIdx  = idxParam;
                context.badSignatureInfos.badSignatureRequestedType = it->second.typeInfoReplace;
                context.badSignatureInfos.genMatchFromNode          = it->second.fromNode;
                context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                context.badSignatureInfos.badGenMatch               = wantedTypeInfo->name;
                SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                context.result = MatchResult::BadSignature;
            }
        }
        else
        {
            bool canReg = true;
            if (wantedTypeInfo->isPointer())
                canReg = false;
            else if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
                canReg = wantedTypeInfo->isSame(callTypeInfo, CASTFLAG_CAST);

            // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
            // yet the type of parameters)
            if (callTypeInfo->declNode && (callTypeInfo->declNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING))
                canReg = false;

            if (canReg)
            {
                // We could have match a non const against a const
                // We need to instantiate with the const equivalent, so make the call type const
                if (!callTypeInfo->isConst() && wantedTypeInfo->isConst())
                    callTypeInfo = g_TypeMgr->makeConst(callTypeInfo);

                auto regTypeInfo = callTypeInfo;

                // :DupGen
                if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
                {
                    auto callStruct   = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                    auto wantedStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
                    if (callStruct->genericParameters.size() == wantedStruct->genericParameters.size() && callStruct->genericParameters.size())
                    {
                        auto newStructType = (TypeInfoStruct*) callStruct->clone();
                        for (size_t i = 0; i < callStruct->genericParameters.size(); i++)
                        {
                            newStructType->genericParameters[i]->name = wantedStruct->genericParameters[i]->typeInfo->name;
                        }

                        regTypeInfo = newStructType;
                    }
                }

                if (wantedTypeInfo->isKindGeneric() && regTypeInfo->isListTuple())
                {
                    context.badSignatureInfos.badSignatureParameterIdx  = idxParam;
                    context.badSignatureInfos.badSignatureRequestedType = wantedTypeInfo;
                    context.badSignatureInfos.badSignatureGivenType     = callTypeInfo;
                    SWAG_ASSERT(context.badSignatureInfos.badSignatureRequestedType);
                    context.result = MatchResult::CannotDeduceGenericType;
                    return;
                }

                // Associate the generic type with that concrete one
                GenericReplaceType st;
                st.typeInfoGeneric                                = wantedTypeInfo;
                st.typeInfoReplace                                = regTypeInfo;
                st.fromNode                                       = callParameter;
                context.genericReplaceTypes[wantedTypeInfo->name] = st;

                // If this is a valid generic argument, register it at the correct call position
                auto itIdx = context.mapGenericTypesIndex.find(wantedTypeInfo->name);
                if (itIdx != context.mapGenericTypesIndex.end())
                {
                    context.genericParametersCallTypes[itIdx->second] = callTypeInfo;
                    context.genericParametersCallFrom[itIdx->second]  = callParameter;
                }
            }
        }

        switch (wantedTypeInfo->kind)
        {
        case TypeInfoKind::Struct:
        {
            auto symbolStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
            if (callTypeInfo->isStruct())
            {
                auto typeStruct = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                if (!typeStruct->genericParameters.empty())
                {
                    auto num = min(symbolStruct->genericParameters.size(), typeStruct->genericParameters.size());
                    for (size_t idx = 0; idx < num; idx++)
                    {
                        auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                        auto rawTypeInfo = typeStruct->genericParameters[idx]->typeInfo;
                        symbolTypeInfos.push_back(genTypeInfo);
                        typeInfos.push_back(rawTypeInfo);
                    }
                }
                else
                {
                    auto num = min(symbolStruct->genericParameters.size(), typeStruct->deducedGenericParameters.size());
                    for (size_t idx = 0; idx < num; idx++)
                    {
                        auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                        auto rawTypeInfo = typeStruct->deducedGenericParameters[idx];
                        symbolTypeInfos.push_back(genTypeInfo);
                        typeInfos.push_back(rawTypeInfo);
                    }
                }
            }
            else if (callTypeInfo->isListTuple())
            {
                auto typeList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListTuple);
                auto num      = min(symbolStruct->genericParameters.size(), typeList->subTypes.size());
                for (size_t idx = 0; idx < num; idx++)
                {
                    // A tuple typelist like {a: 1, b: 2} can have named parameters, which means that the order of
                    // fields is irrelevant, as we can write {b: 2, a: 1} too.
                    //
                    // We have a generic parameter. We search in the struct the field that correspond to that type, in
                    // order to get the corresponding field name. Then we will search for the name in the typelist (if
                    // specified).
                    auto p       = symbolStruct->genericParameters[idx];
                    Utf8 nameVar = p->name;
                    for (size_t idx1 = 0; idx1 < symbolStruct->fields.size(); idx1++)
                    {
                        if (symbolStruct->fields[idx1]->typeInfo->name == symbolStruct->genericParameters[idx]->typeInfo->name)
                        {
                            nameVar = symbolStruct->fields[idx1]->name;
                            break;
                        }
                    }

                    // Then the corresponding field name is searched in the typelist in case the user has specified one.
                    auto p1        = typeList->subTypes[idx];
                    auto typeField = p1->typeInfo;
                    for (size_t j = 0; j < typeList->subTypes.size(); j++)
                    {
                        if (nameVar == typeList->subTypes[j]->name)
                        {
                            typeField = typeList->subTypes[j]->typeInfo;
                            break;
                        }
                    }

                    auto genTypeInfo = p->typeInfo;
                    auto rawTypeInfo = typeField;
                    symbolTypeInfos.push_back(genTypeInfo);
                    typeInfos.push_back(rawTypeInfo);
                }
            }

            break;
        }

        case TypeInfoKind::Pointer:
        {
            auto symbolPtr = CastTypeInfo<TypeInfoPointer>(wantedTypeInfo, TypeInfoKind::Pointer);
            if (callTypeInfo->isPointer())
            {
                auto typePtr = CastTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
                if (symbolPtr->isPointerTo(TypeInfoKind::Struct) && typePtr->isPointerTo(TypeInfoKind::Struct))
                {
                    // Because of using var cast, we can have here *A and *B with a match.
                    // But we do not want A and B to match in generic replacement.
                    // So we check they are the same.
                    auto canNext = symbolPtr->pointedType->isSame(typePtr->pointedType, CASTFLAG_CAST);
                    if (canNext)
                    {
                        symbolTypeInfos.push_back(symbolPtr->pointedType);
                        typeInfos.push_back(typePtr->pointedType);
                    }
                }
                else
                {
                    symbolTypeInfos.push_back(symbolPtr->pointedType);
                    typeInfos.push_back(typePtr->pointedType);
                }
            }
            else if (callTypeInfo->isStruct())
            {
                // Because of using var cast, we can have here *A and *B with a match.
                // But we do not want A and B to match in generic replacement.
                // So we check they are the same.
                auto canNext = symbolPtr->pointedType->isSame(callTypeInfo, CASTFLAG_CAST);
                if (canNext)
                {
                    symbolTypeInfos.push_back(symbolPtr->pointedType);
                    typeInfos.push_back(callTypeInfo);
                }
            }
            else
            {
                symbolTypeInfos.push_back(symbolPtr->pointedType);
                typeInfos.push_back(callTypeInfo);
            }
            break;
        }

        case TypeInfoKind::Array:
        {
            auto symbolArray = CastTypeInfo<TypeInfoArray>(wantedTypeInfo, TypeInfoKind::Array);
            symbolTypeInfos.push_back(symbolArray->finalType);

            uint32_t count = 0;
            if (callTypeInfo->isArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                typeInfos.push_back(typeArray->finalType);
                count = typeArray->count;
            }
            else
            {
                SWAG_ASSERT(callTypeInfo->isListArray());
                auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                typeInfos.push_back(typeArray->subTypes[0]->typeInfo);
                count = typeArray->subTypes.count;
            }

            // Array dimension was a generic symbol. Set the corresponding symbol in order to check its value
            if (symbolArray->isGeneric() && symbolArray->flags & TYPEINFO_GENERIC_COUNT)
            {
                SWAG_ASSERT(symbolArray->sizeNode);
                SWAG_ASSERT(symbolArray->sizeNode->resolvedSymbolName);

                ComputedValue* cv = Allocator::alloc<ComputedValue>();
                cv->reg.s64       = count;

                // Constant already defined ?
                auto& cstName = symbolArray->sizeNode->resolvedSymbolName->name;
                auto  it1     = context.genericReplaceValues.find(cstName);
                if (it1 != context.genericReplaceValues.end())
                {
                    if (!Semantic::valueEqualsTo(it1->second, cv, symbolArray->sizeNode->typeInfo, 0))
                    {
                        context.badSignatureInfos.badNode               = callParameter;
                        context.badSignatureInfos.badGenMatch           = cstName;
                        context.badSignatureInfos.badGenValue1          = it1->second;
                        context.badSignatureInfos.badGenValue2          = cv;
                        context.badSignatureInfos.badSignatureGivenType = symbolArray->sizeNode->typeInfo;
                        context.result                                  = MatchResult::MismatchGenericValue;
                    }
                    else
                    {
                        Allocator::free<ComputedValue>(cv);

                        GenericReplaceType st;
                        st.typeInfoGeneric = symbolArray->sizeNode->typeInfo;
                        st.typeInfoReplace = symbolArray->sizeNode->typeInfo;
                        st.fromNode        = symbolArray->sizeNode;

                        context.genericReplaceTypes[symbolArray->sizeNode->typeInfo->name] = st;
                    }
                }
                else
                {
                    context.genericReplaceValues[cstName] = cv;

                    GenericReplaceType st;
                    st.typeInfoGeneric = symbolArray->sizeNode->typeInfo;
                    st.typeInfoReplace = symbolArray->sizeNode->typeInfo;
                    st.fromNode        = symbolArray->sizeNode;

                    context.genericReplaceTypes[symbolArray->sizeNode->typeInfo->name] = st;
                }
            }

            break;
        }

        case TypeInfoKind::Slice:
        {
            auto symbolSlice = CastTypeInfo<TypeInfoSlice>(wantedTypeInfo, TypeInfoKind::Slice);
            if (callTypeInfo->isSlice())
            {
                auto typeSlice = CastTypeInfo<TypeInfoSlice>(callTypeInfo, TypeInfoKind::Slice);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeSlice->pointedType);
            }
            else if (callTypeInfo->isArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeArray->pointedType);
            }
            else if (callTypeInfo->isListArray())
            {
                auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(typeArray->subTypes[0]->typeInfo);
            }
            else
            {
                symbolTypeInfos.push_back(symbolSlice->pointedType);
                typeInfos.push_back(callTypeInfo);
            }
            break;
        }

        case TypeInfoKind::LambdaClosure:
        {
            auto symbolLambda = CastTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::LambdaClosure);
            auto typeLambda   = CastTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::LambdaClosure);
            if (symbolLambda->returnType && symbolLambda->returnType->isGeneric())
            {
                symbolTypeInfos.push_back(symbolLambda->returnType);
                typeInfos.push_back(typeLambda->returnType);
            }

            auto num = symbolLambda->parameters.size();
            for (size_t idx = 0; idx < num; idx++)
            {
                if (symbolLambda->isClosure() && !idx)
                    continue;
                TypeInfoParam* symbolParam = symbolLambda->parameters[idx];

                TypeInfoParam* typeParam;
                if (symbolLambda->isClosure() && typeLambda->isLambda())
                    typeParam = typeLambda->parameters[idx - 1];
                else
                    typeParam = typeLambda->parameters[idx];

                if (symbolParam->typeInfo->isGeneric() &&
                    !typeParam->typeInfo->isNative(NativeTypeKind::Undefined))
                {
                    symbolTypeInfos.push_back(symbolParam->typeInfo);
                    typeInfos.push_back(typeParam->typeInfo);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

void Generic::setUserGenericTypeReplacement(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters)
{
    int wantedNumGenericParams = (int) genericParameters.size();
    int numGenericParams       = (int) context.genericParameters.size();
    if (!numGenericParams && !wantedNumGenericParams)
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
        auto genType                                          = genericParameters[i];
        context.mapGenericTypesIndex[genType->typeInfo->name] = i;
        context.genericParametersGenTypes[i]                  = genType->typeInfo;
    }

    GenericReplaceType st;
    for (int i = 0; i < numGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto& genTypeName = genericParameters[i]->typeInfo->name;
        auto        genNode     = context.genericParameters[i];
        if (!context.genericParametersCallTypes[i])
        {
            st.typeInfoGeneric = genericParameters[i]->typeInfo;
            st.typeInfoReplace = genNode->typeInfo;
            st.fromNode        = genNode;

            context.genericReplaceTypes[genTypeName] = st;
            context.genericReplaceValues[genName]    = genNode->computedValue;

            context.genericParametersCallTypes[i]  = genNode->typeInfo;
            context.genericParametersCallValues[i] = genNode->computedValue;
            context.genericParametersCallFrom[i]   = genNode;
        }
        else
        {
            st.typeInfoGeneric = nullptr;
            st.typeInfoReplace = context.genericParametersCallTypes[i];
            st.fromNode        = context.genericParametersCallFrom[i];

            context.genericReplaceTypes[genTypeName] = st;
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
        }
    }

    for (auto i = numGenericParams; i < wantedNumGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto& genTypeName = genericParameters[i]->typeInfo->name;
        if (context.genericParametersCallTypes[i])
        {
            st.typeInfoGeneric = nullptr;
            st.typeInfoReplace = context.genericParametersCallTypes[i];
            st.fromNode        = context.genericParametersCallFrom[i];

            context.genericReplaceTypes[genTypeName] = st;
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
        }
        else
        {
            auto it = context.genericReplaceTypes.find(genTypeName);
            if (it != context.genericReplaceTypes.end())
            {
                context.genericParametersCallTypes[i] = it->second.typeInfoReplace;
                context.genericParametersCallFrom[i]  = it->second.fromNode;
            }

            auto it1 = context.genericReplaceValues.find(genName);
            if (it1 != context.genericReplaceValues.end())
                context.genericParametersCallValues[i] = it1->second;
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
        }
    }
}
