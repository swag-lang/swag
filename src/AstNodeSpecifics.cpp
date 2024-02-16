#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

AstNode* AstVarDecl::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstVarDecl>();
    newNode->copyFrom(context, this);
    newNode->publicName  = publicName;
    newNode->attributes  = attributes;
    newNode->assignToken = assignToken;
    if (newNode->attrUse)
        newNode->attrUse->content = newNode;

    newNode->type             = reinterpret_cast<AstTypeExpression*>(findChildRef(type, newNode));
    newNode->assignment       = findChildRef(assignment, newNode);
    newNode->genTypeComesFrom = genTypeComesFrom;

    newNode->typeConstraint = findChildRef(typeConstraint, newNode);
    if (newNode->typeConstraint)
        newNode->typeConstraint->removeAstFlag(AST_NO_SEMANTIC);

    // Is there an alias ?
    const auto it = context.replaceNames.find(newNode->token.text);
    if (it != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(it->second);
        newNode->token.text = it->second;
    }

    return newNode;
}

bool AstVarDecl::isConstDecl() const
{
    if (kind == AstNodeKind::ConstDecl)
        return true;
    if (specFlags & SPECFLAG_IS_LET_TO_CONST)
        return true;
    return false;
}

void AstIdentifierRef::computeName()
{
    token.text.clear();
    for (const auto child : childs)
    {
        if (!token.text.empty())
            token.text += ".";
        token.text += child->token.text;
    }
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstIdentifierRef>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstIdentifier::~AstIdentifier()
{
    if (identifierExtension)
        Allocator::free<IdentifierExtension>(identifierExtension);
}

AstIdentifierRef* AstIdentifier::identifierRef() const
{
    if (parent->kind == AstNodeKind::IdentifierRef)
        return castAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);

    auto check = parent;
    while (check->kind != AstNodeKind::IdentifierRef)
    {
        check = check->parent;
        SWAG_ASSERT(check);
    }

    return castAst<AstIdentifierRef>(check, AstNodeKind::IdentifierRef);
}

void AstIdentifier::allocateIdentifierExtension()
{
    if (identifierExtension)
        return;
    identifierExtension = Allocator::alloc<IdentifierExtension>();
}

AstNode* AstIdentifier::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstIdentifier>();
    newNode->copyFrom(context, this);

    // Is there an alias ?
    const auto itn = context.replaceNames.find(newNode->token.text);
    if (itn != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(itn->second);
        newNode->token.text = itn->second;
    }

    newNode->genericParameters = static_cast<AstFuncCallParams*>(findChildRef(genericParameters, newNode));
    newNode->callParameters    = static_cast<AstFuncCallParams*>(findChildRef(callParameters, newNode));

    if (identifierExtension)
    {
        newNode->allocateIdentifierExtension();
        newNode->identifierExtension->aliasNames   = identifierExtension->aliasNames;
        newNode->identifierExtension->scopeUpMode  = identifierExtension->scopeUpMode;
        newNode->identifierExtension->scopeUpValue = identifierExtension->scopeUpValue;
    }

    // Check if we need to replace the token.text with a type substitution
    // That way the new resolveIdentifier will just try to keep the typeinfo
    const auto it = context.replaceTypes.find(newNode->token.text);
    if (it != context.replaceTypes.end())
    {
        // :ForLocationInValidIf
        if (it->second.fromNode)
        {
            newNode->allocateIdentifierExtension();
            newNode->identifierExtension->fromAlternateVar = it->second.fromNode;
        }

        if (!it->second.typeInfoReplace->isUndefined())
            newNode->token.text = it->second.typeInfoReplace->name;

        newNode->typeInfo = it->second.typeInfoReplace;
        if (newNode->typeInfo->declNode)
        {
            newNode->resolvedSymbolName     = newNode->typeInfo->declNode->resolvedSymbolName;
            newNode->resolvedSymbolOverload = newNode->typeInfo->declNode->resolvedSymbolOverload;
        }

        newNode->addAstFlag(AST_FROM_GENERIC | AST_FROM_GENERIC_REPLACE);
    }

    return newNode;
}

bool AstFuncDecl::mustAutoInline() const
{
    if (!content)
        return false;
    if (sourceFile->module->buildCfg.byteCodeAutoInline == false)
        return false;
    if (hasAttribute(ATTRIBUTE_NO_INLINE))
        return false;

    // All short functions
    if (specFlags & SPECFLAG_SHORT_FORM)
        return true;

    return false;
}

