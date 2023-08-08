#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "DataSegment.h"

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
                    diag.hint = Hnt(Hnt0052);
                else if (errType && errType->isUntypedFloat())
                    diag.hint = Hnt(Hnt0052);
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
        job->waitSymbolNoLock(symbol);

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

void Generic::waitForGenericParameters(SemanticContext* context, OneGenericMatch& match)
{
    for (auto it : match.genericReplaceTypes)
    {
        auto typeInfo = it.second;
        auto declNode = typeInfo->declNode;
        if (!declNode)
            continue;
        if (!declNode->resolvedSymbolOverload)
            continue;
        if (declNode->resolvedSymbolOverload->symbol == match.symbolName)
            continue;
        if (typeInfo->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            if (typeStruct->fromGeneric)
                continue;

            // If a field in a struct has generic parameters with the struct itself, we authorize to wait
            if (context->node->flags & AST_STRUCT_MEMBER && typeInfo == context->node->ownerStructScope->owner->typeInfo)
                continue;
        }

        context->job->waitOverloadCompleted(declNode->resolvedSymbolOverload);
        if (context->result == ContextResult::Pending)
            return;

        SWAG_ASSERT(typeInfo->sizeOf > 0);
    }
}

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool& alias)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report({node, Fmt(Err(Err0039), node->token.ctext())}));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(match.symbolOverload->typeInfo, match.symbolOverload->typeInfo->kind);
    context->job->waitAllStructSpecialMethods(typeStruct);
    if (context->result != ContextResult::Done)
        return true;
    context->job->waitAllStructInterfaces(typeStruct);
    if (context->result != ContextResult::Done)
        return true;

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;

    // Can be a type alias
    // In that case, we need to retrieve the real struct
    auto genericStructType = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, overload->typeInfo->kind);
    auto sourceSymbol      = match.symbolName;
    SWAG_VERIFY(sourceNode->kind == AstNodeKind::StructDecl, context->report({node, Fmt(Err(Err0040), node->token.ctext())}));

    // Make a new type
    auto newType = CastTypeInfo<TypeInfoStruct>(genericStructType->clone(), genericStructType->kind);
    newType->removeGenericFlag();
    newType->fromGeneric = genericStructType;

    // Replace generic types in the struct generic parameters
    auto sourceNodeStruct = CastAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
    SWAG_CHECK(updateGenericParameters(context, true, false, newType->genericParameters, sourceNodeStruct->genericParameters->childs, genericParameters, match));
    newType->forceComputeName();

    // Be sure that after the generic instantiation, we will not have a match.
    // If this is the case, then the corresponding instantiation is used instead of creating a new exact same one.
    // This can happen when deducing generic parameters.
    for (auto& v : sourceSymbol->overloads)
    {
        if (v->typeInfo->name == newType->name)
        {
            TypeInfoStruct* t0 = (TypeInfoStruct*) v->typeInfo;
            TypeInfoStruct* t1 = (TypeInfoStruct*) newType;

            bool same = true;
            for (size_t i = 0; i < t0->genericParameters.size(); i++)
            {
                if (!t0->genericParameters[i]->typeInfo->isSame(t1->genericParameters[i]->typeInfo, CASTFLAG_EXACT))
                {
                    same = false;
                    break;
                }
            }

            if (same)
            {
                alias                = true;
                match.symbolOverload = v;
                return true;
            }
        }
    }

    CloneContext cloneContext;
    cloneContext.replaceTypes     = match.genericReplaceTypes;
    cloneContext.replaceTypesFrom = match.genericReplaceFrom;
    // Add the struct type replacement now, in case the struct has a field to replace
    cloneContext.replaceTypes[overload->typeInfo->name] = newType;

    // :GenericConcreteAlias
    // Make all types concrete in case of simple aliases
    for (auto& p : cloneContext.replaceTypes)
    {
        p.second = TypeManager::concreteType(p.second, CONCRETE_ALIAS);
    }

    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    newType->scope              = structNode->scope;
    newType->declNode           = structNode;
    newType->replaceTypes       = cloneContext.replaceTypes;
    newType->replaceTypesFrom   = cloneContext.replaceTypesFrom;
    structNode->typeInfo        = newType;
    structNode->originalGeneric = sourceNode;

    // Replace generic values in the struct generic parameters
    SWAG_CHECK(updateGenericParameters(context, false, true, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));

    auto structJob = end(context, context->job, sourceSymbol, structNode, true, cloneContext.replaceTypes);

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (auto method : newType->methods)
    {
        if (method->declNode->isSpecialFunctionName())
        {
            auto specFunc = CastAst<AstFuncDecl>(method->declNode, AstNodeKind::FuncDecl);
            if (specFunc != genericStructType->opUserDropFct &&
                specFunc != genericStructType->opUserPostCopyFct &&
                specFunc != genericStructType->opUserPostMoveFct &&
                !specFunc->genericParameters)
            {
                instantiateSpecialFunc(context, structJob, cloneContext, &specFunc);
            }
        }
    }

    // Instantiate generic interfaces
    structNode->allocateExtension(ExtensionKind::Owner);
    cloneContext.ownerStructScope   = structNode->scope;
    newType->cptRemainingInterfaces = (uint32_t) genericStructType->interfaces.size();
    for (auto itf : genericStructType->interfaces)
    {
        auto typeItf = (TypeInfoParam*) itf->clone();
        newType->interfaces.push_back(typeItf);

        SWAG_ASSERT(itf->declNode);
        auto newItf       = itf->declNode->clone(cloneContext);
        typeItf->declNode = newItf;
        newItf->flags |= AST_FROM_GENERIC;
        structNode->extOwner()->nodesToFree.push_back(newItf);

        auto implJob = SemanticJob::newJob(context->job->dependentJob, context->sourceFile, newItf, false);
        structJob->addDependentJob(implJob);
    }

    context->job->jobsToAdd.push_back(structJob);
    return true;
}

