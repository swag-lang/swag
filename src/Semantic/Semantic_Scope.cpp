#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

void Semantic::addSymbolMatch(VectorNative<OneSymbolMatch>& symbolsMatch, SymbolName* symName, Scope* scope, CollectedScopeFlags altFlags)
{
    for (const auto& ds : symbolsMatch)
    {
        if (ds.symbol == symName)
            return;
    }

    OneSymbolMatch osm;
    osm.symbol   = symName;
    osm.scope    = scope;
    osm.altFlags = altFlags;
    symbolsMatch.push_back(osm);
}

bool Semantic::findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier)
{
    return findIdentifierInScopes(context, context->cacheSymbolsMatch, identifierRef, identifier);
}

bool Semantic::collectAutoScope(SemanticContext* context, VectorNative<CollectedScope>& scopeHierarchy, AstIdentifierRef* identifierRef, const AstIdentifier* identifier)
{
    VectorNative<TypeInfoEnum*>                      typeEnum;
    VectorNative<std::pair<AstNode*, TypeInfoEnum*>> hasEnum;
    VectorNative<SymbolOverload*>                    testedOver;
    SWAG_CHECK(Semantic::findEnumTypeInContext(context, identifierRef, typeEnum, hasEnum, testedOver));
    YIELD();

    // More than one match : ambiguous
    if (typeEnum.size() > 1)
    {
        Diagnostic err{identifierRef, formErr(Err0021, identifier->token.cstr())};
        bool       first = true;
        for (const auto t : hasEnum)
        {
            Utf8 msg;
            if (first)
                msg = formNte(Nte0151, t.second->getDisplayNameC());
            else
                msg = formNte(Nte0150, t.second->getDisplayNameC());
            err.addNote(t.first, msg);
            first = false;
        }

        err.addNote(toNte(Nte0064));
        return context->report(err);
    }

    // One single match : we are done
    if (typeEnum.size() == 1)
    {
        identifierRef->startScope = typeEnum[0]->scope;
        scopeHierarchy.clear();
        addCollectedScopeOnce(scopeHierarchy, typeEnum[0]->scope);
    }

    // No match, we will try 'with'
    else
    {
        const auto withNodeP = identifier->findParent(AstNodeKind::With);
        if (!withNodeP)
        {
            if (!hasEnum.empty())
            {
                Diagnostic err{identifierRef, formErr(Err0675, identifier->token.cstr(), hasEnum[0].second->getDisplayNameC())};
                const auto closest = SemanticError::findClosestMatchesMsg(identifier->token.text, {{hasEnum[0].second->scope, 0}}, IdentifierSearchFor::Whatever);
                if (!closest.empty())
                    err.addNote(closest);
                if (hasEnum[0].first)
                    err.addNote(hasEnum[0].first, Diagnostic::isType(hasEnum[0].first));
                err.addNote(Diagnostic::hereIs(hasEnum[0].second->declNode));
                return context->report(err);
            }

            Diagnostic err{identifierRef, formErr(Err0681, identifier->token.cstr())};

            // Call to a function ?
            if (testedOver.size() == 1)
                err.addNote(Diagnostic::hereIs(testedOver[0]));

            return context->report(err);
        }

        const auto withNode = castAst<AstWith>(withNodeP, AstNodeKind::With);

        // Prepend the 'with' identifier, and reevaluate
        for (uint32_t wi = withNode->id.size() - 1; wi != UINT32_MAX; wi--)
        {
            const auto id = Ast::newIdentifier(identifierRef, withNode->id[wi], nullptr, identifierRef);
            id->addAstFlag(AST_GENERATED);
            id->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_WITH);
            id->allocateIdentifierExtension();
            id->identifierExtension->alternateEnum    = hasEnum.empty() ? nullptr : hasEnum[0].second;
            id->identifierExtension->fromAlternateVar = withNode->firstChild();
            id->inheritTokenLocation(identifierRef->token);
            identifierRef->children.pop_back();
            Ast::addChildFront(identifierRef, id);
            identifierRef->addSpecFlag(AstIdentifierRef::SPEC_FLAG_WITH_SCOPE);
            context->baseJob->nodes.push_back(id);
        }

        context->result = ContextResult::NewChildren;
    }

    return true;
}