bool AstFuncDecl::mustUserInline(bool forExport) const
{
    if (!content)
        return false;
    if (hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
        return true;
    if (sourceFile->module->buildCfg.byteCodeInline == false && !forExport)
        return false;
    if (hasAttribute(ATTRIBUTE_INLINE))
        return true;
    return false;
}

bool AstFuncDecl::mustInline() const
{
    return mustUserInline() || mustAutoInline();
}

Utf8 AstFuncDecl::getCallName()
{
    if (hasAttribute(ATTRIBUTE_FOREIGN | ATTRIBUTE_PUBLIC))
    {
        if (!hasAttribute(ATTRIBUTE_SHARP_FUNC))
        {
            computeFullNameForeignExport();
            return fullnameForeignExport;
        }
    }

    SWAG_ASSERT(hasExtByteCode() && extByteCode()->bc);
    return extByteCode()->bc->getCallName();
}

Utf8 AstFuncDecl::getNameForUserCompiler()
{
    if (hasAttribute(ATTRIBUTE_SHARP_FUNC))
    {
        auto fct = parent;
        while (fct && (fct->kind != AstNodeKind::FuncDecl || fct->hasAttribute(ATTRIBUTE_SHARP_FUNC)))
            fct = fct->parent;
        if (fct)
            return fct->getScopedName();
    }

    return getScopedName();
}

const char* AstFuncDecl::getDisplayNameC() const
{
    const auto res = getDisplayName();
    return _strdup(res.c_str()); // Leak and slow, but only for messages
}

Utf8 AstFuncDecl::getDisplayName() const
{
    return Naming::funcToName(this);
}

namespace
{
    void normalizeForeignName(Utf8& dest, const Utf8& nameForeign)
    {
        dest = nameForeign;

        const auto len = nameForeign.length();
        auto       pz  = nameForeign.buffer;
        auto       pzd = dest.buffer;
        for (uint32_t i = 0; i < len; i++)
        {
            if (*pz == ' ')
            {
                pz++;
            }
            else if (*pz == ',' || *pz == '\'' || *pz == '-' || *pz == '>' || *pz == '\"')
            {
                *pzd++ = '@';
                pz++;
            }
            else
                *pzd++ = *pz++;
        }

        *pzd++ = 0;

        dest.count = static_cast<uint32_t>(pzd - dest.buffer) - 1;
    }
}

void AstFuncDecl::computeFullNameForeignExport()
{
    ScopedLock lk(mutex);

    if (!fullnameForeignExport.empty())
        return;

    SWAG_ASSERT(ownerScope);
    auto nameForeign = getScopedName();

    // If the symbol has overloads, i.e. more than one definition, then we
    // append the type
    if (resolvedSymbolName && resolvedSymbolName->overloads.size() > 1)
    {
        // Empty (forward) decl are counted as overloads, so be sure it's not a bunch of
        // empty functions.
        uint32_t countNoEmpty = 0;
        for (const auto r : resolvedSymbolName->overloads)
        {
            if (!r->node->isEmptyFct())
            {
                countNoEmpty++;
                if (countNoEmpty > 1)
                    break;
            }
        }

        if (countNoEmpty > 1)
        {
            nameForeign += "@@";
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
            typeFunc->computeScopedName();
            const auto pz = strstr(typeFunc->scopedName.c_str(), "(");
            SWAG_ASSERT(pz);
            nameForeign += pz;
        }
    }

    normalizeForeignName(fullnameForeignExport, nameForeign);
}

const Utf8& AstFuncDecl::getFullNameForeignImport() const
{
    SharedLock lk(mutex);

    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
    const auto value    = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
    if (value && !value->text.empty())
        return value->text;
    return token.text;
}

bool AstFuncDecl::cloneSubDecls(ErrorContext* context, CloneContext& cloneContext, const AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode)
{
    // We need to duplicate sub declarations, and register the symbol in the new corresponding scope
    for (auto f : subDecls)
    {
        ScopedLock lk(f->mutex);

        // A sub declaration node has the root of the file as parent, but has the correct scope. We need to find
        // the duplicated parent node that corresponds to the original one, in order to get the corresponding new
        // scope (if a sub declaration is declared inside an if statement scope for example, we need the duplicated
        // sub declaration to be registered in the corresponding new scope).
        const auto newScopeNode = oldOwnerNode->findChildRefRec(f->ownerScope->owner, refNode);
        SWAG_ASSERT(newScopeNode);
        const auto subFuncScope = newScopeNode->ownerScope;

        cloneContext.parentScope = subFuncScope;
        cloneContext.parent      = nullptr; // Register in parent will be done at the end (race condition)
        if (f->parent->kind == AstNodeKind::AttrUse)
            f = f->parent;
        auto subF         = f->clone(cloneContext);
        auto subDecl      = subF->kind == AstNodeKind::AttrUse ? static_cast<AstAttrUse*>(subF)->content : subF;
        subDecl->typeInfo = subDecl->typeInfo->clone();

        // Be sure symbol is not already there. This can happen when using mixins
        const SymbolName* sym = nullptr;
        if (context)
            sym = subFuncScope->symTable.find(subDecl->token.text);

        auto symKind = SymbolKind::Invalid;
        switch (subDecl->kind)
        {
        case AstNodeKind::FuncDecl:
        {
            const auto nodeFunc = castAst<AstFuncDecl>(subDecl, AstNodeKind::FuncDecl);
            if (sym)
            {
                const Diagnostic diag{nodeFunc, nodeFunc->tokenName, FMT(Err(Err0627), "function", subDecl->token.c_str())};
                return context->report(diag);
            }

            nodeFunc->content->removeAstFlag(AST_NO_SEMANTIC);
            if (cloneContext.alternativeScope)
                nodeFunc->addAlternativeScope(cloneContext.alternativeScope);
            symKind = SymbolKind::Function;

            // Sub decl reference
            if (subDecl->hasAstFlag(AST_SPEC_SEMANTIC_HAS3))
                nodeFunc->addAstFlag(AST_NO_SEMANTIC | AST_SPEC_SEMANTIC3 | AST_SPEC_SEMANTIC_HAS3);

            // Function is supposed to start semantic when captured parameters have been evaluated
            // So no semantic on it now
            if (nodeFunc->captureParameters)
                nodeFunc->addAstFlag(AST_NO_SEMANTIC | AST_SPEC_SEMANTIC1);

            // If function is linked to a makePointerLambda, then we make a unique name for the function, and change the
            // corresponding reference.
            // This way, even if the lambda is used inside a mixin, we won't have any name collisions if the mixin is included multiple times
            // in the same scope.
            // Example 4506.
            if (nodeFunc->makePointerLambda)
            {
                const int id = g_UniqueID.fetch_add(1);
                subDecl->token.text += to_string(id);
                const auto idRef  = castAst<AstIdentifier>(nodeFunc->makePointerLambda->childs.front()->childs.back(), AstNodeKind::Identifier);
                idRef->token.text = subDecl->token.text;
            }

            break;
        }
        case AstNodeKind::StructDecl:
        {
            const auto nodeStruct = castAst<AstStruct>(subDecl, AstNodeKind::StructDecl);
            if (sym)
            {
                const Diagnostic diag{nodeStruct, nodeStruct->tokenName, FMT(Err(Err0627), "struct", subDecl->token.c_str())};
                return context->report(diag);
            }

            symKind               = SymbolKind::Struct;
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(subDecl->typeInfo, TypeInfoKind::Struct);
            typeStruct->scope     = nodeStruct->scope;
            break;
        }
        case AstNodeKind::InterfaceDecl:
            if (sym)
            {
                const Diagnostic diag{subDecl, subDecl->token, FMT(Err(Err0627), "interface", subDecl->token.c_str())};
                return context->report(diag);
            }

            symKind = SymbolKind::Interface;
            break;

        default:
            SWAG_ASSERT(false);
            break;
        }

        subDecl->typeInfo->removeGenericFlag();
        subDecl->typeInfo->declNode = subDecl;

        subDecl->addSemFlag(SEMFLAG_FILE_JOB_PASS);
        newFctNode->subDecls.push_back(subDecl);

        // We need to add the parent scope of the inline function (the global one), in order for
        // the inline content to be resolved in the same context as the original function
        auto globalScope = f->ownerScope;
        while (!globalScope->isGlobalOrImpl())
            globalScope = globalScope->parentScope;
        subDecl->addAlternativeScope(globalScope);

        subDecl->resolvedSymbolName     = subFuncScope->symTable.registerSymbolName(nullptr, subDecl, symKind);
        subDecl->resolvedSymbolOverload = nullptr;

        // Do it last to avoid a race condition with the file job
        Ast::addChildBack(f->parent, subF);
    }

    return true;
}

AstFuncDecl::~AstFuncDecl()
{
    if (scope)
        scope->release();
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    const auto newNode      = Ast::newNode<AstFuncDecl>();
    auto       cloneContext = context;
    cloneContext.forceFlags &= ~AST_SPEC_STACK_SIZE;

    newNode->copyFrom(context, this, false);
    newNode->aliasMask   = aliasMask;
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;
    newNode->tokenName   = tokenName;

    newNode->removeSpecFlag(SPECFLAG_FULL_RESOLVE | SPECFLAG_PARTIAL_RESOLVE | SPECFLAG_SHORT_FORM);

    cloneContext.ownerFct    = newNode;
    cloneContext.parent      = newNode;
    const auto functionScope = Ast::newScope(newNode, newNode->token.text, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = functionScope;

    cloneContext.parentScope = functionScope;

    // Capture parameters are not part of the func hierarchy
    newNode->captureParameters = captureParameters ? captureParameters->clone(cloneContext) : nullptr;
    if (newNode->captureParameters)
        newNode->childs.pop_back();

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->validif           = validif ? validif->clone(cloneContext) : nullptr;
    newNode->nodeCounts        = nodeCounts;
    newNode->makePointerLambda = makePointerLambda;

    // We have a corresponding MakePointerLambda, which has already been duplicated.
    // We then need to make 'makePointerLambda' point to it.
    if (makePointerLambda)
    {
        // The duplicated MakePointerLambda is registered in context.nodeRefsToUpdate, so we search
        // for one which has been updated to reference 'newNode'.
        // And then we update the current 'makePointerLambda'.
        for (const auto& p : context.nodeRefsToUpdate)
        {
            if (p.node->kind == AstNodeKind::MakePointerLambda)
            {
                const auto mpl = castAst<AstMakePointer>(p.node, AstNodeKind::MakePointerLambda);
                if (mpl->lambda == newNode)
                    newNode->makePointerLambda = mpl;
            }
        }
    }

    newNode->returnType = returnType ? returnType->clone(cloneContext) : nullptr;
    if (newNode->returnType)
        newNode->returnType->ownerScope = context.parentScope ? context.parentScope : ownerScope;

    if (content)
    {
        const auto bodyScope     = Ast::newScope(newNode, newNode->token.text, ScopeKind::FunctionBody, functionScope);
        cloneContext.parentScope = bodyScope;
        newNode->content         = content->clone(cloneContext);

        // Content can be an automatic try block in case of a throwable function
        // But we want the scope to have the function statement as an owner
        // :AutomaticTryContent
        if (newNode->content->kind == AstNodeKind::Try)
        {
            SWAG_ASSERT(newNode->content->childs.front()->kind == AstNodeKind::Statement || newNode->content->childs.front()->kind == AstNodeKind::Return);
            bodyScope->owner = newNode->content->childs.front();
        }
        else
            bodyScope->owner = newNode->content;

        cloneSubDecls(nullptr, cloneContext, this, newNode, newNode);
    }
    else
    {
        newNode->content = nullptr;
    }

    context.propagateResult(cloneContext);
    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstAttrDecl>();
    newNode->copyFrom(context, this);
    newNode->tokenName = tokenName;

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstAttrUse>();
    newNode->copyFrom(context, this);

    newNode->content = findChildRef(content, newNode);
    if (newNode->content)
        newNode->content->setOwnerAttrUse(newNode);

    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstFuncCallParam>();
    newNode->copyFrom(context, this);

    // :ReverseLiteralStruct
    // Order of childs is correct, so remove flag
    newNode->removeAstFlag(AST_REVERSE_SEMANTIC);

    newNode->resolvedParameter = resolvedParameter;
    newNode->indexParam        = indexParam;
    return newNode;
}

AstNode* AstBinaryOpNode::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstBinaryOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstConditionalOpNode::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstConditionalOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstIf>();
    newNode->copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = static_cast<AstCompilerIfBlock*>(findChildRef(ifBlock, newNode));
    newNode->elseBlock      = static_cast<AstCompilerIfBlock*>(findChildRef(elseBlock, newNode));
    return newNode;
}

void AstBreakable::copyFrom(CloneContext& context, AstBreakable* from)
{
    AstNode::copyFrom(context, from, false);
    breakableFlags = from->breakableFlags;
    registerIndex  = from->registerIndex;
}

AstNode* AstSubstBreakContinue::clone(CloneContext& context) const
{
    if (altSubstBreakable == context.ownerBreakable)
        return altSubst->clone(context);
    return defaultSubst->clone(context);
}

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    // Do the token replacement only if this is a break/continue in the original breakable,
    // not a 'new' breakable in the user code
    if (context.ownerBreakable == context.replaceTokensBreakable)
    {
        const auto it = context.replaceTokens.find(tokenId);
        if (it != context.replaceTokens.end())
        {
            const auto newNode = Ast::newNode<AstSubstBreakContinue>(nullptr, AstNodeKind::SubstBreakContinue, sourceFile, context.parent);
            newNode->allocateExtension(ExtensionKind::Semantic);
            newNode->extSemantic()->semanticBeforeFct = Semantic::preResolveSubstBreakContinue;

            CloneContext cloneContext = context;
            cloneContext.replaceTokens.clear();
            cloneContext.parent = newNode;

            newNode->defaultSubst      = clone(cloneContext);
            newNode->altSubst          = it->second->clone(cloneContext);
            newNode->altSubstBreakable = context.ownerBreakable;

            context.propagateResult(cloneContext);
            return newNode;
        }
    }

    const auto newNode = Ast::newNode<AstBreakContinue>();
    newNode->copyFrom(context, this);
    newNode->label = label;

    if (context.ownerBreakable)
    {
        if (newNode->kind == AstNodeKind::Break)
            context.ownerBreakable->breakList.push_back(newNode);
        else if (newNode->kind == AstNodeKind::Continue)
            context.ownerBreakable->continueList.push_back(newNode);
    }

    return newNode;
}

