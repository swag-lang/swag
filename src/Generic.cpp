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
        auto genGen = match.genericParametersCallTypes[i].typeInfoGeneric;
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


void Generic::checkCanInstantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch)
{
    auto symbol = firstMatch.symbolName;

    // Be sure number of overloads has not changed since then
    if (firstMatch.numOverloadsWhenChecked != symbol->overloads.size())
    {
        context->result = ContextResult::NewChilds;
        return;
    }

    if (firstMatch.numOverloadsInitWhenChecked != symbol->cptOverloadsInit)
    {
        context->result = ContextResult::NewChilds;
        return;
    }

    // Cannot instantiate if the type is incomplete
    Semantic::waitForGenericParameters(context, firstMatch);
}

bool Generic::instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct)
{
    auto       node              = context->node;
    auto&      matches           = context->cacheMatches;
    auto       symbol            = firstMatch.symbolName;
    auto       genericParameters = firstMatch.genericParameters;
    ScopedLock lk(symbol->mutex);

    // If we are inside a generic function (not instantiated), then we are done, we
    // cannot instantiate (can occure when evaluating function body of an incomplete short lammbda
    if (node->ownerFct && node->ownerFct->flags & AST_IS_GENERIC)
        return true;

    checkCanInstantiateGenericSymbol(context, firstMatch);
    YIELD();

    if (forStruct)
    {
        // Be sure we have generic parameters if there's an automatic match
        if (!genericParameters && (firstMatch.flags & SymbolMatchContext::MATCH_GENERIC_AUTO))
        {
            SWAG_ASSERT(!firstMatch.genericParametersCallTypes.empty());
            auto identifier               = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
            identifier->genericParameters = Ast::newFuncCallGenParams(node->sourceFile, node);
            genericParameters             = identifier->genericParameters;
            for (int i = 0; i < (int) firstMatch.genericParametersCallTypes.size(); i++)
            {
                const auto& param     = firstMatch.genericParametersCallTypes[i];
                auto        callParam = Ast::newFuncCallParam(node->sourceFile, genericParameters);
                callParam->typeInfo   = param.typeInfoReplace;
                if (param.fromNode)
                    callParam->token = param.fromNode->token;

                if (firstMatch.genericParametersCallValues[i])
                {
                    callParam->allocateComputedValue();
                    *callParam->computedValue = *firstMatch.genericParametersCallValues[i];
                }
            }
        }

        // We can instantiate the struct because it's no more generic, and we have generic parameters to replace
        if (!(node->flags & AST_IS_GENERIC) && genericParameters)
        {
            bool alias = false;
            SWAG_CHECK(Generic::instantiateStruct(context, genericParameters, firstMatch, alias));
            if (alias)
            {
                auto oneMatch            = context->getOneMatch();
                oneMatch->symbolOverload = firstMatch.symbolOverload;
                matches.push_back(oneMatch);
            }
        }

        // The new struct is no more generic without generic parameters.
        // It happens each time we reference a generic struct without generic parameters, like impl 'Array', @typeof(Array), self* Array or even
        // a variable type (in that case we will try later to instantiate it with default generic parameters).
        // So we match a generic struct as a normal match without instantiation (for now).
        else if (!(node->flags & AST_IS_GENERIC))
        {
            auto oneMatch            = context->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;
        }

        // The new struct is still generic and we have generic parameters
        else
        {
            SWAG_ASSERT(genericParameters);
            auto oneMatch            = context->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;

            // :DupGen
            // We generate a new struct with the wanted generic parameters to have those names for replacement.
            auto newStructType = CastTypeInfo<TypeInfoStruct>(firstMatch.symbolOverload->typeInfo, TypeInfoKind::Struct);
            if (newStructType->genericParameters.size() == genericParameters->childs.size() && genericParameters->childs.size())
            {
                auto typeWasForced = firstMatch.symbolOverload->typeInfo->clone();
                newStructType      = CastTypeInfo<TypeInfoStruct>(typeWasForced, TypeInfoKind::Struct);
                for (size_t i = 0; i < genericParameters->childs.size(); i++)
                {
                    newStructType->genericParameters[i]->name     = genericParameters->childs[i]->typeInfo->name;
                    newStructType->genericParameters[i]->typeInfo = genericParameters->childs[i]->typeInfo;
                }

                typeWasForced->forceComputeName();
                oneMatch->typeWasForced = typeWasForced;
            }
        }
    }
    else
    {
        SWAG_CHECK(Generic::instantiateFunction(context, genericParameters, firstMatch));
    }

    return true;
}
