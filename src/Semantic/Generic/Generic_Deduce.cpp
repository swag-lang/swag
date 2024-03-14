#include "pch.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

void Generic::deduceSubType(SymbolMatchContext&      context,
                            TypeInfo*                wantedTypeInfo,
                            TypeInfo*&               callTypeInfo,
                            VectorNative<TypeInfo*>& wantedTypeInfos,
                            VectorNative<TypeInfo*>& callTypeInfos,
                            AstNode*                 callParameter)
{
    switch (wantedTypeInfo->kind)
    {
        case TypeInfoKind::Struct:
        {
            const auto wantedStruct = castTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
            if (callTypeInfo->isStruct())
            {
                const auto callStruct = castTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
                if (!callStruct->genericParameters.empty())
                {
                    const auto num = min(wantedStruct->genericParameters.size(), callStruct->genericParameters.size());
                    for (uint32_t idx = 0; idx < num; idx++)
                    {
                        wantedTypeInfos.push_back(wantedStruct->genericParameters[idx]->typeInfo);
                        callTypeInfos.push_back(callStruct->genericParameters[idx]->typeInfo);
                    }
                }
                else
                {
                    const auto num = min(wantedStruct->genericParameters.size(), callStruct->deducedGenericParameters.size());
                    for (uint32_t idx = 0; idx < num; idx++)
                    {
                        wantedTypeInfos.push_back(wantedStruct->genericParameters[idx]->typeInfo);
                        callTypeInfos.push_back(callStruct->deducedGenericParameters[idx]);
                    }
                }
            }
            else if (callTypeInfo->isListTuple())
            {
                const auto callList = castTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListTuple);
                const auto num      = min(wantedStruct->genericParameters.size(), callList->subTypes.size());
                for (uint32_t idx = 0; idx < num; idx++)
                {
                    // A tuple typelist like {a: 1, b: 2} can have named parameters, which means that the order of
                    // fields is irrelevant, as we can write {b: 2, a: 1} too.
                    //
                    // We have a generic parameter. We search in the struct the field that correspond to that type, in
                    // order to get the corresponding field name. Then we will search for the name in the typelist (if
                    // specified).
                    const auto p       = wantedStruct->genericParameters[idx];
                    Utf8       nameVar = p->name;
                    for (uint32_t idx1 = 0; idx1 < wantedStruct->fields.size(); idx1++)
                    {
                        if (wantedStruct->fields[idx1]->typeInfo->name == wantedStruct->genericParameters[idx]->typeInfo->name)
                        {
                            nameVar = wantedStruct->fields[idx1]->name;
                            break;
                        }
                    }

                    // Then the corresponding field name is searched in the typelist in case the user has specified one.
                    const auto p1        = callList->subTypes[idx];
                    auto       typeField = p1->typeInfo;
                    for (const auto& subType : callList->subTypes)
                    {
                        if (nameVar == subType->name)
                        {
                            typeField = subType->typeInfo;
                            break;
                        }
                    }

                    wantedTypeInfos.push_back(p->typeInfo);
                    callTypeInfos.push_back(typeField);
                }
            }

            break;
        }

        case TypeInfoKind::Pointer:
        {
            const auto wantedPointer = castTypeInfo<TypeInfoPointer>(wantedTypeInfo, TypeInfoKind::Pointer);
            if (wantedPointer->isPointerTo(TypeInfoKind::Struct) && callTypeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                // Because of using var cast, we can have here *A and *B with a match.
                // But we do not want A and B to match in generic replacement.
                // So we check they are the same.
                const auto callPointer = castTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
                if (wantedPointer->pointedType->isSame(callPointer->pointedType, CAST_FLAG_CAST))
                {
                    wantedTypeInfos.push_back(wantedPointer->pointedType);
                    callTypeInfos.push_back(callPointer->pointedType);
                }
            }
            else if (callTypeInfo->isStruct())
            {
                // Because of using var cast, we can have here *A and *B with a match.
                // But we do not want A and B to match in generic replacement.
                // So we check they are the same.
                if (wantedPointer->pointedType->isSame(callTypeInfo, CAST_FLAG_CAST))
                {
                    wantedTypeInfos.push_back(wantedPointer->pointedType);
                    callTypeInfos.push_back(callTypeInfo);
                }
            }
            else if (callTypeInfo->isPointer())
            {
                const auto callPointer = castTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
                wantedTypeInfos.push_back(wantedPointer->pointedType);
                callTypeInfos.push_back(callPointer->pointedType);
            }
            else
            {
                wantedTypeInfos.push_back(wantedPointer->pointedType);
                callTypeInfos.push_back(callTypeInfo);
            }
            break;
        }

        case TypeInfoKind::Array:
        {
            const auto wantedArray = castTypeInfo<TypeInfoArray>(wantedTypeInfo, TypeInfoKind::Array);

            uint32_t count = 0;
            if (callTypeInfo->isArray())
            {
                const auto callArray = castTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                wantedTypeInfos.push_back(wantedArray->finalType);
                callTypeInfos.push_back(callArray->finalType);
                count = callArray->count;
            }
            else if (callTypeInfo->isListArray())
            {
                const auto callList = castTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                wantedTypeInfos.push_back(wantedArray->finalType);
                callTypeInfos.push_back(callList->subTypes[0]->typeInfo);
                count = callList->subTypes.count;
            }
            else
            {
                SWAG_ASSERT(false);
            }

            // Array dimension was a generic symbol. Set the corresponding symbol in order to check its value
            if (wantedArray->isGeneric() && wantedArray->hasFlag(TYPEINFO_GENERIC_COUNT))
            {
                SWAG_ASSERT(wantedArray->sizeNode);
                SWAG_ASSERT(wantedArray->sizeNode->resolvedSymbolName());
                const auto typeSize = wantedArray->sizeNode->typeInfo;

                ComputedValue* cv = Allocator::alloc<ComputedValue>();
                cv->reg.s64       = count;

                // Constant already defined ?
                const auto& cstName = wantedArray->sizeNode->resolvedSymbolName()->name;
                const auto  it      = context.genericReplaceValues.find(cstName);
                if (it != context.genericReplaceValues.end())
                {
                    if (!Semantic::valueEqualsTo(it->second, cv, typeSize, 0))
                    {
                        context.badSignatureInfos.badNode               = callParameter;
                        context.badSignatureInfos.badGenMatch           = cstName;
                        context.badSignatureInfos.badGenValue1          = it->second;
                        context.badSignatureInfos.badGenValue2          = cv;
                        context.badSignatureInfos.badSignatureGivenType = typeSize;
                        context.result                                  = MatchResult::MismatchGenericValue;
                        break;
                    }

                    Allocator::free<ComputedValue>(cv);
                }
                else
                {
                    context.genericReplaceValues[cstName] = cv;
                }

                GenericReplaceType st;
                st.typeInfoGeneric                          = typeSize;
                st.typeInfoReplace                          = typeSize;
                st.fromNode                                 = wantedArray->sizeNode;
                context.genericReplaceTypes[typeSize->name] = st;
            }

            break;
        }

        case TypeInfoKind::Slice:
        {
            const auto wantedSlice = castTypeInfo<TypeInfoSlice>(wantedTypeInfo, TypeInfoKind::Slice);
            if (callTypeInfo->isSlice())
            {
                const auto callSlice = castTypeInfo<TypeInfoSlice>(callTypeInfo, TypeInfoKind::Slice);
                wantedTypeInfos.push_back(wantedSlice->pointedType);
                callTypeInfos.push_back(callSlice->pointedType);
            }
            else if (callTypeInfo->isArray())
            {
                const auto callArray = castTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
                wantedTypeInfos.push_back(wantedSlice->pointedType);
                callTypeInfos.push_back(callArray->pointedType);
            }
            else if (callTypeInfo->isListArray())
            {
                const auto callList = castTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
                wantedTypeInfos.push_back(wantedSlice->pointedType);
                callTypeInfos.push_back(callList->subTypes[0]->typeInfo);
            }
            else
            {
                wantedTypeInfos.push_back(wantedSlice->pointedType);
                callTypeInfos.push_back(callTypeInfo);
            }
            break;
        }

        case TypeInfoKind::LambdaClosure:
        {
            const auto wantedLambda = castTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::LambdaClosure);
            const auto callLambda   = castTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::LambdaClosure);

            if (wantedLambda->returnType && wantedLambda->returnType->isGeneric())
            {
                if (!callLambda->returnType->isUndefined() && !callLambda->returnType->isGeneric())
                {
                    wantedTypeInfos.push_back(wantedLambda->returnType);
                    callTypeInfos.push_back(callLambda->returnType);
                }
                else
                {
                    const AstFuncDecl* decl = castAst<AstFuncDecl>(callLambda->declNode, AstNodeKind::FuncDecl);
                    if (decl->pendingLambdaJob && decl->resolvedSymbolOverload()->hasFlag(OVERLOAD_UNDEFINED))
                    {
                        const auto tt = replaceGenericTypes(context.genericReplaceTypes, wantedLambda);
                        if (tt != wantedLambda)
                            Semantic::resolvePendingLambdaTyping(context.semContext, callLambda->declNode, tt);
                        return;
                    }

                    SWAG_ASSERT(!callLambda->returnType->isGeneric());
                }
            }

            const auto num = wantedLambda->parameters.size();
            for (uint32_t idx = 0; idx < num; idx++)
            {
                if (wantedLambda->isClosure() && !idx)
                    continue;

                TypeInfoParam* callParam;
                if (wantedLambda->isClosure() && callLambda->isLambda())
                    callParam = callLambda->parameters[idx - 1];
                else
                    callParam = callLambda->parameters[idx];

                const TypeInfoParam* wantedParam = wantedLambda->parameters[idx];
                if (wantedParam->typeInfo->isGeneric() && !callParam->typeInfo->isUndefined())
                {
                    wantedTypeInfos.push_back(wantedParam->typeInfo);
                    callTypeInfos.push_back(callParam->typeInfo);
                }
            }
            break;
        }
    }
}