AstScopeBreakable::AstScopeBreakable()
{
    breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
}

AstNode* AstScopeBreakable::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstScopeBreakable>();
    newNode->copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstWhile>();
    newNode->copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstFor::~AstFor()
{
    ownerScope->release();
}

AstNode* AstFor::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstFor>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);

    return newNode;
}

AstLoop::~AstLoop()
{
    ownerScope->release();
}

AstNode* AstLoop::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstLoop>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->specificName = findChildRef(specificName, newNode);
    newNode->expression   = findChildRef(expression, newNode);
    newNode->block        = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstVisit::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstVisit>();
    newNode->copyFrom(context, this);

    newNode->extraNameToken   = extraNameToken;
    newNode->wantPointerToken = wantPointerToken;
    newNode->expression       = findChildRef(expression, newNode);
    newNode->block            = findChildRef(block, newNode);
    newNode->aliasNames       = aliasNames;

    return newNode;
}

AstSwitch::AstSwitch()
{
    breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
    breakableFlags &= ~BREAKABLE_CAN_HAVE_CONTINUE;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstSwitch>();
    newNode->copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->expression = findChildRef(expression, newNode);
    for (const auto expr : cases)
        newNode->cases.push_back(static_cast<AstSwitchCase*>(findChildRef(expr, newNode)));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstSwitchCase>();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = castAst<AstSwitch>(context.parent, AstNodeKind::Switch);
    newNode->caseIndex   = caseIndex;
    for (const auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstSwitchCaseBlock::~AstSwitchCaseBlock()
{
    ownerScope->release();
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstSwitchCaseBlock>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->copyFrom(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->ownerCase = castAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstTypeExpression>();
    newNode->copyFrom(context, this);

    newNode->identifier      = findChildRef(identifier, newNode);
    newNode->typeFromLiteral = typeFromLiteral;
    newNode->literalType     = literalType;
    newNode->arrayDim        = arrayDim;
    newNode->typeFlags       = typeFlags;
    newNode->locConst        = locConst;

    // :StructParamsNoSem
    // We need to revaluate the call parameters of the struct initialization, because inside we can have some
    // symbols, and we want them to be correctly found in the right function (inline).
    // Otherwise we can have a out of frame error, because the original symbol is not in the same stack frame.
    if (specFlags & SPECFLAG_CREATED_STRUCT_PARAMETERS)
    {
        if (context.cloneFlags & CLONE_FORCE_OWNER_FCT)
        {
            SWAG_ASSERT(newNode->identifier);
            newNode->identifier->removeAstFlag(AST_NO_SEMANTIC);
            const auto back = castAst<AstIdentifier>(newNode->identifier->childs.back(), AstNodeKind::Identifier);
            SWAG_ASSERT(back->callParameters);
            back->callParameters->removeAstFlag(AST_NO_SEMANTIC);
        }
    }

    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstTypeLambda>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstArrayPointerSlicing::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstArrayPointerSlicing>();
    newNode->copyFrom(context, this);

    newNode->array      = findChildRef(array, newNode);
    newNode->lowerBound = findChildRef(lowerBound, newNode);
    newNode->upperBound = findChildRef(upperBound, newNode);
    for (const auto f : structFlatParams)
        newNode->structFlatParams.push_back(findChildRef(f, newNode));

    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstExpressionList>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstArrayPointerIndex::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstArrayPointerIndex>();
    newNode->copyFrom(context, this);

    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    for (const auto f : structFlatParams)
        newNode->structFlatParams.push_back(findChildRef(f, newNode));

    return newNode;
}

AstStruct::~AstStruct()
{
    if (scope)
        scope->release();
}

AstNode* AstStruct::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstStruct>();
    newNode->copyFrom(context, this, false);
    newNode->packing = packing;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;

    newNode->scope = cloneContext.parentScope;
    newNode->addAlternativeScope(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->tokenName         = tokenName;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;
    newNode->validif           = validif ? validif->clone(cloneContext) : nullptr;
    newNode->addAstFlag(AST_FROM_GENERIC);
    newNode->content->removeAstFlag(AST_NO_SEMANTIC);

    if (newNode->typeInfo)
    {
        newNode->typeInfo = newNode->typeInfo->clone();
        newNode->typeInfo->removeGenericFlag();
        newNode->typeInfo->declNode = newNode;
        newNode->typeInfo->forceComputeName();
    }

    context.propagateResult(cloneContext);
    return newNode;
}

AstImpl::~AstImpl()
{
    if (scope)
        scope->release();
}

AstNode* AstImpl::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstImpl>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, newNode->token.text, ScopeKind::Impl, context.parentScope ? context.parentScope : ownerScope);
    SWAG_ASSERT(cloneContext.ownerStructScope); // Should be setup in generic instantiation
    newNode->scope = cloneContext.parentScope;

    // If this is an interface implementation block, we need to register a sub scope with the name
    // of the interface in the instantiated struct scope.
    // All following functions must be in that sub scope.
    // :SubScopeImplFor
    if (identifierFor)
    {
        const auto baseScope     = cloneContext.ownerStructScope;
        auto       itfName       = childs.front()->token.text;
        cloneContext.parentScope = Ast::newScope(newNode, itfName, ScopeKind::Impl, baseScope);
        newNode->scope           = cloneContext.parentScope;

        // :FakeImplForType
        const auto implTypeInfo  = makeType<TypeInfoStruct>();
        implTypeInfo->name       = itfName;
        implTypeInfo->structName = implTypeInfo->name;
        implTypeInfo->scope      = newNode->scope;
        implTypeInfo->declNode   = newNode;

        AddSymbolTypeInfo toAdd;
        toAdd.node      = newNode;
        toAdd.typeInfo  = implTypeInfo;
        toAdd.kind      = SymbolKind::Struct;
        toAdd.flags     = OVERLOAD_IMPL_IN_STRUCT;
        toAdd.aliasName = &itfName;
        baseScope->symTable.addSymbolTypeInfo(nullptr, toAdd);
    }

    for (const auto c : childs)
    {
        const auto newChild = c->clone(cloneContext);

        if (newChild->kind == AstNodeKind::FuncDecl)
        {
            const auto newFunc       = castAst<AstFuncDecl>(newChild, AstNodeKind::FuncDecl);
            newFunc->originalGeneric = c;

            // Resolution of 'impl' needs all functions to have their symbol registered in the 'impl' scope,
            // in order to be able to wait for the resolution of all functions before solving the 'impl'
            // implementation.
            newNode->scope->symTable.registerSymbolName(nullptr, newFunc, SymbolKind::Function);

            if (!newFunc->genericParameters)
            {
                // The function inside the implementation was generic, so be sure we can now solve
                // its content
                newFunc->content->removeAstFlag(AST_NO_SEMANTIC);

                // Be sure we have a specific no generic typeinfo
                SWAG_ASSERT(newFunc->typeInfo);
                newFunc->typeInfo = newFunc->typeInfo->clone();
                newFunc->typeInfo->removeGenericFlag();
                newFunc->typeInfo->declNode = newFunc;
                newFunc->typeInfo->forceComputeName();
            }
            else
            {
                // If function inside the impl block has specific generic parameters, then we
                // consider it to be generic
                newFunc->removeAstFlag(AST_FROM_GENERIC);
                newFunc->addAstFlag(AST_IS_GENERIC);
                for (const auto g : newFunc->genericParameters->childs)
                {
                    g->removeAstFlag(AST_FROM_GENERIC);
                    g->addAstFlag(AST_IS_GENERIC);
                }

                // Be sure we keep a generic typeinfo
                SWAG_ASSERT(newFunc->typeInfo);
                newFunc->typeInfo = newFunc->typeInfo->clone();
                newFunc->typeInfo->removeFlag(TYPEINFO_FROM_GENERIC);
                newFunc->typeInfo->addFlag(TYPEINFO_GENERIC);
                newFunc->typeInfo->declNode = newFunc;
                newFunc->typeInfo->forceComputeName();
            }
        }
    }

    newNode->identifier    = findChildRef(identifier, newNode);
    newNode->identifierFor = findChildRef(identifierFor, newNode);

    // Cloning an impl block should be called only for generic interface implementation
    SWAG_ASSERT(newNode->identifierFor);

    context.propagateResult(cloneContext);
    return newNode;
}

AstEnum::~AstEnum()
{
    if (scope)
        scope->release();
}

AstNode* AstEnum::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstEnum>();
    newNode->copyFrom(context, this, false);
    newNode->tokenName = tokenName;
    newNode->type      = findChildRef(type, newNode);

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Enum, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    newNode->scope = cloneContext.parentScope;

    SWAG_ASSERT(newNode->typeInfo);
    newNode->typeInfo = newNode->typeInfo->clone();
    newNode->typeInfo->removeGenericFlag();
    newNode->typeInfo->declNode = newNode;

    return newNode;
}