bool Semantic::collectScopeHierarchy(SemanticContext*                 context,
                                     VectorNative<CollectedScope>&    scopeHierarchy,
                                     VectorNative<CollectedScopeVar>& scopeHierarchyVars,
                                     AstIdentifierRef*                identifierRef,
                                     AstIdentifier*                   identifier)
{
    // :AutoScope
    // Auto scoping depending on the context
    // We scan the parent hierarchy for an already defined type that can be used for scoping
    if (identifierRef->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE) && !identifierRef->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_WITH_SCOPE))
    {
        SWAG_CHECK(collectAutoScope(context, scopeHierarchy, identifierRef, identifier));
        YIELD();
    }
    else
    {
        const auto scopeUpMode  = identifier->identifierExtension ? identifier->identifierExtension->scopeUpMode : IdentifierScopeUpMode::None;
        auto       scopeUpValue = identifier->identifierExtension ? identifier->identifierExtension->scopeUpValue : TokenParse{};
        SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, identifier, COLLECT_ALL, scopeUpMode, &scopeUpValue));
    }

    return true;
}

bool Semantic::collectScopeHierarchy(Scope*                           startScope,
                                     VectorNative<CollectedScope>&    scopeHierarchy,
                                     VectorNative<CollectedScopeVar>& scopeHierarchyVars,
                                     VectorNative<OneSymbolMatch>&    symbolsMatch,
                                     const AstIdentifierRef*          identifierRef,
                                     const AstIdentifier*             identifier,
                                     uint32_t                         identifierCrc)
{
    addCollectedScopeOnce(scopeHierarchy, startScope);

    // No need to go further if we have found the symbol in the parent identifierRef scope (if specified).
    const auto symbol = startScope->symTable.find(identifier->token.text, identifierCrc);
    if (symbol)
    {
        addSymbolMatch(symbolsMatch, symbol, startScope, 0);
        return true;
    }

    // Only deal with previous scope if the previous node wants to
    bool addAlternative = true;
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->hasSemFlag(SEMFLAG_FORCE_SCOPE))
        addAlternative = false;

    if (addAlternative)
    {
        // A namespace scope can in fact be shared between multiple nodes, so the 'owner' is not
        // relevant and we cannot use it
        if (startScope->isNot(ScopeKind::Namespace) && startScope->owner->hasExtension())
        {
            collectAlternativeScopes(startScope->owner, scopeHierarchy);
            collectAlternativeScopeVars(startScope->owner, scopeHierarchy, scopeHierarchyVars);
        }
    }

    return true;
}

void Semantic::findSymbolsInHierarchy(VectorNative<CollectedScope>& scopeHierarchy, VectorNative<OneSymbolMatch>& symbolsMatch, const AstIdentifier* identifier, uint32_t identifierCrc)
{
    while (!scopeHierarchy.empty())
    {
        for (uint32_t i = 0; i < scopeHierarchy.size(); i++)
        {
            const auto& as = scopeHierarchy[i];
            if (as.scope)
            {
                if (!as.scope->symTable.tryRead())
                    continue;
                const auto symbol = as.scope->symTable.findNoLock(identifier->token.text, identifierCrc);
                as.scope->symTable.endRead();
                if (symbol)
                    addSymbolMatch(symbolsMatch, symbol, as.scope, as.flags);
            }

            scopeHierarchy.erase_unordered(i);
            i--;
        }
    }
}

