#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Generic/Generic.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

void Generic::instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** specialFct)
{
    const auto funcNode = *specialFct;
    if (!funcNode)
        return;

    // Clone original node
    AstFuncDecl* newFunc = castAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    if (newFunc->genericParameters)
    {
        newFunc->addAstFlag(AST_GENERIC);
        newFunc->content->addAstFlag(AST_NO_SEMANTIC);
    }
    else
    {
        newFunc->addAstFlag(AST_FROM_GENERIC);
        newFunc->content->removeAstFlag(AST_NO_SEMANTIC);
    }

    if (!newFunc->constraints.empty())
        newFunc->content->addAstFlag(AST_NO_SEMANTIC);

    newFunc->originalGeneric = funcNode;
    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the replaceGenericTypes didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = castTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->isGeneric())
    {
        newTypeFunc = castTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newFunc->typeInfo = newTypeFunc;
    }

    newTypeFunc->forceComputeName();

    const auto symbolName = newFunc->resolvedSymbolName();
    ScopedLock lk(symbolName->mutex);
    const auto newJob = end(context, context->baseJob, symbolName, newFunc, false, cloneContext.replaceTypes);
    structJob->dependentJobs.add(newJob);
}

bool Generic::instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneMatch& match, bool isConstraint)
{
    const auto node = context->node;

    // We should have types to replace
    const bool noReplaceTypes = match.genericReplaceTypes.empty();
    if (noReplaceTypes)
    {
        // If we do not have types to replace, perhaps we will be able to deduce them in case of a
        // function call parameter
        if (!node->findParent(AstNodeKind::FuncCallParam))
        {
            node->setResolvedSymbol(match.symbolName, match.symbolOverload);
            node->typeInfo = match.symbolOverload->typeInfo;

            SWAG_CHECK(instantiateDefaultGenericFunc(context));
            YIELD();
        }
    }

    // If we instantiate with a tuple list (literal), then we must convert that to a proper struct decl and make
    // a reference to it.
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(match.symbolOverload->node->typeInfo, TypeInfoKind::FuncAttr);
    for (const auto& val : match.genericReplaceTypes | std::views::values)
    {
        if (val.typeInfoReplace->isListTuple())
        {
            const auto tpt = castTypeInfo<TypeInfoList>(val.typeInfoReplace, TypeInfoKind::TypeListTuple);
            int        idx = 0;
            for (const auto p : match.parameters)
            {
                if (p->typeInfo == tpt)
                {
                    if (match.solvedParameters[idx]->typeInfo->kind != TypeInfoKind::Generic)
                    {
                        PushErrCxtStep ec(context, typeFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
                        const auto     typeDest = castTypeInfo<TypeInfoStruct>(match.solvedParameters[idx]->typeInfo, TypeInfoKind::Struct);
                        SWAG_CHECK(Ast::convertLiteralTupleToStructType(context, match.solvedParameters[idx]->declNode, typeDest, p));
                        SWAG_ASSERT(context->result != ContextResult::Done);
                        return true;
                    }

                    PushErrCxtStep ec(context, typeFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
                    const auto     typeDest = TypeManager::convertTypeListToStruct(context, tpt);
                    SWAG_CHECK(Ast::convertLiteralTupleToStructType(context, nullptr, typeDest, p));
                    SWAG_ASSERT(context->result != ContextResult::Done);
                    return true;
                }

                idx++;
            }
        }
    }

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes  = match.genericReplaceTypes;
    cloneContext.replaceValues = match.genericReplaceValues;

    // We replace all types and generic types with undefined for now
    if (noReplaceTypes)
    {
        GenericReplaceType st;
        st.typeInfoReplace = g_TypeMgr->typeInfoUndefined;

        for (const auto p : typeFunc->genericParameters)
        {
            cloneContext.replaceTypes[p->typeInfo->name] = st;
        }

        for (const auto p : typeFunc->parameters)
        {
            if (p->typeInfo->isGeneric())
                cloneContext.replaceTypes[p->typeInfo->name] = st;
        }
    }

    // @GenericConcreteAlias
    // Make all types concrete in case of simple aliases
    for (auto& val : cloneContext.replaceTypes | std::views::values)
    {
        val.typeInfoReplace = TypeManager::concreteType(val.typeInfoReplace, CONCRETE_ALIAS);
    }

    // Clone original node
    const auto overload = match.symbolOverload;
    const auto funcNode = overload->node;

    // If function has some attributes, we need to clone the attributes too
    auto cloneNode = funcNode;
    while (cloneNode->parent && cloneNode->parent->is(AstNodeKind::AttrUse))
        cloneNode = cloneNode->parent;

    cloneNode = cloneNode->clone(cloneContext);

    auto newFuncNode = cloneNode;
    while (newFuncNode->is(AstNodeKind::AttrUse))
        newFuncNode = newFuncNode->lastChild();

    AstFuncDecl* newFunc = castAst<AstFuncDecl>(newFuncNode, AstNodeKind::FuncDecl);
    newFunc->addAstFlag(AST_FROM_GENERIC);
    newFunc->originalGeneric  = funcNode;
    newFunc->requestedGeneric = node;

    // If this is for testing a constraint match, we must not evaluate the function content until the
    // where has passed
    if (isConstraint)
        newFunc->content->addAstFlag(AST_NO_SEMANTIC);
    else
        newFunc->content->removeAstFlag(AST_NO_SEMANTIC);

    auto p = funcNode->parent;
    while (p && p->is(AstNodeKind::AttrUse))
        p = p->parent;
    Ast::addChildBack(p, cloneNode);

    // @ContextCall
    // If we are calling the function in a struct context (struct.func), then add the struct as
    // an alternative scope
    if (context->node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->identifierRef()->resolvedSymbolOverload())
        {
            const auto concreteType = TypeManager::concreteType(identifier->identifierRef()->resolvedSymbolOverload()->typeInfo);
            if (concreteType->isStruct())
            {
                const auto contextualStruct = castAst<AstStruct>(concreteType->declNode, AstNodeKind::StructDecl);
                newFunc->addAlternativeScope(contextualStruct->scope, COLLECTED_SCOPE_CONTEXT_CALL);
            }
        }
    }

    // If we are in a function, inherit also the scopes from the function if we are in the same struct.
    if (node->ownerFct &&
        node->ownerFct->hasExtMisc() &&
        node->ownerStructScope &&
        node->ownerStructScope == node->ownerFct->ownerStructScope)
    {
        newFunc->addAlternativeScopes(node->ownerFct->extMisc());
    }

    // Inherit alternative scopes from the generic function
    if (funcNode->hasExtMisc())
        newFunc->addAlternativeScopes(funcNode->extMisc());

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the replaceGenericTypes didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = castTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->isGeneric() || noReplaceTypes)
    {
        newTypeFunc = castTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newTypeFunc->declNode      = newFunc;
        newTypeFunc->replaceTypes  = cloneContext.replaceTypes;
        newTypeFunc->replaceValues = cloneContext.replaceValues;
        newFunc->typeInfo          = newTypeFunc;
        if (noReplaceTypes)
            newTypeFunc->addFlag(TYPEINFO_UNDEFINED);
    }
    else
    {
        newTypeFunc->replaceTypes  = cloneContext.replaceTypes;
        newTypeFunc->replaceValues = cloneContext.replaceValues;
    }

    // Replace generic types and values in the function generic parameters
    SWAG_CHECK(replaceGenericParameters(match, newTypeFunc->genericParameters, newFunc->genericParameters->children, genericParameters, true, true));
    newTypeFunc->forceComputeName();

    const auto job = end(context, context->baseJob, match.symbolName, cloneNode, true, cloneContext.replaceTypes);
    context->baseJob->jobsToAdd.push_back(job);
    return true;
}