void Generic::instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** specialFct)
{
    auto funcNode = *specialFct;
    if (!funcNode)
        return;

    // Clone original node
    AstFuncDecl* newFunc = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    if (newFunc->genericParameters)
    {
        newFunc->flags |= AST_IS_GENERIC;
        newFunc->content->flags |= AST_NO_SEMANTIC;
    }
    else
    {
        newFunc->flags |= AST_FROM_GENERIC;
        newFunc->content->flags &= ~AST_NO_SEMANTIC;
    }

    if (newFunc->validif)
        newFunc->content->flags |= AST_NO_SEMANTIC;

    newFunc->originalGeneric = funcNode;
    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->isGeneric())
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newFunc->typeInfo = newTypeFunc;
    }

    newTypeFunc->forceComputeName();

    ScopedLock lk(newFunc->resolvedSymbolName->mutex);
    auto       newJob = end(context, context->job, newFunc->resolvedSymbolName, newFunc, false, cloneContext.replaceTypes);
    structJob->dependentJobs.add(newJob);
}

bool Generic::instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool validif)
{
    auto node = context->node;

    // We should have types to replace
    bool noReplaceTypes = match.genericReplaceTypes.empty();
    if (noReplaceTypes)
    {
        // If we do not have types to replace, perhaps we will be able to deduce them in case of a
        // function call parameter
        if (!node->findParent(AstNodeKind::FuncCallParam))
        {
            node->resolvedSymbolName     = match.symbolName;
            node->resolvedSymbolOverload = match.symbolOverload;
            node->typeInfo               = match.symbolOverload->typeInfo;

            SWAG_CHECK(instantiateDefaultGenericFunc(context));
            if (context->result != ContextResult::Done)
                return true;
        }
    }

    // If we instantiate with a tuple list (literal), then we must convert that to a proper struct decl and make
    // a reference to it.
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(match.symbolOverload->node->typeInfo, TypeInfoKind::FuncAttr);
    for (auto& v : match.genericReplaceTypes)
    {
        if (v.second->isListTuple())
        {
            auto tpt = CastTypeInfo<TypeInfoList>(v.second, TypeInfoKind::TypeListTuple);
            int  idx = 0;
            for (auto p : match.parameters)
            {
                if (p->typeInfo == tpt)
                {
                    if (match.solvedParameters[idx]->typeInfo->kind != TypeInfoKind::Generic)
                    {
                        PushErrCxtStep ec(context, typeFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
                        auto           typeDest = CastTypeInfo<TypeInfoStruct>(match.solvedParameters[idx]->typeInfo, TypeInfoKind::Struct);
                        SWAG_CHECK(Ast::convertLiteralTupleToStructType(context, typeDest, p));
                        SWAG_ASSERT(context->result != ContextResult::Done);
                        return true;
                    }
                    else
                    {
                        PushErrCxtStep ec(context, typeFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
                        auto           typeDest = TypeManager::convertTypeListToStruct(context, tpt, true);
                        SWAG_CHECK(Ast::convertLiteralTupleToStructType(context, typeDest, p));
                        SWAG_ASSERT(context->result != ContextResult::Done);
                        return true;
                    }
                }

                idx++;
            }
        }
    }

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes     = match.genericReplaceTypes;
    cloneContext.replaceTypesFrom = match.genericReplaceFrom;

    // We replace all types and generic types with undefined for now
    if (noReplaceTypes)
    {
        for (auto p : typeFunc->genericParameters)
        {
            cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr->typeInfoUndefined;
        }

        for (auto p : typeFunc->parameters)
        {
            if (p->typeInfo->isGeneric())
                cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr->typeInfoUndefined;
        }
    }

    // :GenericConcreteAlias
    // Make all types concrete in case of simple aliases
    for (auto& p : cloneContext.replaceTypes)
    {
        p.second = TypeManager::concreteType(p.second, CONCRETE_ALIAS);
    }

    // Clone original node
    auto overload = match.symbolOverload;
    auto funcNode = overload->node;

    // If function has some attributes, we need to clone the attributes too
    auto cloneNode = funcNode;
    while (cloneNode->parent && cloneNode->parent->kind == AstNodeKind::AttrUse)
        cloneNode = cloneNode->parent;
    cloneNode        = cloneNode->clone(cloneContext);
    auto newFuncNode = cloneNode;
    while (newFuncNode->kind == AstNodeKind::AttrUse)
        newFuncNode = newFuncNode->childs.back();

    AstFuncDecl* newFunc = CastAst<AstFuncDecl>(newFuncNode, AstNodeKind::FuncDecl);
    newFunc->flags |= AST_FROM_GENERIC;
    newFunc->originalGeneric = funcNode;

    // If this is for testing a #validif match, we must not evaluate the function content until the
    // #validif has passed
    if (validif)
        newFunc->content->flags |= AST_NO_SEMANTIC;
    else
        newFunc->content->flags &= ~AST_NO_SEMANTIC;

    auto p = funcNode->parent;
    while (p && p->kind == AstNodeKind::AttrUse)
        p = p->parent;
    Ast::addChildBack(p, cloneNode);

    // :ContextCall
    // If we are calling the function in a struct context (struct.func), then add the struct as
    // an alternative scope
    if (context->node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->identifierRef()->resolvedSymbolOverload)
        {
            auto concreteType = TypeManager::concreteType(identifier->identifierRef()->resolvedSymbolOverload->typeInfo);
            if (concreteType->isStruct())
            {
                auto contextualStruct = CastAst<AstStruct>(concreteType->declNode, AstNodeKind::StructDecl);
                newFunc->addAlternativeScope(contextualStruct->scope);
            }
        }
    }

    // If we are in a function, inherit also the scopes from the function.
    // Be carreful that if the call is inside a #macro, we do not want to inherit the function (3550)
    if (node->ownerFct &&
        node->ownerFct->hasExtMisc() &&
        node->ownerFct->extMisc()->alternativeScopes.size() &&
        !node->findParent(AstNodeKind::CompilerMacro))
    {
        newFunc->addAlternativeScopes(node->ownerFct->extMisc()->alternativeScopes);
    }

    // Inherit alternative scopes from the generic function
    if (funcNode->hasExtMisc() &&
        funcNode->extMisc()->alternativeScopes.size())
    {
        newFunc->addAlternativeScopes(funcNode->extMisc()->alternativeScopes);
    }

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->isGeneric() || noReplaceTypes)
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newTypeFunc->declNode         = newFunc;
        newTypeFunc->replaceTypes     = cloneContext.replaceTypes;
        newTypeFunc->replaceTypesFrom = cloneContext.replaceTypesFrom;
        newFunc->typeInfo             = newTypeFunc;
        if (noReplaceTypes)
            newTypeFunc->flags |= TYPEINFO_UNDEFINED;
    }

    // Replace generic types and values in the function generic parameters
    SWAG_CHECK(updateGenericParameters(context, true, true, newTypeFunc->genericParameters, newFunc->genericParameters->childs, genericParameters, match));
    newTypeFunc->forceComputeName();

    auto job = end(context, context->job, match.symbolName, cloneNode, true, cloneContext.replaceTypes);
    context->job->jobsToAdd.push_back(job);

    return true;
}

