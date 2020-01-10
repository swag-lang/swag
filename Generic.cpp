#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "ThreadManager.h"

void Generic::end(SemanticContext* context, AstNode* newNode, bool waitSymbol)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  symbol           = *dependentSymbols.begin();

    // Need to wait for the struct to be semantic resolved
    symbol->cptOverloads++;
    symbol->cptOverloadsInit++;
    if (waitSymbol)
        job->waitForSymbolNoLock(symbol);

    // Run semantic on that struct
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(job->dependentJob, sourceFile, newNode, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->expansionNode.append(srcCxt->expansionNode);
    destCxt->expansionNode.push_back(context->node);

    g_ThreadMgr.addJob(newJob);
}

bool Generic::updateGenericParameters(SemanticContext* context, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match)
{
    for (int i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        if (callGenericParameters)
        {
            param->typeInfo = callGenericParameters->childs[i]->typeInfo;
            param->value    = callGenericParameters->childs[i]->computedValue;
        }
        else
        {
            param->typeInfo = match.genericParametersCallTypes[i];
        }

        // We should not instantiate with unresolved types
        auto genGen = match.genericParametersGenTypes[i];
        if (genGen->kind == TypeInfoKind::Generic)
        {
            if (param->typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                auto symbol = match.symbolName;
                return context->report({context->node, format("cannot instantiate generic %s '%s' with an untyped integer, you need to specify a type\n", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())});
            }

            if (param->typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
            {
                auto symbol = match.symbolName;
                return context->report({context->node, format("cannot instantiate generic %s '%s' with an untyped float, you need to specify a type\n", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())});
            }
        }

        auto nodeParam           = nodeGenericParameters[i];
        nodeParam->kind          = AstNodeKind::ConstDecl;
        nodeParam->computedValue = param->value;
        nodeParam->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }

    return true;
}

TypeInfo* Generic::doTypeSubstitution(CloneContext& cloneContext, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
        return typeInfo;

    auto it = cloneContext.replaceTypes.find(typeInfo->name);
    if (it != cloneContext.replaceTypes.end())
        return it->second;

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto newType   = doTypeSubstitution(cloneContext, typeAlias->rawType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = static_cast<TypeInfoAlias*>(typeAlias->clone());
            typeAlias->rawType = newType;
            typeAlias->flags &= ~TYPEINFO_GENERIC;
            typeAlias->computeName();
            return typeAlias;
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto newType     = doTypeSubstitution(cloneContext, typePointer->finalType);
        if (newType != typePointer->finalType)
        {
            typePointer            = static_cast<TypeInfoPointer*>(typePointer->clone());
            typePointer->finalType = newType;
            typePointer->flags &= ~TYPEINFO_GENERIC;
            typePointer->computeName();
            typePointer->pointedType = typePointer->computePointedType();
            return typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto newType   = doTypeSubstitution(cloneContext, typeArray->pointedType);
        if (newType != typeArray->pointedType)
        {
            typeArray              = static_cast<TypeInfoArray*>(typeArray->clone());
            typeArray->pointedType = newType;
            typeArray->flags &= ~TYPEINFO_GENERIC;
            typeArray->computeName();
            return typeArray;
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto newType   = doTypeSubstitution(cloneContext, typeSlice->pointedType);
        if (newType != typeSlice->pointedType)
        {
            typeSlice              = static_cast<TypeInfoSlice*>(typeSlice->clone());
            typeSlice->pointedType = newType;
            typeSlice->flags &= ~TYPEINFO_GENERIC;
            typeSlice->computeName();
            return typeSlice;
        }

        break;
    }

    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        TypeInfoFuncAttr* newLambda  = nullptr;
        auto              typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);

        auto newType = doTypeSubstitution(cloneContext, typeLambda->returnType);
        if (newType != typeLambda->returnType)
        {
            newLambda = static_cast<TypeInfoFuncAttr*>(typeLambda->clone());
            newLambda->flags &= ~TYPEINFO_GENERIC;
            newLambda->returnType = newType;
        }

        auto numParams = typeLambda->parameters.size();
        for (int idx = 0; idx < numParams; idx++)
        {
            auto param = CastTypeInfo<TypeInfoParam>(typeLambda->parameters[idx], TypeInfoKind::Param);
            newType    = doTypeSubstitution(cloneContext, param->typeInfo);
            if (newType != param->typeInfo)
            {
                if (!newLambda)
                {
                    newLambda = static_cast<TypeInfoFuncAttr*>(typeLambda->clone());
                    newLambda->flags &= ~TYPEINFO_GENERIC;
                }

                auto newParam      = static_cast<TypeInfoParam*>(newLambda->parameters[idx]);
                newParam->typeInfo = newType;
            }
        }

        if (newLambda)
        {
            newLambda->computeName();
            return newLambda;
        }

        break;
    }
    }

    return typeInfo;
}

