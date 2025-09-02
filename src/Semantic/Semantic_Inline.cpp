#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool Semantic::mustInline(SemanticContext* context, AstIdentifier* identifier, const SymbolOverload* overload, bool& canInline)
{
    const auto funcDecl = castAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    canInline = true;
    if (!mustInline(funcDecl) || isFunctionButNotACall(context, identifier, overload->symbol))
        canInline = false;
    if (identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_NO_INLINE) && !funcDecl->isMixinMacro())
        canInline = false;

    // If we are inside a function call argument, which is meant to be converted to a code block,
    // then we must not inline function calls. The inlining must be done later, at the #inject point.
    if (canInline)
    {
        VectorNative<SymbolOverload*> overloads;
        AstFuncCallParam*             fctCallParam;
        SWAG_CHECK(Semantic::findFuncCallInContext(context, identifier, overloads, &fctCallParam));
        YIELD();
        if (!overloads.empty())
        {
            VectorNative<TypeInfoParam*> result;
            SWAG_CHECK(Semantic::findFuncCallParamInContext(context, fctCallParam, overloads, result));
            YIELD();
            for (const auto it : result)
            {
                if (it->typeInfo->isCode())
                {
                    canInline = false;
                    break;
                }
            }
        }
    }

    return true;
}

bool Semantic::mustInline(const AstFuncDecl* funcDecl)
{
    if (!funcDecl)
        return false;

    if (funcDecl->mustUserInline())
        return true;

    if (!funcDecl->token.sourceFile->module->buildCfg.byteCodeAutoInline)
        return false;
    if (funcDecl->hasAttribute(ATTRIBUTE_NO_INLINE))
        return false;
    if (funcDecl->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN | ATTRIBUTE_COMPILER))
        return false;
    if (!funcDecl->content)
        return false;

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
        return true;

    if (funcDecl->content->is(AstNodeKind::Return))
        return true;
    SharedLock lk(funcDecl->content->mutex);
    if (funcDecl->content->firstChild() && funcDecl->content->firstChild()->is(AstNodeKind::Return))
        return true;

    return false;
}

bool Semantic::resolveInlineBefore(SemanticContext* context)
{
    const auto inlineNode = castAst<AstInline>(context->node, AstNodeKind::Inline);

    ErrorCxtStep expNode;
    expNode.node = inlineNode->parent;
    expNode.type = ErrCxtStepKind::DuringInline;
    context->errCxtSteps.push_back(expNode);

    if (inlineNode->hasSemFlag(SEMFLAG_RESOLVE_INLINED))
        return true;
    inlineNode->addSemFlag(SEMFLAG_RESOLVE_INLINED);

    const auto funcDecl     = inlineNode->func;
    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

    // For a return by copy, need to reserve room on the stack for the return result
    if (typeInfoFunc->returnNeedsStack())
    {
        inlineNode->addAstFlag(AST_TRANSIENT);
        allocateOnStack(inlineNode, funcDecl->returnType->typeInfo);
    }

    inlineNode->scope->startStackSize = inlineNode->ownerScope->startStackSize;

    // If we inline a throwable function, be sure the top level function is informed
    if (typeInfoFunc->hasFlag(TYPEINFO_CAN_THROW))
        inlineNode->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);

    // Register all function parameters as inline symbols
    if (funcDecl->parameters)
    {
        if (inlineNode->ownerFct)
            inlineNode->ownerFct->funcMutex.lock();

        const AstIdentifier* identifier = nullptr;
        if (inlineNode->parent->is(AstNodeKind::Identifier))
            identifier = castAst<AstIdentifier>(inlineNode->parent, AstNodeKind::Identifier);
        for (uint32_t i = 0; i < funcDecl->parameters->childCount(); i++)
        {
            const auto funcParam = funcDecl->parameters->children[i];

            // If the call parameter of the inlined function is constant, then we register it in a specific
            // constant scope, not in the caller (for mixins)/inline scope.
            // This is a separated scope because mixins do not have their own scope, and we must have a
            // different symbol registration for each constant value
            // @InlineUsingParam
            bool              isConstant   = false;
            AstFuncCallParam* orgCallParam = nullptr;
            if (identifier && identifier->callParameters)
            {
                for (const auto& child : identifier->callParameters->children)
                {
                    const auto callParam = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                    if (callParam->indexParam != i)
                        continue;
                    orgCallParam = callParam;
                    if (!callParam->hasFlagComputedValue())
                        continue;
                    AddSymbolTypeInfo toAdd;
                    toAdd.node                = callParam;
                    toAdd.typeInfo            = funcParam->typeInfo;
                    toAdd.kind                = SymbolKind::Variable;
                    toAdd.computedValue       = callParam->computedValue();
                    toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN | OVERLOAD_CONST_VALUE;
                    toAdd.storageOffset       = callParam->computedValue()->storageOffset;
                    toAdd.storageSegment      = callParam->computedValue()->storageSegment;
                    toAdd.aliasName           = funcParam->token.text;
                    const auto overload       = inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                    overload->fromInlineParam = orgCallParam;
                    isConstant                = true;
                    break;
                }
            }

            if (!isConstant)
            {
                SWAG_ASSERT(inlineNode->parametersScope);
                AddSymbolTypeInfo toAdd;
                toAdd.node                = Ast::clone(funcParam, nullptr);
                toAdd.typeInfo            = funcParam->typeInfo;
                toAdd.kind                = SymbolKind::Variable;
                toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN;
                const auto overload       = inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                overload->fromInlineParam = orgCallParam;
            }
        }

        if (inlineNode->ownerFct)
            inlineNode->ownerFct->funcMutex.unlock();
    }

    // Sub declarations in an inline block have access to the 'parametersScope', so we must start resolving
    // them only when we have filled the parameters
    resolveSubDecls(context, inlineNode->ownerFct);

    return true;
}