bool Generic::instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node)
{
    if (node->typeInfo->isStruct() && node->type && node->type->kind == AstNodeKind::TypeExpression)
    {
        auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
            if (nodeStruct->genericParameters)
            {
                auto idRef      = CastAst<AstIdentifierRef>(typeExpr->identifier, AstNodeKind::IdentifierRef);
                auto identifier = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
                if (!identifier->genericParameters)
                {
                    identifier->genericParameters = Ast::newFuncCallGenParams(context->sourceFile, identifier);
                    identifier->genericParameters->flags |= AST_NO_BYTECODE;

                    CloneContext cloneContext;
                    for (auto p : nodeStruct->genericParameters->childs)
                    {
                        auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                        if (!param->assignment)
                        {
                            Diagnostic diag{node->sourceFile, node->type->token, Fmt(Err(Err0721), typeExpr->identifier->resolvedSymbolName->name.c_str())};
                            diag.hint = Hnt(Hnt0056);
                            return context->report(diag, Diagnostic::hereIs(typeExpr->identifier->resolvedSymbolOverload));
                        }

                        auto child          = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->flags &= ~AST_IS_GENERIC;
                    identifier->flags &= ~AST_IS_GENERIC;
                    node->flags &= ~AST_IS_GENERIC;
                    node->type->flags &= ~AST_IS_GENERIC;

                    // Force the reevaluation of the variable and its childs
                    context->result     = ContextResult::NewChilds;
                    node->semanticState = AstNodeResolveState::Enter;
                    Ast::visit(node, [](AstNode* n)
                               { n->semFlags &= ~SEMFLAG_ONCE; });
                    return true;
                }
            }
        }
    }

    return context->report({node, Fmt(Err(Err0312), node->typeInfo->getDisplayNameC())});
}

