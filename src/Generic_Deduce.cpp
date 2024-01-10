#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"

void Generic::deduceSubType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo*& callTypeInfo, VectorNative<TypeInfo*>& wantedTypeInfos, VectorNative<TypeInfo*>& callTypeInfos, AstNode* callParameter)
{
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
                    wantedTypeInfos.push_back(genTypeInfo);
                    callTypeInfos.push_back(rawTypeInfo);
                }
            }
            else
            {
                auto num = min(symbolStruct->genericParameters.size(), typeStruct->deducedGenericParameters.size());
                for (size_t idx = 0; idx < num; idx++)
                {
                    auto genTypeInfo = symbolStruct->genericParameters[idx]->typeInfo;
                    auto rawTypeInfo = typeStruct->deducedGenericParameters[idx];
                    wantedTypeInfos.push_back(genTypeInfo);
                    callTypeInfos.push_back(rawTypeInfo);
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
                wantedTypeInfos.push_back(genTypeInfo);
                callTypeInfos.push_back(rawTypeInfo);
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
                    wantedTypeInfos.push_back(symbolPtr->pointedType);
                    callTypeInfos.push_back(typePtr->pointedType);
                }
            }
            else
            {
                wantedTypeInfos.push_back(symbolPtr->pointedType);
                callTypeInfos.push_back(typePtr->pointedType);
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
                wantedTypeInfos.push_back(symbolPtr->pointedType);
                callTypeInfos.push_back(callTypeInfo);
            }
        }
        else
        {
            wantedTypeInfos.push_back(symbolPtr->pointedType);
            callTypeInfos.push_back(callTypeInfo);
        }
        break;
    }

    case TypeInfoKind::Array:
    {
        auto symbolArray = CastTypeInfo<TypeInfoArray>(wantedTypeInfo, TypeInfoKind::Array);
        wantedTypeInfos.push_back(symbolArray->finalType);

        uint32_t count = 0;
        if (callTypeInfo->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
            callTypeInfos.push_back(typeArray->finalType);
            count = typeArray->count;
        }
        else
        {
            SWAG_ASSERT(callTypeInfo->isListArray());
            auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
            callTypeInfos.push_back(typeArray->subTypes[0]->typeInfo);
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
            wantedTypeInfos.push_back(symbolSlice->pointedType);
            callTypeInfos.push_back(typeSlice->pointedType);
        }
        else if (callTypeInfo->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
            wantedTypeInfos.push_back(symbolSlice->pointedType);
            callTypeInfos.push_back(typeArray->pointedType);
        }
        else if (callTypeInfo->isListArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
            wantedTypeInfos.push_back(symbolSlice->pointedType);
            callTypeInfos.push_back(typeArray->subTypes[0]->typeInfo);
        }
        else
        {
            wantedTypeInfos.push_back(symbolSlice->pointedType);
            callTypeInfos.push_back(callTypeInfo);
        }
        break;
    }

    case TypeInfoKind::LambdaClosure:
    {
        auto symbolLambda = CastTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::LambdaClosure);
        auto typeLambda   = CastTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::LambdaClosure);
        if (symbolLambda->returnType && symbolLambda->returnType->isGeneric() && !typeLambda->returnType->isUndefined())
        {
            wantedTypeInfos.push_back(symbolLambda->returnType);
            callTypeInfos.push_back(typeLambda->returnType);
        }

        auto num = symbolLambda->parameters.size();
        for (size_t idx = 0; idx < num; idx++)
        {
            if (symbolLambda->isClosure() && !idx)
                continue;

            TypeInfoParam* typeParam;
            if (symbolLambda->isClosure() && typeLambda->isLambda())
                typeParam = typeLambda->parameters[idx - 1];
            else
                typeParam = typeLambda->parameters[idx];

            TypeInfoParam* symbolParam = symbolLambda->parameters[idx];
            if (symbolParam->typeInfo->isGeneric() && !typeParam->typeInfo->isUndefined())
            {
                wantedTypeInfos.push_back(symbolParam->typeInfo);
                callTypeInfos.push_back(typeParam->typeInfo);
            }
        }
        break;
    }
    default:
        break;
    }
}