void Generic::instanciateSpecialFunc(SemanticContext* context, CloneContext& cloneContext, TypeInfoStruct* typeStruct, AstFuncDecl** specialFct)
{
    auto funcNode = *specialFct;
    if (!funcNode)
        return;

    auto newFunc = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    newFunc->flags |= AST_FROM_GENERIC;
    newFunc->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    TypeInfoFuncAttr* newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo);
    if (newTypeFunc->flags & TYPEINFO_GENERIC)
    {
        newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo->clone());
        newTypeFunc->flags &= ~TYPEINFO_GENERIC;
        newFunc->typeInfo = newTypeFunc;
    }

    // Replace generic types and values in the function generic parameters
    newTypeFunc->computeName();

    // Run semantic on that struct
    newFunc->resolvedSymbolName->cptOverloads++;
    newFunc->resolvedSymbolName->cptOverloadsInit++;
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(context->job->dependentJob, sourceFile, newFunc, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->expansionNode.append(srcCxt->expansionNode);
    destCxt->expansionNode.push_back(context->node);

    g_ThreadMgr.addJob(newJob);
}

bool Generic::instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool waitSymbol)
{
    CloneContext cloneContext;

    // Types replacements
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    // Make a new type
    auto newType = static_cast<TypeInfoStruct*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->scope       = structNode->scope;
    newType->structNode  = structNode;
    structNode->typeInfo = newType;

    // Replace generic types and values in the struct generic parameters
    SWAG_CHECK(updateGenericParameters(context, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));
    newType->computeName();

    end(context, structNode, waitSymbol);

    cloneContext.replaceTypes[overload->typeInfo->name] = newType;
    instanciateSpecialFunc(context, cloneContext, newType, &newType->opUserDropFct);
    instanciateSpecialFunc(context, cloneContext, newType, &newType->opUserPostCopyFct);
    instanciateSpecialFunc(context, cloneContext, newType, &newType->opUserPostMoveFct);

    return true;
}

bool Generic::instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    CloneContext cloneContext;

    // Types replacements
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto funcNode   = CastAst<AstFuncDecl>(sourceNode->clone(cloneContext), AstNodeKind::FuncDecl);
    funcNode->flags |= AST_FROM_GENERIC;
    funcNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, funcNode);

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newType = static_cast<TypeInfoFuncAttr*>(funcNode->typeInfo);
    if (newType->flags & TYPEINFO_GENERIC)
    {
        newType = static_cast<TypeInfoFuncAttr*>(funcNode->typeInfo->clone());
        newType->flags &= ~TYPEINFO_GENERIC;
        funcNode->typeInfo = newType;
    }

    // Replace generic types and values in the function generic parameters
    SWAG_CHECK(updateGenericParameters(context, newType->genericParameters, funcNode->genericParameters->childs, genericParameters, match));
    newType->computeName();

    end(context, funcNode, true);
    return true;
}

bool Generic::instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node)
{
    if (node->typeInfo->kind == TypeInfoKind::Struct && node->type && node->type->kind == AstNodeKind::TypeExpression)
    {
        auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            auto nodeStruct = CastAst<AstStruct>(typeStruct->structNode, AstNodeKind::StructDecl);
            if (nodeStruct->genericParameters)
            {
                auto idRef      = CastAst<AstIdentifierRef>(typeExpr->identifier, AstNodeKind::IdentifierRef);
                auto identifier = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
                if (!identifier->genericParameters)
                {
                    identifier->genericParameters = Ast::newFuncCallParams(context->sourceFile, identifier);
                    identifier->genericParameters->flags |= AST_NO_BYTECODE;

                    CloneContext cloneContext;
                    for (auto p : nodeStruct->genericParameters->childs)
                    {
                        auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                        if (!param->assignment)
                            return context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())});

                        auto child          = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->flags &= ~AST_IS_GENERIC;
                    identifier->flags &= ~AST_IS_GENERIC;
                    node->flags &= ~AST_IS_GENERIC;
                    node->type->flags &= ~AST_IS_GENERIC;
                    context->result     = ContextResult::NewChilds;
                    node->semanticState = AstNodeResolveState::Enter;
                    return true;
                }
            }
        }
    }

    return context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())});
}