bool Generic::instantiateDefaultGenericFunc(SemanticContext* context)
{
    auto node = context->node;

    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (!identifier->genericParameters)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            auto nodeFunc = CastAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
            if (nodeFunc->genericParameters)
            {
                identifier->typeInfo          = nullptr;
                identifier->genericParameters = Ast::newFuncCallGenParams(context->sourceFile, identifier);
                identifier->genericParameters->flags |= AST_NO_BYTECODE;
                Ast::removeFromParent(identifier->genericParameters);
                Ast::addChildFront(identifier, identifier->genericParameters);

                CloneContext cloneContext;
                for (auto p : nodeFunc->genericParameters->childs)
                {
                    auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                    if (!param->assignment)
                    {
                        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0715), identifier->resolvedSymbolName->name.c_str())};
                        diag.hint = Hnt(Hnt0056);
                        return context->report(diag, Diagnostic::hereIs(identifier->resolvedSymbolOverload));
                    }

                    auto child          = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                    cloneContext.parent = child;
                    param->assignment->clone(cloneContext);
                }

                identifier->identifierRef()->flags &= ~AST_IS_GENERIC;
                identifier->flags &= ~AST_IS_GENERIC;

                // Force the reevaluation of the identifier and its childs
                context->result     = ContextResult::NewChilds1;
                node->semanticState = AstNodeResolveState::Enter;
                Ast::visit(node, [](AstNode* n)
                           { n->semFlags &= ~SEMFLAG_ONCE; });
                return true;
            }
        }
    }

    // :ContextCall
    // Get the contextual structure call if it exists
    if (context->node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        auto idRef      = identifier->identifierRef();
        if (idRef->resolvedSymbolOverload)
        {
            auto concreteType = TypeManager::concreteType(idRef->resolvedSymbolOverload->typeInfo);
            if (concreteType->isStruct())
            {
                auto contextualNode = idRef->previousResolvedNode;
                if (contextualNode)
                {
                    Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0715), node->token.ctext())};
                    diag.hint = Hnt(Hnt0057);
                    diag.addRange(contextualNode, Hnt(Hnt0056));
                    return context->report(diag);
                }
            }
        }
    }

    return context->report({node->sourceFile, node->token, Fmt(Err(Err0042), node->token.ctext())});
}