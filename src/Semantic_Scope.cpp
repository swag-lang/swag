#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Scope.h"
#include "SemanticError.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "Workspace.h"

void Semantic::addDependentSymbol(VectorNative<OneSymbolMatch>& symbols, SymbolName* symName, Scope* scope, AltScopeFlags asFlags)
{
    for (const auto& ds : symbols)
    {
        if (ds.symbol == symName)
            return;
    }

    OneSymbolMatch osm;
    osm.symbol  = symName;
    osm.scope   = scope;
    osm.asFlags = asFlags;
    symbols.push_back(osm);
}

bool Semantic::findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    return findIdentifierInScopes(context, context->cacheDependentSymbols, identifierRef, node);
}

bool Semantic::findIdentifierInScopes(SemanticContext* context, VectorNative<OneSymbolMatch>& dependentSymbols, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    const auto job = context->baseJob;

    // When this is "retval" type, no need to do fancy things, we take the corresponding function
    // return symbol. This will avoid some ambiguous resolutions with multiple tuples/structs.
    if (node->token.text == g_LangSpec->name_retval)
    {
        // Be sure this is correct
        SWAG_CHECK(resolveRetVal(context));
        const auto fctDecl = node->hasOwnerInline() ? node->ownerInline()->func : node->ownerFct;
        SWAG_ASSERT(fctDecl);
        const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(fctDecl->typeInfo, TypeInfoKind::FuncAttr);
        SWAG_ASSERT(typeFct->returnType->isStruct());
        addDependentSymbol(dependentSymbols, typeFct->returnType->declNode->resolvedSymbolName(), nullptr, 0);
        return true;
    }

    // #self
    if (node->token.text == g_LangSpec->name_sharpself)
    {
        SWAG_VERIFY(node->ownerFct, context->report({node, Err(Err0447)}));
        AstNode* parent = node;
        while (parent->ownerFct->hasAttribute(ATTRIBUTE_SHARP_FUNC) && parent->ownerFct->parent->ownerFct)
            parent = parent->ownerFct->parent;
        SWAG_VERIFY(parent, context->report({parent, Err(Err0447)}));

        if (parent->ownerScope->kind == ScopeKind::Struct || parent->ownerScope->kind == ScopeKind::Enum)
        {
            parent = parent->ownerScope->owner;
            node->addAstFlag(AST_CAN_MATCH_INCOMPLETE);
        }
        else
        {
            SWAG_VERIFY(parent->ownerFct, context->report({parent, Err(Err0447)}));
            parent = parent->ownerFct;

            // Force scope
            if (!node->callParameters && node != identifierRef->children.back())
            {
                node->addSemFlag(SEMFLAG_FORCE_SCOPE);
                node->typeInfo                      = g_TypeMgr->typeInfoVoid;
                identifierRef->previousResolvedNode = node;
                if (node->hasOwnerInline())
                    identifierRef->startScope = node->ownerInline()->parametersScope;
                else
                    identifierRef->startScope = node->ownerFct->scope;
                node->addAstFlag(AST_NO_BYTECODE);
                return true;
            }
        }

        addDependentSymbol(dependentSymbols, parent->resolvedSymbolName(), nullptr, 0);
        return true;
    }

    auto&      scopeHierarchy     = context->cacheScopeHierarchy;
    auto&      scopeHierarchyVars = context->cacheScopeHierarchyVars;
    const auto crc                = node->token.text.hash();

    bool forceEnd  = false;
    bool checkWait = false;

    // We make 2 tries at max : one try with the previous symbol scope (A.B), and one try with the collected scope
    // hierarchy. We need this because even if A.B does not resolve (B is not in A), B(A) can be a match because of UFCS
    for (int oneTry = 0; oneTry < 2 && !forceEnd; oneTry++)
    {
        const auto startScope = identifierRef->startScope;
        if (!startScope || oneTry == 1)
        {
            constexpr CollectFlags collectFlags = COLLECT_ALL;
            const auto             scopeUpMode  = node->identifierExtension ? node->identifierExtension->scopeUpMode : IdentifierScopeUpMode::None;
            auto                   scopeUpValue = node->identifierExtension ? node->identifierExtension->scopeUpValue : TokenParse{};

            // :AutoScope
            // Auto scoping depending on the context
            // We scan the parent hierarchy for an already defined type that can be used for scoping
            if (identifierRef->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE) && !identifierRef->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_WITH_SCOPE))
            {
                VectorNative<TypeInfoEnum*>                      typeEnum;
                VectorNative<std::pair<AstNode*, TypeInfoEnum*>> hasEnum;
                VectorNative<SymbolOverload*>                    testedOver;
                SWAG_CHECK(findEnumTypeInContext(context, identifierRef, typeEnum, hasEnum, testedOver));
                YIELD();

                // More than one match : ambiguous
                if (typeEnum.size() > 1)
                {
                    const Diagnostic          err{identifierRef, FMT(Err(Err0018), node->token.c_str())};
                    Vector<const Diagnostic*> notes;
                    for (const auto t : typeEnum)
                    {
                        auto msg = FMT(Nte(Nte0197), t->getDisplayNameC());
                        notes.push_back(Diagnostic::note(t->declNode, t->declNode->getTokenName(), msg));
                    }

                    return context->report(err, notes);
                }

                // One single match : we are done
                if (typeEnum.size() == 1)
                {
                    identifierRef->startScope = typeEnum[0]->scope;
                    scopeHierarchy.clear();
                    addAlternativeScopeOnce(scopeHierarchy, typeEnum[0]->scope);
                }

                // No match, we will try 'with'
                else
                {
                    const auto withNodeP = node->findParent(AstNodeKind::With);
                    if (!withNodeP)
                    {
                        if (!hasEnum.empty())
                        {
                            const Diagnostic          err{identifierRef, FMT(Err(Err0708), node->token.c_str(), hasEnum[0].second->getDisplayNameC())};
                            Vector<const Diagnostic*> notes;
                            const auto                closest = SemanticError::findClosestMatchesMsg(node->token.text, {{hasEnum[0].second->scope, 0}}, IdentifierSearchFor::Whatever);
                            if (!closest.empty())
                                notes.push_back(Diagnostic::note(closest));
                            if (hasEnum[0].first)
                                notes.push_back(Diagnostic::note(hasEnum[0].first, Diagnostic::isType(hasEnum[0].first)));
                            notes.push_back(Diagnostic::hereIs(hasEnum[0].second->declNode));
                            return context->report(err, notes);
                        }

                        const Diagnostic err{identifierRef, FMT(Err(Err0718), node->token.c_str())};

                        // Call to a function ?
                        const Diagnostic* note = nullptr;
                        if (testedOver.size() == 1)
                            note = Diagnostic::hereIs(testedOver[0]);
                        return context->report(err, note);
                    }

                    const auto withNode = castAst<AstWith>(withNodeP, AstNodeKind::With);

                    // Prepend the 'with' identifier, and reevaluate
                    for (int wi = static_cast<int>(withNode->id.size()) - 1; wi >= 0; wi--)
                    {
                        const auto id = Ast::newIdentifier(context->sourceFile, withNode->id[wi], identifierRef, identifierRef);
                        id->addAstFlag(AST_GENERATED);
                        id->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_WITH);
                        id->allocateIdentifierExtension();
                        id->identifierExtension->alternateEnum    = hasEnum.empty() ? nullptr : hasEnum[0].second;
                        id->identifierExtension->fromAlternateVar = withNode->children.front();
                        id->inheritTokenLocation(identifierRef->token);
                        identifierRef->children.pop_back();
                        Ast::addChildFront(identifierRef, id);
                        identifierRef->addSpecFlag(AstIdentifierRef::SPEC_FLAG_WITH_SCOPE);
                        context->baseJob->nodes.push_back(id);
                    }

                    context->result = ContextResult::NewChildren;
                    return true;
                }
            }
            else
            {
                SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, collectFlags, scopeUpMode, &scopeUpValue));
            }

            // Be sure this is the last try
            forceEnd = true;
        }
        else
        {
            addAlternativeScopeOnce(scopeHierarchy, startScope);

            // No need to go further if we have found the symbol in the parent identifierRef scope (if specified).
            const auto symbol = startScope->symTable.find(node->token.text, crc);
            if (symbol)
            {
                addDependentSymbol(dependentSymbols, symbol, startScope, 0);
            }
            else
            {
                // Only deal with previous scope if the previous node wants to
                bool addAlternative = true;
                if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->hasSemFlag(SEMFLAG_FORCE_SCOPE))
                    addAlternative = false;

                if (addAlternative)
                {
                    // A namespace scope can in fact be shared between multiple nodes, so the 'owner' is not
                    // relevant and we should not use it
                    if (startScope->kind != ScopeKind::Namespace && startScope->owner->extension)
                    {
                        collectAlternativeScopes(startScope->owner, scopeHierarchy);
                        collectAlternativeScopeVars(startScope->owner, scopeHierarchy, scopeHierarchyVars);
                    }
                }
            }
        }

        if (!dependentSymbols.empty())
            break;

        // Search symbol in all the scopes of the hierarchy
        while (!scopeHierarchy.empty())
        {
            for (size_t i = 0; i < scopeHierarchy.size(); i++)
            {
                const auto& as = scopeHierarchy[i];
                if (!as.scope)
                {
                    scopeHierarchy.erase_unordered(i);
                    i--;
                    continue;
                }

                if (as.scope->symTable.tryRead())
                {
                    const auto symbol = as.scope->symTable.findNoLock(node->token.text, crc);
                    as.scope->symTable.endRead();
                    if (symbol)
                        addDependentSymbol(dependentSymbols, symbol, as.scope, as.flags);
                    scopeHierarchy.erase_unordered(i);
                    i--;
                }
            }
        }

        if (!dependentSymbols.empty())
            break;

        // If can happen than types are not completed when collecting hierarchies, so we can miss some scopes (because of using).
        // If the types are completed between the collect and the waitTypeCompleted, then we won't find the missing hierarchies,
        // because we won't parse again.
        // So here, if we do not find symbols, then we restart until all types are completed.
        if (!checkWait)
        {
            // If we dereference something, be sure the owner has been completed
            if (identifierRef->startScope)
            {
                waitTypeCompleted(job, identifierRef->startScope->owner->typeInfo);
                YIELD();
            }

            // Same if dereference is implied by a using var
            for (const auto& sv : scopeHierarchyVars)
            {
                waitTypeCompleted(job, sv.scope->owner->typeInfo);
                YIELD();
            }

            forceEnd  = false;
            checkWait = true;
            oneTry--;
            continue;
        }

        checkWait = false;

        // We raise an error if we have tried to resolve with the normal scope hierarchy, and not just the scope
        // from the previous symbol
        if (oneTry || forceEnd || !identifierRef->startScope || !identifierRef->previousResolvedNode)
        {
            if (identifierRef->hasAstFlag(AST_SILENT_CHECK))
                return true;
            SemanticError::unknownIdentifierError(context, identifierRef, node);
            return false;
        }

        node->addSemFlag(SEMFLAG_FORCE_UFCS);
    }

    return true;
}

