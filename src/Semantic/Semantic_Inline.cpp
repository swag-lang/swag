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
#pragma optimize("", off)

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

bool Semantic::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* node, bool fromSemantic)
{
    if (node->hasAstFlag(AST_INLINED))
        return true;

    waitFuncDeclFullResolve(context->baseJob, funcDecl);
    YIELD();

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

        // Replace user #mix
        if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SPEC_MIXIN))
        {
            for (int i = 0; i < 10; i++)
            {
                cloneContext.replaceNames[form("#mix%d", i)] = form("__mix%d", g_UniqueID.fetch_add(1));
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

    const auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
    const auto returnType = typeFunc->concreteReturnType();

    setupConst(identifier);
    setupIdentifierRef(identifier);
    identifier->byteCodeFct = ByteCodeGen::emitPassThrough;

    if (returnType->isStruct())
        identifier->addSemFlag(SEMFLAG_CONST_ASSIGN_INHERIT | SEMFLAG_CONST_ASSIGN);
    if (typeFunc->returnNeedsStack())
        identifier->addAstFlag(AST_TRANSIENT);
    identifier->addAstFlag(AST_FUNC_INLINE_CALL);

    return true;
}
