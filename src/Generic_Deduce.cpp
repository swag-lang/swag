#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "SemanticJob.h"
#include "Symbol.h"
#include "TypeManager.h"

void Generic::deduceSubType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo*& callTypeInfo, VectorNative<TypeInfo*>& wantedTypeInfos, VectorNative<TypeInfo*>& callTypeInfos, AstNode* callParameter)
{
    switch (wantedTypeInfo->kind)
    {
    case TypeInfoKind::Struct:
    {
        auto wantedStruct = CastTypeInfo<TypeInfoStruct>(wantedTypeInfo, TypeInfoKind::Struct);
        if (callTypeInfo->isStruct())
        {
            auto callStruct = CastTypeInfo<TypeInfoStruct>(callTypeInfo, TypeInfoKind::Struct);
            if (!callStruct->genericParameters.empty())
            {
                auto num = min(wantedStruct->genericParameters.size(), callStruct->genericParameters.size());
                for (size_t idx = 0; idx < num; idx++)
                {
                    wantedTypeInfos.push_back(wantedStruct->genericParameters[idx]->typeInfo);
                    callTypeInfos.push_back(callStruct->genericParameters[idx]->typeInfo);
                }
            }
            else
            {
                auto num = min(wantedStruct->genericParameters.size(), callStruct->deducedGenericParameters.size());
                for (size_t idx = 0; idx < num; idx++)
                {
                    wantedTypeInfos.push_back(wantedStruct->genericParameters[idx]->typeInfo);
                    callTypeInfos.push_back(callStruct->deducedGenericParameters[idx]);
                }
            }
        }
        else if (callTypeInfo->isListTuple())
        {
            auto callList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListTuple);
            auto num      = min(wantedStruct->genericParameters.size(), callList->subTypes.size());
            for (size_t idx = 0; idx < num; idx++)
            {
                // A tuple typelist like {a: 1, b: 2} can have named parameters, which means that the order of
                // fields is irrelevant, as we can write {b: 2, a: 1} too.
                //
                // We have a generic parameter. We search in the struct the field that correspond to that type, in
                // order to get the corresponding field name. Then we will search for the name in the typelist (if
                // specified).
                auto p       = wantedStruct->genericParameters[idx];
                Utf8 nameVar = p->name;
                for (size_t idx1 = 0; idx1 < wantedStruct->fields.size(); idx1++)
                {
                    if (wantedStruct->fields[idx1]->typeInfo->name == wantedStruct->genericParameters[idx]->typeInfo->name)
                    {
                        nameVar = wantedStruct->fields[idx1]->name;
                        break;
                    }
                }

                // Then the corresponding field name is searched in the typelist in case the user has specified one.
                auto p1        = callList->subTypes[idx];
                auto typeField = p1->typeInfo;
                for (size_t j = 0; j < callList->subTypes.size(); j++)
                {
                    if (nameVar == callList->subTypes[j]->name)
                    {
                        typeField = callList->subTypes[j]->typeInfo;
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
        auto wantedPointer = CastTypeInfo<TypeInfoPointer>(wantedTypeInfo, TypeInfoKind::Pointer);
        if (wantedPointer->isPointerTo(TypeInfoKind::Struct) && callTypeInfo->isPointerTo(TypeInfoKind::Struct))
        {
            // Because of using var cast, we can have here *A and *B with a match.
            // But we do not want A and B to match in generic replacement.
            // So we check they are the same.
            auto callPointer = CastTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
            if (wantedPointer->pointedType->isSame(callPointer->pointedType, CASTFLAG_CAST))
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
            if (wantedPointer->pointedType->isSame(callTypeInfo, CASTFLAG_CAST))
            {
                wantedTypeInfos.push_back(wantedPointer->pointedType);
                callTypeInfos.push_back(callTypeInfo);
            }
        }
        else if (callTypeInfo->isPointer())
        {
            auto callPointer = CastTypeInfo<TypeInfoPointer>(callTypeInfo, TypeInfoKind::Pointer);
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
        auto wantedArray = CastTypeInfo<TypeInfoArray>(wantedTypeInfo, TypeInfoKind::Array);

        uint32_t count = 0;
        if (callTypeInfo->isArray())
        {
            auto callArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
            wantedTypeInfos.push_back(wantedArray->finalType);
            callTypeInfos.push_back(callArray->finalType);
            count = callArray->count;
        }
        else if (callTypeInfo->isListArray())
        {
            auto callList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
            wantedTypeInfos.push_back(wantedArray->finalType);
            callTypeInfos.push_back(callList->subTypes[0]->typeInfo);
            count = callList->subTypes.count;
        }
        else
        {
            SWAG_ASSERT(false);
        }

        // Array dimension was a generic symbol. Set the corresponding symbol in order to check its value
        if (wantedArray->isGeneric() && wantedArray->flags & TYPEINFO_GENERIC_COUNT)
        {
            SWAG_ASSERT(wantedArray->sizeNode);
            SWAG_ASSERT(wantedArray->sizeNode->resolvedSymbolName);
            auto typeSize = wantedArray->sizeNode->typeInfo;

            ComputedValue* cv = Allocator::alloc<ComputedValue>();
            cv->reg.s64       = count;

            // Constant already defined ?
            auto& cstName = wantedArray->sizeNode->resolvedSymbolName->name;
            auto  it      = context.genericReplaceValues.find(cstName);
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
        auto wantedSlice = CastTypeInfo<TypeInfoSlice>(wantedTypeInfo, TypeInfoKind::Slice);
        if (callTypeInfo->isSlice())
        {
            auto callSlice = CastTypeInfo<TypeInfoSlice>(callTypeInfo, TypeInfoKind::Slice);
            wantedTypeInfos.push_back(wantedSlice->pointedType);
            callTypeInfos.push_back(callSlice->pointedType);
        }
        else if (callTypeInfo->isArray())
        {
            auto callArray = CastTypeInfo<TypeInfoArray>(callTypeInfo, TypeInfoKind::Array);
            wantedTypeInfos.push_back(wantedSlice->pointedType);
            callTypeInfos.push_back(callArray->pointedType);
        }
        else if (callTypeInfo->isListArray())
        {
            auto callList = CastTypeInfo<TypeInfoList>(callTypeInfo, TypeInfoKind::TypeListArray);
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
        auto wantedLambda = CastTypeInfo<TypeInfoFuncAttr>(wantedTypeInfo, TypeInfoKind::LambdaClosure);
        auto callLambda   = CastTypeInfo<TypeInfoFuncAttr>(callTypeInfo, TypeInfoKind::LambdaClosure);

        if (wantedLambda->returnType &&
            wantedLambda->returnType->isGeneric())
        {
            if (!callLambda->returnType->isUndefined() && !callLambda->returnType->isGeneric())
            {
                wantedTypeInfos.push_back(wantedLambda->returnType);
                callTypeInfos.push_back(callLambda->returnType);
            }
            else
            {
                AstFuncDecl* decl = CastAst<AstFuncDecl>(callLambda->declNode, AstNodeKind::FuncDecl);
                if (decl->pendingLambdaJob && decl->resolvedSymbolOverload->flags & OVERLOAD_UNDEFINED)
                {
                    auto tt = replaceGenericTypes(context.genericReplaceTypes, wantedLambda);
                    Semantic::resolvePendingLambdaTyping(context.semContext, callLambda->declNode, tt, 1);
                    return;
                }
                else
                {
                    SWAG_ASSERT(!callLambda->returnType->isGeneric());
                }
            }
        }

        auto num = wantedLambda->parameters.size();
        for (size_t idx = 0; idx < num; idx++)
        {
            if (wantedLambda->isClosure() && !idx)
                continue;

            TypeInfoParam* callParam;
            if (wantedLambda->isClosure() && callLambda->isLambda())
                callParam = callLambda->parameters[idx - 1];
            else
                callParam = callLambda->parameters[idx];

            TypeInfoParam* wantedParam = wantedLambda->parameters[idx];
            if (wantedParam->typeInfo->isGeneric() && !callParam->typeInfo->isUndefined())
            {
                wantedTypeInfos.push_back(wantedParam->typeInfo);
                callTypeInfos.push_back(callParam->typeInfo);
            }
        }
        break;
    }

    default:
        break;
    }
}

void Generic::deduceType(SymbolMatchContext& context, TypeInfo* wantedTypeInfo, TypeInfo* callTypeInfo, uint64_t castFlags, int idxParam, VectorNative<TypeInfo*>& wantedTypeInfos, VectorNative<TypeInfo*>& callTypeInfos, AstNode* callParameter)
{
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

        return;
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

void Generic::deduceGenericTypes(SymbolMatchContext& context, AstNode* callParameter, TypeInfo* callTypeInfo, TypeInfo* wantedTypeInfo, int idxParam, uint64_t castFlags)
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

        deduceType(context, wantedTypeInfo, callTypeInfo, castFlags, idxParam, wantedTypeInfos, callTypeInfos, callParameter);
        deduceSubType(context, wantedTypeInfo, callTypeInfo, wantedTypeInfos, callTypeInfos, callParameter);
        if (context.semContext->result != ContextResult::Done)
            return;
    }
}