bool Semantic::findIdentifierInScopes(SemanticContext* context, VectorNative<OneSymbolMatch>& symbolsMatch, AstIdentifierRef* identifierRef, AstIdentifier* identifier)
{
    // When this is "retval" type, no need to do fancy things, we take the corresponding function
    // return symbol. This will avoid some ambiguous resolutions with multiple tuples/structs.
    if (identifier->token.is(g_LangSpec->name_retval))
    {
        SWAG_CHECK(matchRetval(context, symbolsMatch, identifier));
        return true;
    }

    // #self
    if (identifier->token.is(g_LangSpec->name_sharp_self))
    {
        SWAG_CHECK(matchSharpSelf(context, symbolsMatch, identifierRef, identifier));
        return true;
    }

    const auto job                = context->baseJob;
    auto&      scopeHierarchy     = context->cacheScopeHierarchy;
    auto&      scopeHierarchyVars = context->cacheScopeHierarchyVars;
    const auto identifierCrc      = identifier->token.text.hash();

    // First, we search in the specified scope, if defined
    const auto startScope = identifierRef->startScope;
    if (startScope)
    {
        for (uint32_t i = 0; i < 2; i++)
        {
            SWAG_CHECK(collectScopeHierarchy(startScope, scopeHierarchy, scopeHierarchyVars, symbolsMatch, identifierRef, identifier, identifierCrc));
            if (!symbolsMatch.empty())
                return true;

            findSymbolsInHierarchy(scopeHierarchy, symbolsMatch, identifier, identifierCrc);
            if (!symbolsMatch.empty())
                return true;

            // It can happen than types are not completed when collecting hierarchies, so we can miss some scopes (because of using).
            // If the types are completed between the collect and the waitTypeCompleted, then we won't find the missing hierarchies,
            // because we won't parse again.
            // So here, if we do not find symbols, then we restart until all types are completed.
            // :TryAgainOnce
            waitTypeCompleted(job, startScope->owner->typeInfo);
            YIELD();
        }

        if (!identifierRef->previousResolvedNode && identifierRef->hasAstFlag(AST_SILENT_CHECK))
            return true;
        if (!identifierRef->previousResolvedNode)
            return SemanticError::unknownIdentifierError(context, identifierRef, identifier);

        identifier->addSemFlag(SEMFLAG_FORCE_UFCS);
    }

    // We then search in the normal hierarchy
    // We need this because even if A.B does not resolve (B is not in A), B(A) can be a match because of UFCS
    for (uint32_t i = 0; i < 2; i++)
    {
        SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, identifierRef, identifier));
        YIELD();

        for (const auto& sv : scopeHierarchyVars)
        {
            if (sv.scope->owner->typeInfo &&
                sv.scope->owner->typeInfo->isStruct() &&
                sv.scope->owner->hasSpecFlag(AstStruct::SPEC_FLAG_HAS_USING))
            {
                waitTypeCompleted(job, sv.scope->owner->typeInfo);
                YIELD();
            }
        }

        findSymbolsInHierarchy(scopeHierarchy, symbolsMatch, identifier, identifierCrc);
        if (!symbolsMatch.empty())
            return true;

        // :TryAgainOnce
        for (const auto& sv : scopeHierarchy)
        {
            waitTypeCompleted(job, sv.scope->owner->typeInfo);
            YIELD();
        }

        for (const auto& sv : scopeHierarchyVars)
        {
            waitTypeCompleted(job, sv.scope->owner->typeInfo);
            YIELD();
        }
    }

    // Error, no symbol !
    if (identifierRef->hasAstFlag(AST_SILENT_CHECK))
        return true;
    return SemanticError::unknownIdentifierError(context, identifierRef, identifier);
}

void Semantic::collectAlternativeScopes(const AstNode* startNode, VectorNative<CollectedScope>& scopes)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<CollectedScope> toAdd;
    VectorNative<Scope*>         done;

    if (startNode->hasExtMisc())
    {
        SharedLock lk(startNode->extMisc()->mutexAltScopes);
        toAdd.append(startNode->extMisc()->alternativeScopes);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addCollectedScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->is(ScopeKind::Struct))
            {
                SharedLock lk(it0.scope->owner->mutex);
                if (it0.scope->owner->hasExtMisc())
                {
                    // We register the sub scope with the original "node" (top level), because the original node will in the end
                    // become the dependentVar node, and will be converted by cast to the correct type.
                    {
                        SharedLock lk1(it0.scope->owner->extMisc()->mutexAltScopes);
                        for (const auto& it1 : it0.scope->owner->extMisc()->alternativeScopes)
                            toAdd.push_back({it1.scope, it1.flags});
                    }
                }
            }
        }
    }
}