AstNode* AstEnumValue::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstEnumValue>();
    newNode->copyFrom(context, this);
    newNode->attributes = attributes;
    return newNode;
}

AstNode* AstInit::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstInit>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = static_cast<AstFuncCallParams*>(findChildRef(parameters, newNode));
    return newNode;
}

AstNode* AstDropCopyMove::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstDropCopyMove>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstReturn>();
    newNode->copyFrom(context, this);

    // If return in an inline block has already been solved, we need this flag !
    if (context.cloneFlags & CLONE_RAW)
        newNode->addSemFlag(semFlags & SEMFLAG_EMBEDDED_RETURN);

    return newNode;
}

AstCompilerMacro::~AstCompilerMacro()
{
    if (scope)
        scope->release();
}

AstNode* AstCompilerMacro::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstCompilerMacro>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = cloneContext.parentScope;
    childs.back()->clone(cloneContext);
    context.propagateResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerMixin::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstCompilerMixin>();
    newNode->copyFrom(context, this);
    newNode->replaceTokens = replaceTokens;
    return newNode;
}

AstInline::~AstInline()
{
    if (scope && scope->kind == ScopeKind::Inline)
        scope->release();
}

AstNode* AstInline::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstInline>();
    newNode->copyFrom(context, this, false);

    // If we clone an inline block after bytecode generation (this happens if we have a mixin inside an inline function
    // for example), then we do not want to copy the AST_NO_BYTECODE_CHILDS (because we want the inline block to be
    // generated).
    // :EmitInlineOnce
    newNode->removeAstFlag(AST_NO_BYTECODE_CHILDS);

    newNode->func = func;

    // Is this correct ? Seems a little wierd, but that way we do not have to copy the parametersScope
    // content, which is not really possible for now (12/07/2021) (i.e. no way to copy already registered symbols in the scope).
    // Because it can happen that an inline block already solved is copied.
    // For example because of convertStructParamsToTmpVar, with inline calls as parameters: func(A{round(6)}) => round already inlined.
    // I guess one day this will hit me in the face...
    newNode->parametersScope = parametersScope;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.ownerInline = newNode;

    if (scope && scope->kind == ScopeKind::Inline)
        cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);

    newNode->scope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerIfBlock::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstCompilerIfBlock>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                 = context;
    cloneContext.parent               = newNode;
    cloneContext.ownerCompilerIfBlock = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    if (newNode->hasExtOwner() && newNode->extOwner()->ownerCompilerIfBlock)
        newNode->extOwner()->ownerCompilerIfBlock->blocks.push_back(newNode);
    return newNode;
}