bool Semantic::resolveInlineAfter(SemanticContext* context)
{
    const auto node = castAst<AstInline>(context->node, AstNodeKind::Inline);
    const auto fct  = node->func;

    // No need to check missing return for inline only, because the function has already been
    // checked as a separated function
    if (fct->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
    {
        if (fct->returnType && !fct->returnType->typeInfo->isVoid())
        {
            if (!node->hasSemFlag(SEMFLAG_SCOPE_HAS_RETURN))
            {
                if (node->hasSemFlag(SEMFLAG_FCT_HAS_RETURN))
                    return context->report({fct->returnType, formErr(Err0475, fct->getDisplayNameC())});
                return context->report({fct->returnType, formErr(Err0472, fct->getDisplayNameC(), fct->returnType->typeInfo->getDisplayNameC())});
            }
        }
    }

    context->errCxtSteps.pop_back();
    return true;
}

bool Semantic::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* node, bool fromSemantic)
{
    if (node->hasAstFlag(AST_INLINED))
        return true;

    if (funcDecl == node->ownerFct) // Recursive
    {
        const Diagnostic err{node, node->token, formErr(Err0501, node->token.cstr(), g_CommandLine.limitInlineLevel)};
        return context->report(err);
    }

    waitFuncDeclFullResolve(context->baseJob, funcDecl);
    YIELD();

    ScopedLock lkF(funcDecl->funcMutex);

    funcDecl->resolvedSymbolName()->addFlag(SYMBOL_USED);
    node->addAstFlag(AST_INLINED);

    CloneContext cloneContext;

    // Be sure this is not recursive
    uint32_t cpt         = 0;
    auto     ownerInline = node->safeOwnerInline();
    while (ownerInline)
    {
        if (ownerInline->func == funcDecl)
        {
            cpt++;
            if (g_CommandLine.limitInlineLevel && cpt > g_CommandLine.limitInlineLevel)
            {
                const Diagnostic err{node, node->token, formErr(Err0501, node->token.cstr(), g_CommandLine.limitInlineLevel)};
                return context->report(err);
            }
        }

        ownerInline = ownerInline->safeOwnerInline();
    }

    // The content will be inline in its separated syntax block
    const auto inlineNode      = Ast::newInline(nullptr, node);
    inlineNode->attributeFlags = funcDecl->attributeFlags;
    inlineNode->func           = funcDecl;
    inlineNode->scope          = node->ownerScope;
    inlineNode->typeInfo       = TypeManager::concreteType(funcDecl->typeInfo);

    if (node->hasOwnerTryCatchAssume())
    {
        inlineNode->allocateExtension(ExtensionKind::Owner);
        inlineNode->extOwner()->ownerTryCatchAssume = node->ownerTryCatchAssume();
    }

    if (funcDecl->hasExtMisc())
    {
        SharedLock lk1(funcDecl->extMisc()->mutexAltScopes);
        if (!funcDecl->extMisc()->alternativeScopes.empty() || !funcDecl->extMisc()->alternativeScopesVars.empty())
        {
            inlineNode->allocateExtension(ExtensionKind::Misc);
            ScopedLock lk(inlineNode->extMisc()->mutexAltScopes);
            inlineNode->extMisc()->alternativeScopes     = funcDecl->extMisc()->alternativeScopes;
            inlineNode->extMisc()->alternativeScopesVars = funcDecl->extMisc()->alternativeScopesVars;
        }
    }

    // Try/Assume
    setEmitTryCatchAssume(inlineNode, inlineNode->func->typeInfo);
    if (inlineNode->hasOwnerTryCatchAssume() &&
        inlineNode->func->typeInfo->hasFlag(TYPEINFO_CAN_THROW))
    {
        // Reset the emitting callback from the modifier if it exists, as the inline block will deal with that
        if (node->hasExtByteCode())
        {
            const auto extByteCode = node->extByteCode();
            if (extByteCode->byteCodeAfterFct == ByteCodeGen::emitTry)
                extByteCode->byteCodeAfterFct = nullptr;
            else if (extByteCode->byteCodeAfterFct == ByteCodeGen::emitTryCatch)
                extByteCode->byteCodeAfterFct = nullptr;
            else if (extByteCode->byteCodeAfterFct == ByteCodeGen::emitCatch)
                extByteCode->byteCodeAfterFct = nullptr;
            else if (extByteCode->byteCodeAfterFct == ByteCodeGen::emitAssume)
                extByteCode->byteCodeAfterFct = nullptr;
        }
    }

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobalOrImpl())
        globalScope = globalScope->parentScope;
    inlineNode->addAlternativeScope(globalScope);

    // We also need to add all alternative scopes (using), in case the function relies on them to
    // be solved
    const AstNode* parentNode = funcDecl;
    while (parentNode)
    {
        if (parentNode->hasExtMisc())
            inlineNode->addAlternativeScopes(parentNode->extMisc());
        parentNode = parentNode->parent;
    }

    // If a function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters.
    // So we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->addAlternativeScope(scope);
        for (const auto child : funcDecl->genericParameters->children)
        {
            const auto symName = scope->symTable.registerSymbolNameNoLock(context, child, SymbolKind::Variable);
            symName->addOverloadNoLock(child, child->typeInfo, &child->resolvedSymbolOverload()->computedValue);
            symName->cptOverloads = 0; // Simulate a done resolution
        }
    }

    // A mixin behave exactly like if it is in the caller scope, so do not create a sub-scope for them
    Scope* newScope = node->ownerScope;
    if (!funcDecl->hasAttribute(ATTRIBUTE_MIXIN))
    {
        uint32_t numScopes = 0;

        {
            SharedLock lk(node->ownerScope->mutex);
            numScopes = node->ownerScope->childrenScopes.size();
        }

        newScope          = Ast::newScope(inlineNode, form("__inline%d", numScopes), ScopeKind::Inline, node->ownerScope);
        inlineNode->scope = newScope;
    }

    // Create a scope that will contain all parameters.
    // Parameters are specific to each inline block, that's why we create it even for mixins
    inlineNode->parametersScope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);

    // Clone the function body
    cloneContext.parent         = inlineNode;
    cloneContext.ownerInline    = inlineNode;
    cloneContext.ownerFct       = node->ownerFct;
    cloneContext.ownerBreakable = node->safeOwnerBreakable();
    cloneContext.parentScope    = newScope;
    cloneContext.forceFlags.add(node->flags.mask(AST_NO_BACKEND));
    cloneContext.forceFlags.add(node->flags.mask(AST_IN_RUN_BLOCK));
    cloneContext.forceFlags.add(node->flags.mask(AST_IN_DEFER));
    cloneContext.removeFlags.add(AST_R_VALUE);
    cloneContext.cloneFlags.add(CLONE_FORCE_OWNER_FCT | CLONE_INLINE);

    // Register all aliases
    if (node->is(AstNodeKind::Identifier))
    {
        // Replace user aliases of the form #alias?
        // Can come from the identifier itself (for 'visit') or from call parameters (for macros/mixins)
        const auto id = castAst<AstIdentifier>(node, AstNodeKind::Identifier);

        int idx = 0;
        if (id->identifierExtension)
        {
            for (const auto& alias : id->identifierExtension->aliasNames)
                cloneContext.replaceNames[form("#alias%d", idx++)] = alias.text;
        }

        idx = 0;
        for (const auto& alias : id->callParameters->aliasNames)
            cloneContext.replaceNames[form("#alias%d", idx++)] = alias.text;

        // Replace user #uniq
        if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_HAS_UNIQ))
        {
            for (uint32_t i = 0; i < 10; i++)
            {
                cloneContext.replaceNames[form("#uniq%u", i)] = form("__uniq%u", g_UniqueID.fetch_add(1));
            }
        }

        for (const auto child : id->callParameters->children)
        {
            const auto param = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            if (!param->resolvedParameter)
                continue;

            // Transmit code type
            if (param->typeInfo->isCode())
            {
                AddSymbolTypeInfo toAdd;
                toAdd.node      = param;
                toAdd.typeInfo  = param->typeInfo;
                toAdd.kind      = SymbolKind::Variable;
                toAdd.aliasName = param->resolvedParameter->name;

                inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
            }
        }
    }

    // Clone!
    const auto newContent = funcDecl->content->clone(cloneContext);

    if (newContent->hasExtByteCode())
    {
        newContent->extByteCode()->byteCodeBeforeFct = nullptr;
        if (funcDecl->hasAttribute(ATTRIBUTE_MIXIN))
            newContent->extByteCode()->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
    }

    if (newContent->is(AstNodeKind::Try) || newContent->is(AstNodeKind::TryCatch) || newContent->is(AstNodeKind::Assume))
    {
        if (funcDecl->hasAttribute(ATTRIBUTE_MIXIN) && newContent->firstChild()->hasExtension())
        {
            const auto front = newContent->firstChild();
            if (front->hasExtByteCode())
                front->extByteCode()->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
        }
    }

    newContent->removeAstFlag(AST_NO_SEMANTIC);

    // Sub declarations in the inline block, like sub functions
    if (!funcDecl->subDecl.empty())
    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::DuringInline, nullptr);

        // Authorize a subfunction to access inline parameters, if possible
        // This will work for compile-time values, otherwise we will have an out-of-stack frame when generating the code
        cloneContext.alternativeScope = inlineNode->parametersScope;

        if (inlineNode->ownerFct)
            SWAG_CHECK(funcDecl->cloneSubDecl(context, cloneContext, funcDecl->content, inlineNode->ownerFct, newContent));
    }

    // Need to reevaluate the identifier (if this is an identifier) because the makeInline can be called
    // for something else (like a loop node, for example 'opCount'). In that case, we let the specific node
    // deal with the (re)evaluation.
    if (node->is(AstNodeKind::Identifier))
    {
        // Do not reevaluate function parameters
        const auto castId = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (castId->callParameters)
            castId->callParameters->addAstFlag(AST_NO_SEMANTIC);

        node->semanticState = AstNodeResolveState::Enter;
        node->bytecodeState = AstNodeResolveState::Enter;
    }

    // Check used aliases
    // Error if an alias has been defined but not 'eaten' by the function
    if (node->is(AstNodeKind::Identifier))
    {
        if (cloneContext.replaceNames.size() != cloneContext.usedReplaceNames.size())
        {
            PushErrCxtStep ec(context, node, ErrCxtStepKind::DuringInline, nullptr);
            const auto     id = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
            for (auto& val : cloneContext.replaceNames | std::views::values)
            {
                auto it = cloneContext.usedReplaceNames.find(val);
                if (it == cloneContext.usedReplaceNames.end())
                {
                    if (id->identifierExtension)
                    {
                        for (auto& alias : id->identifierExtension->aliasNames)
                        {
                            if (alias.text == val)
                            {
                                const Diagnostic err{id, alias, formErr(Err0710, alias.cstr())};
                                return context->report(err);
                            }
                        }
                    }

                    for (auto& alias : id->callParameters->aliasNames)
                    {
                        if (alias.text == val)
                        {
                            const Diagnostic err{id, alias, formErr(Err0710, alias.cstr())};
                            return context->report(err);
                        }
                    }
                }
            }
        }
    }

    if (fromSemantic)
        context->result = ContextResult::NewChildren;
    else
    {
        context->baseJob->setPending(JobWaitKind::MakeInline, nullptr, funcDecl, nullptr);
        const auto job = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, inlineNode, false);
        job->addDependentJob(context->baseJob);
        context->baseJob->jobsToAdd.push_back(job);
    }

    return true;
}

bool Semantic::makeInline(JobContext* context, AstIdentifier* identifier, bool fromSemantic)
{
    const auto funcDecl = castAst<AstFuncDecl>(identifier->resolvedSymbolOverload()->node, AstNodeKind::FuncDecl);
    SWAG_CHECK(makeInline(context, funcDecl, identifier, fromSemantic));
    YIELD();
    identifier->byteCodeFct = ByteCodeGen::emitPassThrough;
    return true;
}