void Generic::deduceGenericTypeReplacement(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, uint64_t castFlags)
{
    SWAG_ASSERT(wantedTypeInfo->isGeneric());

    VectorNative<TypeInfo*> wantedTypeInfos;
    VectorNative<TypeInfo*> callTypeInfos;
    wantedTypeInfos.push_back(wantedTypeInfo);
    callTypeInfos.push_back(callTypeInfo);

    while (wantedTypeInfos.size())
    {
        // When we have a reference, we match with the real type, as we do not want a generic function/struct to have a
        // reference as a concrete type
        callTypeInfo   = TypeManager::concretePtrRef(callTypeInfos.get_pop_back());
        wantedTypeInfo = wantedTypeInfos.get_pop_back();

        if (!callTypeInfo)
            continue;

        // Do we already have mapped the generic parameter to something ?
        auto it = context.genericReplaceTypes.find(wantedTypeInfo->name);
        if (it != context.genericReplaceTypes.end())
        {
            // We must be sure that the registered type is the same as the new one
            bool same = TypeManager::makeCompatibles(context.semContext, it->second.typeInfoReplace, callTypeInfo, nullptr, nullptr, CASTFLAG_JUST_CHECK | CASTFLAG_PARAMS | castFlags);
            if (context.semContext->result != ContextResult::Done)
                return;

            // The previous type is not the same, then this is an error
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

            deduceSubType(context, wantedTypeInfo, callTypeInfo, wantedTypeInfos, callTypeInfos, callParameter);
            continue;
        }

        bool canReg = true;

        // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
        // yet the type of parameters)
        if (callTypeInfo->declNode && (callTypeInfo->declNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING))
            canReg = false;
        else if (wantedTypeInfo->isPointer())
            canReg = false;
        else if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
            canReg = wantedTypeInfo->isSame(callTypeInfo, CASTFLAG_CAST);

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
                if (callStruct->genericParameters.size() == wantedStruct->genericParameters.size() &&
                    callStruct->genericParameters.size())
                {
                    auto newStructType = (TypeInfoStruct*) callStruct->clone();
                    for (size_t i = 0; i < callStruct->genericParameters.size(); i++)
                        newStructType->genericParameters[i]->name = wantedStruct->genericParameters[i]->typeInfo->name;
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
            auto itIdx = context.mapGenericTypeToIndex.find(wantedTypeInfo->name);
            if (itIdx != context.mapGenericTypeToIndex.end())
            {
                st.typeInfoGeneric                                = wantedTypeInfo;
                st.typeInfoReplace                                = callTypeInfo;
                st.fromNode                                       = callParameter;
                context.genericParametersCallTypes[itIdx->second] = st;
            }
        }

        deduceSubType(context, wantedTypeInfo, callTypeInfo, wantedTypeInfos, callTypeInfos, callParameter);
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

    if (numGenericParams > wantedNumGenericParams)
    {
        context.badSignatureInfos.badSignatureNum1 = numGenericParams;
        context.badSignatureInfos.badSignatureNum2 = wantedNumGenericParams;
        context.result                             = MatchResult::TooManyGenericParameters;
        return;
    }

    GenericReplaceType                  st;
    VectorMap<Utf8, GenericReplaceType> m;

    for (int i = 0; i < numGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        auto        genType     = genericParameters[i]->typeInfo;
        const auto& genTypeName = genType->name;
        auto        genNode     = context.genericParameters[i];

        if (!context.genericParametersCallTypes[i].typeInfoReplace)
        {
            st.typeInfoGeneric = genType;
            st.typeInfoReplace = genNode->typeInfo;
            st.fromNode        = genNode;

            context.genericReplaceTypes[genTypeName] = st;
            context.genericParametersCallTypes[i]    = st;
            context.genericReplaceValues[genName]    = genNode->computedValue;
            context.genericParametersCallValues[i]   = genNode->computedValue;
        }
        else
        {
            context.genericParametersCallTypes[i].typeInfoGeneric = genType;
            context.genericReplaceTypes[genTypeName]              = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]                 = context.genericParametersCallValues[i];

            // We have a new type replacement, so we must be sure that every other types registered in the
            // context.genericReplaceTypes are up to date too.
            // For example if type T is now s32, we must be sure that a potential *T or [..] T in the map will
            // be updated correspondinly.
            if (context.genericReplaceTypes.size() > 1)
            {
                m.clear();
                m[genTypeName] = context.genericParametersCallTypes[i];

                for (auto& v : context.genericReplaceTypes)
                {
                    if (!v.second.typeInfoGeneric)
                        continue;
                    if (v.first == genTypeName)
                        continue;
                    auto typeInfoReplace = doTypeSubstitution(m, v.second.typeInfoGeneric);
                    if (typeInfoReplace != v.second.typeInfoGeneric)
                    {
                        v.second.typeInfoReplace = typeInfoReplace;
                    }
                }
            }
        }
    }

    for (auto i = numGenericParams; i < wantedNumGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        auto        genType     = genericParameters[i]->typeInfo;
        const auto& genTypeName = genType->name;

        if (context.genericParametersCallTypes[i].typeInfoReplace)
        {
            context.genericReplaceTypes[genTypeName] = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
        }
        else
        {
            auto it = context.genericReplaceTypes.find(genTypeName);
            if (it != context.genericReplaceTypes.end())
                context.genericParametersCallTypes[i] = it->second;

            auto it1 = context.genericReplaceValues.find(genName);
            if (it1 != context.genericReplaceValues.end())
                context.genericParametersCallValues[i] = it1->second;
        }
    }

    for (int i = 0; i < wantedNumGenericParams; i++)
    {
        auto genType                                          = genericParameters[i]->typeInfo;
        context.mapGenericTypeToIndex[genType->name]          = i;
        context.genericParametersCallTypes[i].typeInfoGeneric = genType;
    }
}

void Generic::setContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags)
{
    auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypeToIndex.clear();

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

    // Collect all
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