AstNode* AstCompilerSpecFunc::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstCompilerSpecFunc>();

    auto cloneContext = context;
    cloneContext.forceFlags &= ~AST_SPEC_STACK_SIZE;

    newNode->copyFrom(cloneContext, this, false);

    // Clone childs by hand, because a compiler block can contain a sub function, and this sub
    // function shouldn't have the ownerInline set (if the #ast is in an inline block)
    cloneContext.parent = newNode;
    for (const auto p : childs)
    {
        if (p->kind == AstNodeKind::FuncDecl)
        {
            cloneContext.ownerInline = nullptr;
            cloneContext.removeFlags |= AST_IN_RUN_BLOCK;
        }
        else
        {
            cloneContext.ownerInline = context.ownerInline;
            cloneContext.removeFlags = context.removeFlags;
        }

        p->clone(cloneContext)->removeAstFlag(AST_NO_SEMANTIC);
    }

    // If the compiler block has an embedded function, we need to restore the semantic pass on that function
    // content now
    if (newNode->childs.size() > 1)
    {
        // We also want to replace the name of the function (and the reference to it) in case
        // the block is in a mixin block, because in that case the function can be registered
        // more than once in the same scope.
        const int id = g_UniqueID.fetch_add(1);
        // ReSharper disable once StringLiteralTypo
        const Utf8 newName = "__cmpfunc" + to_string(id);

        const auto func  = castAst<AstFuncDecl>(newNode->childs.front(), AstNodeKind::FuncDecl);
        func->token.text = newName;
        func->removeAstFlag(AST_NO_SEMANTIC);
        func->content->removeAstFlag(AST_NO_SEMANTIC);
        func->addAstFlag(AST_FROM_GENERIC);
        newNode->ownerScope->symTable.registerSymbolName(nullptr, func, SymbolKind::Function);

        // Ref to the function
        const auto idRef                 = castAst<AstIdentifierRef>(newNode->childs.back(), AstNodeKind::IdentifierRef);
        idRef->childs.back()->token.text = newName;
    }

    context.propagateResult(cloneContext);
    return newNode;
}