void Semantic::collectAlternativeScopeVars(const AstNode* startNode, VectorNative<CollectedScope>& scopes, VectorNative<CollectedScopeVar>& scopesVars)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<CollectedScopeVar> toAdd;
    VectorNative<Scope*>            done;

    if (startNode->hasExtMisc())
    {
        SharedLock lk(startNode->extMisc()->mutexAltScopes);
        toAdd.append(startNode->extMisc()->alternativeScopesVars);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();
        scopesVars.push_back(it0);

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addCollectedScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->is(ScopeKind::Struct))
            {               
                SharedLock lk(it0.scope->owner->mutex);
                if (it0.scope->owner->hasExtMisc())
                {
                    // We register the sub scope with the original "node" (top level), because the original node will in the end
                    // become the dependentVar node, and will be converted by cast to the correct type.
                    {
                        SharedLock lk1(it0.scope->owner->extMisc()->mutexAltScopes);
                        for (const auto& it1 : it0.scope->owner->extMisc()->alternativeScopesVars)
                            toAdd.push_back({it0.node, it1.node, it1.scope, it1.flags});
                    }

                    // If this is a struct that comes from a generic, we need to also register the generic scope in order
                    // to be able to find generic functions to instantiate
                    SWAG_ASSERT(it0.scope->owner->typeInfo->isStruct());
                    const auto typeStruct = castTypeInfo<TypeInfoStruct>(it0.scope->owner->typeInfo, TypeInfoKind::Struct);
                    if (typeStruct->fromGeneric)
                    {
                        const auto structDecl = castAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
                        addCollectedScopeOnce(scopes, structDecl->scope, 0);
                    }
                }
            }
        }
    }
}

void Semantic::addCollectedScopeOnce(VectorNative<CollectedScope>& scopes, Scope* scope, CollectedScopeFlags flags)
{
    if (hasCollectedScope(scopes, scope))
        return;
    addCollectedScope(scopes, scope, flags);
}

bool Semantic::hasCollectedScope(VectorNative<CollectedScope>& scopes, const Scope* scope)
{
    for (const auto& as : scopes)
    {
        if (as.scope == scope)
            return true;
    }

    return false;
}

void Semantic::addCollectedScope(VectorNative<CollectedScope>& scopes, Scope* scope, CollectedScopeFlags flags)
{
    if (!scope)
        return;

    SWAG_ASSERT(!hasCollectedScope(scopes, scope));

    CollectedScope as;
    as.scope = scope;
    as.flags = flags;
    if (scope->flags.has(SCOPE_FILE_PRIVATE))
        as.flags.add(COLLECTED_SCOPE_FILE_PRIVATE);
    scopes.push_back(as);
}