bool Generic::instantiateDefaultGenericFunc(SemanticContext* context)
{
    const auto node = context->node;

    if (node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (!identifier->genericParameters)
        {
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            const auto nodeFunc = castAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
            if (nodeFunc->genericParameters)
            {
                identifier->typeInfo          = nullptr;
                identifier->genericParameters = Ast::newFuncCallGenParams(nullptr, identifier);
                identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);
                Ast::removeFromParent(identifier->genericParameters);
                Ast::addChildFront(identifier, identifier->genericParameters);

                CloneContext cloneContext;
                for (const auto p : nodeFunc->genericParameters->children)
                {
                    const auto param = castAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                    if (!param->assignment)
                    {
                        const Diagnostic err{node->token.sourceFile, node->token, formErr(Err0081, identifier->resolvedSymbolName()->name.cstr())};
                        return context->report(err, Diagnostic::hereIs(identifier->resolvedSymbolOverload()));
                    }

                    const auto child    = Ast::newFuncCallParam(nullptr, identifier->genericParameters);
                    cloneContext.parent = child;
                    param->assignment->clone(cloneContext);
                }

                identifier->identifierRef()->removeAstFlag(AST_GENERIC);
                identifier->removeAstFlag(AST_GENERIC);

                // Force the reevaluation of the identifier and its children
                context->result     = ContextResult::NewChildren1;
                node->semanticState = AstNodeResolveState::Enter;
                Ast::visit(node, [](AstNode* n) { n->removeSemFlag(SEMFLAG_ONCE); });
                return true;
            }
        }
    }

    // @ContextCall
    // Get the contextual structure call if it exists
    if (context->node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        const auto idRef      = identifier->identifierRef();
        if (idRef->resolvedSymbolOverload())
        {
            const auto concreteType = TypeManager::concreteType(idRef->resolvedSymbolOverload()->typeInfo);
            if (concreteType->isStruct())
            {
                const auto contextualNode = idRef->previousNode;
                if (contextualNode)
                {
                    const Diagnostic err{node->token.sourceFile, node->token, formErr(Err0081, node->token.cstr())};
                    return context->report(err);
                }
            }
        }
    }

    return context->report({node->token.sourceFile, node->token, formErr(Err0722, node->token.cstr())});
}