void Semantic::collectAlternativeScopes(const AstNode* startNode, VectorNative<AlternativeScope>& scopes)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<AlternativeScope> toAdd;
    VectorNative<Scope*>           done;

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
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct)
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

void Semantic::collectAlternativeScopeVars(const AstNode* startNode, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<AlternativeScopeVar> toAdd;
    VectorNative<Scope*>              done;

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
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct)
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
                        addAlternativeScopeOnce(scopes, structDecl->scope, 0);
                    }
                }
            }
        }
    }
}

void Semantic::addAlternativeScopeOnce(VectorNative<AlternativeScope>& scopes, Scope* scope, AltScopeFlags flags)
{
    if (hasAlternativeScope(scopes, scope))
        return;
    addAlternativeScope(scopes, scope, flags);
}

bool Semantic::hasAlternativeScope(VectorNative<AlternativeScope>& scopes, const Scope* scope)
{
    for (const auto& as : scopes)
    {
        if (as.scope == scope)
            return true;
    }

    return false;
}

void Semantic::addAlternativeScope(VectorNative<AlternativeScope>& scopes, Scope* scope, AltScopeFlags flags)
{
    if (!scope)
        return;

    AlternativeScope as;
    as.scope = scope;
    as.flags = flags;
    if (scope->flags.has(SCOPE_FILE_PRIVATE))
        as.flags.add(ALT_SCOPE_FILE_PRIVATE);
    scopes.push_back(as);
}