void Semantic::collectAlternativeScopeHierarchy(SemanticContext*                 context,
                                                VectorNative<CollectedScope>&    scopes,
                                                VectorNative<CollectedScopeVar>& scopesVars,
                                                AstNode*                         startNode,
                                                CollectFlags                     flags,
                                                IdentifierScopeUpMode            scopeUpMode,
                                                TokenParse*                      scopeUpValue)
{
    // Add registered alternative scopes of the node of the owner scope
    if (startNode->ownerScope && startNode->ownerScope->owner)
    {
        const auto owner = startNode->ownerScope->owner;
        if (owner->hasExtMisc())
        {
            SharedLock lk(owner->extMisc()->mutexAltScopes);
            auto&      toProcess = context->scopesToProcess;
            for (const auto& as : owner->extMisc()->alternativeScopes)
            {
                if (!hasCollectedScope(scopes, as.scope) && as.flags.has(COLLECTED_SCOPE_USING))
                {
                    addCollectedScope(scopes, as.scope, as.flags);
                    addCollectedScopeOnce(toProcess, as.scope, as.flags);
                }
            }
        }
    }

    // Add registered alternative scopes of the current node
    if (startNode->hasExtMisc())
    {
        SharedLock lk(startNode->extMisc()->mutexAltScopes);
        auto&      toProcess = context->scopesToProcess;
        for (const auto& as : startNode->extMisc()->alternativeScopes)
        {
            if (!hasCollectedScope(scopes, as.scope))
            {
                addCollectedScope(scopes, as.scope, as.flags);
                addCollectedScopeOnce(toProcess, as.scope, as.flags);
            }
        }
    }

    if (startNode->hasExtMisc())
    {
        // Can only take a using var if in the same function
        if (startNode->ownerFct == context->node->ownerFct || startNode == context->node->ownerFct)
        {
            collectAlternativeScopeVars(startNode, scopes, scopesVars);
        }
    }

    // An inline block contains a specific scope that contains the parameters.
    // That scope does not have a parent, so the hierarchy scan will stop at it.
    if (startNode->is(AstNodeKind::Inline) && !flags.has(COLLECT_NO_INLINE_PARAMS) && !context->forDebugger)
    {
        const auto inlineNode = castAst<AstInline>(startNode, AstNodeKind::Inline);
        SWAG_ASSERT(inlineNode->parametersScope);
        addCollectedScopeOnce(scopes, inlineNode->parametersScope);
    }

    if (startNode->is(AstNodeKind::CompilerMacro))
    {
        if (scopeUpMode == IdentifierScopeUpMode::None)
        {
            while (startNode->parent->isNot(AstNodeKind::Inline) && // Need to test on parent to be able to add alternative scopes of the inline block
                   startNode->isNot(AstNodeKind::FuncDecl))
            {
                startNode = startNode->parent;
            }

            flags.add(COLLECT_NO_INLINE_PARAMS);
        }

        scopeUpMode = IdentifierScopeUpMode::None;

        // Macro in a sub function, stop there
        if (startNode->is(AstNodeKind::FuncDecl))
            return;
    }

    // If we are in an inline block, jump right to the function parent
    // Not that the function parent can be null in case of inlined expression in a global for example (compile-time execution)
    else if (startNode->is(AstNodeKind::Inline) && !context->forDebugger)
    {
        const auto inlineBlock = castAst<AstInline>(startNode, AstNodeKind::Inline);
        if (!inlineBlock->func->hasAttribute(ATTRIBUTE_MIXIN))
        {
            if (!inlineBlock->func->hasAttribute(ATTRIBUTE_MACRO))
                return;

            if (scopeUpMode == IdentifierScopeUpMode::None)
            {
                while (startNode && startNode->isNot(AstNodeKind::FuncDecl))
                    startNode = startNode->parent;
            }

            scopeUpMode = IdentifierScopeUpMode::None;
        }
    }

    if (!startNode)
        return;

    const auto alternativeNode = startNode->extraPointer<AstNode>(ExtraPointerKind::AlternativeNode);
    if (alternativeNode)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, alternativeNode, flags, scopeUpMode, scopeUpValue);
    else if (startNode->parent)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->parent, flags, scopeUpMode, scopeUpValue);

    // Mixin block, collect alternative scopes from the original source tree (with the user code, before
    // making the inline)
    if (alternativeNode &&
        startNode->parent->is(AstNodeKind::CompilerInject))
    {
        // We authorize mixin code to access the parameters of the Swag.mixin function, except if there's a #macro block
        // in the way.
        while (startNode->isNot(AstNodeKind::Inline) &&
               startNode->isNot(AstNodeKind::CompilerMacro) &&
               startNode->isNot(AstNodeKind::FuncDecl))
        {
            startNode = startNode->parent;
        }

        if (startNode->is(AstNodeKind::Inline))
        {
            const auto inlineNode = castAst<AstInline>(startNode, AstNodeKind::Inline);
            SWAG_ASSERT(inlineNode->parametersScope);
            addCollectedScopeOnce(scopes, inlineNode->parametersScope);
        }
    }
}