void Generic::deduceType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo* callTypeInfo, AstNode* callParameter, uint32_t idxParam, CastFlags castFlags)
{
    // Do we already have mapped the generic parameter to something ?
    const auto it = context.genericReplaceTypes.find(wantedTypeInfo->name);
    if (it != context.genericReplaceTypes.end())
    {
        // If the deduced type was untyped, then restore it to untyped. That way we will replace it
        // with a real type if we can (i.e. if the match is ok).
        auto typeReplace = it->second.typeInfoReplace;
        if (typeReplace == g_TypeMgr->typeInfoS32Promoted)
            typeReplace = g_TypeMgr->typeInfoUntypedInt;
        else if (typeReplace == g_TypeMgr->typeInfoF32Promoted)
            typeReplace = g_TypeMgr->typeInfoUntypedFloat;
        else if (typeReplace == g_TypeMgr->typeInfoU32Promoted)
            typeReplace = g_TypeMgr->typeInfoUntypedInt;

        // We must be sure that the registered type is the same as the new one
        const bool same = TypeManager::makeCompatibles(context.semContext,
                                                       typeReplace,
                                                       callTypeInfo,
                                                       nullptr,
                                                       callParameter,
                                                       castFlags.with(CAST_FLAG_JUST_CHECK | CAST_FLAG_PARAMS | CAST_FLAG_COMMUTATIVE));
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
        else if (it->second.typeInfoReplace != typeReplace)
        {
            it->second.typeInfoReplace = callTypeInfo;
        }

        return;
    }

    bool canReg = true;

    // Do not register type replacement if the concrete type is a pending lambda typing (we do not know
    // yet the type of parameters)
    if (callTypeInfo->declNode && callTypeInfo->declNode->hasSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING))
        canReg = false;
    else if (wantedTypeInfo->isPointer())
        canReg = false;
    else if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
        canReg = wantedTypeInfo->isSame(callTypeInfo, CAST_FLAG_CAST);

    if (!canReg)
        return;

    // We could have match a non const against a const
    // We need to instantiate with the const equivalent, so make the call type const
    if (!callTypeInfo->isConst() && wantedTypeInfo->isConst())
        callTypeInfo = g_TypeMgr->makeConst(callTypeInfo);

    auto regTypeInfo = callTypeInfo;

    // :DupGen
    if (wantedTypeInfo->isStruct() && callTypeInfo->isStruct())
    {
        const auto callStruct   = castTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
        const auto wantedStruct = castTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
        if (callStruct->genericParameters.size() == wantedStruct->genericParameters.size() &&
            !callStruct->genericParameters.empty())
        {
            const auto newStructType = castTypeInfo<TypeInfoStruct>(callStruct->clone());
            for (uint32_t i = 0; i < callStruct->genericParameters.size(); i++)
                newStructType->genericParameters[i]->name = wantedStruct->genericParameters[i]->typeInfo->name;
            regTypeInfo = newStructType;
        }
    }

    // Associate the generic type with that concrete one
    GenericReplaceType st;
    st.typeInfoGeneric                                = wantedTypeInfo;
    st.typeInfoReplace                                = TypeManager::promoteUntyped(regTypeInfo);
    st.fromNode                                       = callParameter;
    context.genericReplaceTypes[wantedTypeInfo->name] = st;

    // If this is a valid generic argument, register it at the correct call position
    const auto itIdx = context.mapGenericTypeToIndex.find(wantedTypeInfo->name);
    if (itIdx != context.mapGenericTypeToIndex.end())
    {
        st.typeInfoGeneric                                = wantedTypeInfo;
        st.typeInfoReplace                                = callTypeInfo;
        st.fromNode                                       = callParameter;
        context.genericParametersCallTypes[itIdx->second] = st;
    }
}