void Semantic::collectAlternativeScopeHierarchy(SemanticContext*                   context,
                                                VectorNative<AlternativeScope>&    scopes,
                                                VectorNative<AlternativeScopeVar>& scopesVars,
                                                AstNode*                           startNode,
                                                CollectFlags                       flags,
                                                IdentifierScopeUpMode              scopeUpMode,
                                                TokenParse*                        scopeUpValue)
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
                if (!hasAlternativeScope(scopes, as.scope) && as.flags.has(ALT_SCOPE_USING))
                {
                    addAlternativeScope(scopes, as.scope, as.flags);
                    addAlternativeScopeOnce(toProcess, as.scope, as.flags);
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
            if (!hasAlternativeScope(scopes, as.scope))
            {
                addAlternativeScope(scopes, as.scope, as.flags);
                addAlternativeScopeOnce(toProcess, as.scope, as.flags);
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
    if (startNode->kind == AstNodeKind::Inline && !flags.has(COLLECT_NO_INLINE_PARAMS))
    {
        const auto inlineNode = castAst<AstInline>(startNode, AstNodeKind::Inline);
        SWAG_ASSERT(inlineNode->parametersScope);
        addAlternativeScopeOnce(scopes, inlineNode->parametersScope);
    }

    if (startNode->kind == AstNodeKind::CompilerMacro)
    {
        if (scopeUpMode == IdentifierScopeUpMode::None)
        {
            while (startNode->parent->kind != AstNodeKind::Inline && // Need to test on parent to be able to add alternative scopes of the inline block
                   startNode->kind != AstNodeKind::FuncDecl)
            {
                startNode = startNode->parent;
            }

            flags.add(COLLECT_NO_INLINE_PARAMS);
        }

        scopeUpMode = IdentifierScopeUpMode::None;

        // Macro in a sub function, stop there
        if (startNode->kind == AstNodeKind::FuncDecl)
            return;
    }

    // If we are in an inline block, jump right to the function parent
    // Not that the function parent can be null in case of inlined expression in a global for example (compile time execution)
    else if (startNode->kind == AstNodeKind::Inline)
    {
        const auto inlineBlock = castAst<AstInline>(startNode, AstNodeKind::Inline);
        if (!inlineBlock->func->hasAttribute(ATTRIBUTE_MIXIN))
        {
            if (!inlineBlock->func->hasAttribute(ATTRIBUTE_MACRO))
                return;

            if (scopeUpMode == IdentifierScopeUpMode::None)
            {
                while (startNode && startNode->kind != AstNodeKind::FuncDecl)
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
        startNode->parent->kind == AstNodeKind::CompilerMixin)
    {
        // We authorize mixin code to access the parameters of the Swag.mixin function, except if there's a #macro block
        // in the way.
        while (startNode->kind != AstNodeKind::Inline &&
               startNode->kind != AstNodeKind::CompilerMacro &&
               startNode->kind != AstNodeKind::FuncDecl)
        {
            startNode = startNode->parent;
        }

        if (startNode->kind == AstNodeKind::Inline)
        {
            const auto inlineNode = castAst<AstInline>(startNode, AstNodeKind::Inline);
            SWAG_ASSERT(inlineNode->parametersScope);
            addAlternativeScopeOnce(scopes, inlineNode->parametersScope);
        }
    }
}

bool Semantic::collectScopeHierarchy(SemanticContext*                   context,
                                     VectorNative<AlternativeScope>&    scopes,
                                     VectorNative<AlternativeScopeVar>& scopesVars,
                                     AstNode*                           startNode,
                                     CollectFlags                       flags,
                                     IdentifierScopeUpMode              scopeUpMode,
                                     TokenParse*                        scopeUpValue)
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
                while (startScope && startScope->kind != ScopeKind::Inline && startScope->kind != ScopeKind::Macro)
                    startScope = startScope->parentScope;

                if (!startScope && i == 0)
                {
                    const Diagnostic err{context->node, *scopeUpValue, Err(Err0449)};
                    return context->report(err);
                }

                if (!startScope && i)
                {
                    const Diagnostic err{context->node, *scopeUpValue, FMT(Err(Err0148), scopeUpValue->literalValue.u8)};
                    return context->report(err);
                }

                startScope = startScope->parentScope;
            }

            scopeUpMode = IdentifierScopeUpMode::None;
        }

        addAlternativeScope(scopes, startScope);
        addAlternativeScope(toProcess, startScope);
    }

    // Add current file scope
    addAlternativeScope(scopes, context->sourceFile->scopeFile);
    addAlternativeScope(toProcess, context->sourceFile->scopeFile);

    // Add bootstrap
    SWAG_ASSERT(g_Workspace->bootstrapModule);
    addAlternativeScope(scopes, g_Workspace->bootstrapModule->scopeRoot);
    addAlternativeScope(toProcess, g_Workspace->bootstrapModule->scopeRoot);

    // Add runtime, except for the bootstrap
    if (!sourceFile->hasFlag(FILE_IS_BOOTSTRAP_FILE))
    {
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addAlternativeScope(scopes, g_Workspace->runtimeModule->scopeRoot);
        addAlternativeScope(toProcess, g_Workspace->runtimeModule->scopeRoot);
    }

    for (size_t i = 0; i < toProcess.size(); i++)
    {
        const auto& as    = toProcess[i];
        auto        scope = as.scope;
        if (!scope)
            continue;

        // For an inline scope, stop here
        if (scope->kind == ScopeKind::Inline)
        {
            continue;
        }

        // For a macro scope, jump right to the inline
        if (scope->kind == ScopeKind::Macro)
        {
            const auto orgScope = scope;
            if (scope->hieScope)
                scope = scope->hieScope;
            else
            {
                while (scope && scope->kind != ScopeKind::Inline)
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
            if (scope->parentScope->kind == ScopeKind::Struct && flags.has(COLLECT_NO_STRUCT))
                continue;

            if (!hasAlternativeScope(scopes, scope->parentScope))
            {
                addAlternativeScope(scopes, scope->parentScope);
                addAlternativeScope(toProcess, scope->parentScope);
            }
        }
    }

    SWAG_VERIFY(scopeUpMode == IdentifierScopeUpMode::None, context->report({startNode, Err(Err0449)}));

    return true;
}