bool Semantic::collectScopeHierarchy(SemanticContext*                 context,
                                     VectorNative<CollectedScope>&    scopes,
                                     VectorNative<CollectedScopeVar>& scopesVars,
                                     AstNode*                         startNode,
                                     CollectFlags                     flags,
                                     IdentifierScopeUpMode            scopeUpMode,
                                     TokenParse*                      scopeUpValue)
{
    auto&      toProcess  = context->scopesToProcess;
    const auto sourceFile = context->sourceFile;

    toProcess.clear();
    scopes.clear();

    // Get alternative scopes from the node hierarchy
    collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode, flags, scopeUpMode, scopeUpValue);

    auto startScope = startNode->ownerScope;
    if (startScope)
    {
        // For a #up, do not collect scope until we find an inline block
        if (scopeUpMode == IdentifierScopeUpMode::Count)
        {
            for (uint64_t i = 0; i < scopeUpValue->literalValue.u8; i++)
            {
                while (startScope && startScope->isNot(ScopeKind::Inline) && startScope->isNot(ScopeKind::Macro))
                    startScope = startScope->parentScope;

                if (!startScope && i == 0)
                {
                    const Diagnostic err{context->node, scopeUpValue->token, toErr(Err0311)};
                    return context->report(err);
                }

                if (!startScope && i)
                {
                    const Diagnostic err{context->node, scopeUpValue->token, formErr(Err0125, scopeUpValue->literalValue.u8)};
                    return context->report(err);
                }

                startScope = startScope->parentScope;
            }

            scopeUpMode = IdentifierScopeUpMode::None;
        }

        addCollectedScopeOnce(scopes, startScope);
        addCollectedScopeOnce(toProcess, startScope);
    }

    // Add bootstrap
    SWAG_ASSERT(g_Workspace->bootstrapModule);
    addCollectedScopeOnce(scopes, g_Workspace->bootstrapModule->scopeRoot);
    addCollectedScopeOnce(toProcess, g_Workspace->bootstrapModule->scopeRoot);

    // Add runtime, except for the bootstrap
    if (!sourceFile->hasFlag(FILE_BOOTSTRAP))
    {
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addCollectedScopeOnce(scopes, g_Workspace->runtimeModule->scopeRoot);
        addCollectedScopeOnce(toProcess, g_Workspace->runtimeModule->scopeRoot);
    }

    for (uint32_t i = 0; i < toProcess.size(); i++)
    {
        const auto& as    = toProcess[i];
        auto        scope = as.scope;
        if (!scope)
            continue;

        // For an inline scope, stop here
        if (scope->is(ScopeKind::Inline) && !context->forDebugger)
        {
            continue;
        }

        // For a macro scope, jump right to the inline
        if (scope->is(ScopeKind::Macro))
        {
            const auto orgScope = scope;
            if (scope->hieScope)
                scope = scope->hieScope;
            else
            {
                while (scope && scope->isNot(ScopeKind::Inline))
                    scope = scope->parentScope;
                if (!scope)
                    continue;
                scope              = scope->parentScope;
                orgScope->hieScope = scope;
            }
        }

        // Add parent scope
        if (scope->parentScope)
        {
            if (scope->parentScope->is(ScopeKind::Struct) && flags.has(COLLECT_NO_STRUCT))
                continue;

            if (!hasCollectedScope(scopes, scope->parentScope))
            {
                addCollectedScope(scopes, scope->parentScope);
                addCollectedScope(toProcess, scope->parentScope);
            }
        }
    }

    SWAG_VERIFY(scopeUpMode == IdentifierScopeUpMode::None, context->report({startNode, toErr(Err0311)}));
    return true;
}