void Generic::deduceGenericTypes(SymbolMatchContext& context,
                                 AstNode*            callParameter,
                                 TypeInfo*           callTypeInfo,
                                 TypeInfo*           wantedTypeInfo,
                                 uint32_t            idxParam,
                                 CastFlags           castFlags)
{
    SWAG_ASSERT(wantedTypeInfo->isGeneric());

    VectorNative<TypeInfo*> wantedTypeInfos;
    VectorNative<TypeInfo*> callTypeInfos;
    wantedTypeInfos.push_back(wantedTypeInfo);
    callTypeInfos.push_back(callTypeInfo);

    while (!wantedTypeInfos.empty())
    {
        // When we have a reference, we match with the real type, as we do not want a generic function/struct to have a
        // reference as a concrete type
        callTypeInfo   = TypeManager::concretePtrRef(callTypeInfos.get_pop_back());
        wantedTypeInfo = wantedTypeInfos.get_pop_back();
        if (!callTypeInfo)
            continue;

        deduceType(context, wantedTypeInfo, callTypeInfo, callParameter, idxParam, castFlags);
        deduceSubType(context, wantedTypeInfo, callTypeInfo, wantedTypeInfos, callTypeInfos, callParameter);
        if (context.semContext->result != ContextResult::Done)
            return;
    }
}