AstNode* AstNameSpace::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstNameSpace>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstTryCatchAssume::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstTryCatchAssume>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                = context;
    cloneContext.ownerTryCatchAssume = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propagateResult(cloneContext);

    return newNode;
}

AstNode* AstAlias::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstAlias>();
    newNode->copyFrom(context, this);
    newNode->kwdLoc = kwdLoc;
    if (resolvedSymbolName)
        newNode->resolvedSymbolName = newNode->ownerScope->symTable.registerSymbolName(nullptr, newNode, resolvedSymbolName->kind);
    return newNode;
}

AstNode* AstCast::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstCast>();
    newNode->copyFrom(context, this);
    newNode->toCastTypeInfo = toCastTypeInfo;
    return newNode;
}

AstNode* AstFuncCallParams::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstFuncCallParams>();
    newNode->copyFrom(context, this);
    newNode->aliasNames = aliasNames;

    // Propagate aliases
    int idx = 0;
    for (const auto& it : newNode->aliasNames)
    {
        auto itn = context.replaceNames.find(it.text);
        if (itn != context.replaceNames.end())
        {
            context.usedReplaceNames.insert(itn->second);
            newNode->aliasNames[idx].text = itn->second;
        }

        idx++;
    }

    return newNode;
}

AstNode* AstRange::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstRange>();
    newNode->copyFrom(context, this);
    newNode->expressionLow = findChildRef(expressionLow, newNode);
    newNode->expressionUp  = findChildRef(expressionUp, newNode);
    return newNode;
}

AstNode* AstRefSubDecl::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstRefSubDecl>();
    newNode->copyFrom(context, this);
    newNode->refSubDecl = refSubDecl;
    context.nodeRefsToUpdate.push_back({newNode, &newNode->refSubDecl});
    return newNode;
}

AstNode* AstMakePointer::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstMakePointer>();
    newNode->copyFrom(context, this);

    if (lambda)
    {
        // This is super hacky
        // The problem is that the relation between a lambda and the make pointer lambda is tight,
        // and with #mixin the lambda is not duplicated, but the make pointer lambda is...
        // So this is a mess
        if (context.forceFlags & AST_IN_MIXIN)
        {
            if (lambda->captureParameters && childs.front() == lambda->captureParameters)
            {
                SWAG_ASSERT(newNode->childs[0]->kind == AstNodeKind::FuncCallParams);
                lambda->captureParameters = newNode->childs[0];
            }

            lambda->makePointerLambda = newNode;
        }

        newNode->lambda = lambda;
        context.nodeRefsToUpdate.push_back({newNode, reinterpret_cast<AstNode**>(&newNode->lambda)});
    }

    return newNode;
}

AstNode* AstOp::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstOp>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstDefer::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstDefer>();
    newNode->copyFrom(context, this);
    newNode->deferKind = deferKind;
    return newNode;
}

AstNode* AstWith::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstWith>();
    newNode->copyFrom(context, this);
    newNode->id = id;
    return newNode;
}

AstNode* AstLiteral::clone(CloneContext& context)
{
    const auto newNode = Ast::newNode<AstLiteral>();
    newNode->copyFrom(context, this);
    newNode->literalType  = literalType;
    newNode->literalValue = literalValue;
    return newNode;
}
