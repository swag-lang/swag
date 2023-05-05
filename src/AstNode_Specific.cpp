#include "pch.h"
#include "Ast.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "ByteCode.h"
#include "Module.h"
#include "LanguageSpec.h"

AstNode* AstVarDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstVarDecl>();
    newNode->copyFrom(context, this);
    newNode->publicName  = publicName;
    newNode->attributes  = attributes;
    newNode->assignToken = assignToken;
    if (newNode->attrUse)
        newNode->attrUse->content = newNode;

    newNode->type             = (AstTypeExpression*) findChildRef(type, newNode);
    newNode->assignment       = findChildRef(assignment, newNode);
    newNode->genTypeComesFrom = genTypeComesFrom;

    newNode->typeConstraint = findChildRef(typeConstraint, newNode);
    if (newNode->typeConstraint)
        newNode->typeConstraint->flags &= ~AST_NO_SEMANTIC;

    // Is there an alias ?
    auto it = context.replaceNames.find(newNode->token.text);
    if (it != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(it->second);
        newNode->token.text = it->second;
    }

    return newNode;
}

void AstIdentifierRef::computeName()
{
    token.text.clear();
    for (auto child : childs)
    {
        if (!token.text.empty())
            token.text += ".";
        token.text += child->token.text;
    }
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIdentifierRef>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstIdentifier::~AstIdentifier()
{
    if (identifierExtension)
        Allocator::free<IdentifierExtension>(identifierExtension);
}

AstIdentifierRef* AstIdentifier::identifierRef()
{
    if (parent->kind == AstNodeKind::IdentifierRef)
        return CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);

    auto check = parent;
    while (check->kind != AstNodeKind::IdentifierRef)
    {
        check = check->parent;
        SWAG_ASSERT(check);
    }

    return CastAst<AstIdentifierRef>(check, AstNodeKind::IdentifierRef);
}

void AstIdentifier::allocateIdentifierExtension()
{
    if (identifierExtension)
        return;
    identifierExtension = Allocator::alloc<IdentifierExtension>();
}

AstNode* AstIdentifier::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIdentifier>();
    newNode->copyFrom(context, this);

    // Is there an alias ?
    auto itn = context.replaceNames.find(newNode->token.text);
    if (itn != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(itn->second);
        newNode->token.text = itn->second;
    }

    newNode->genericParameters = (AstFuncCallParams*) findChildRef(genericParameters, newNode);
    newNode->callParameters    = (AstFuncCallParams*) findChildRef(callParameters, newNode);

    if (identifierExtension)
    {
        newNode->allocateIdentifierExtension();
        newNode->identifierExtension->aliasNames   = identifierExtension->aliasNames;
        newNode->identifierExtension->scopeUpMode  = identifierExtension->scopeUpMode;
        newNode->identifierExtension->scopeUpValue = identifierExtension->scopeUpValue;
    }

    // Check if we need to replace the token.text with a type substitution
    // That way the new resolveIdentifier will just try to keep the typeinfo
    auto it = context.replaceTypes.find(newNode->token.text);
    if (it != context.replaceTypes.end())
    {
        // :ForLocationInValidIf
        auto it1 = context.replaceTypesFrom.find(newNode->token.text);
        if (it1 != context.replaceTypesFrom.end())
        {
            newNode->allocateIdentifierExtension();
            newNode->identifierExtension->fromAlternateVar = it1->second;
        }

        if (!it->second->isNative(NativeTypeKind::Undefined))
            newNode->token.text = it->second->name;

        newNode->typeInfo = it->second;
        if (newNode->typeInfo->declNode)
        {
            newNode->resolvedSymbolName     = newNode->typeInfo->declNode->resolvedSymbolName;
            newNode->resolvedSymbolOverload = newNode->typeInfo->declNode->resolvedSymbolOverload;
        }

        newNode->flags |= AST_FROM_GENERIC | AST_FROM_GENERIC_REPLACE;
    }

    return newNode;
}

bool AstFuncDecl::mustInline()
{
    if (attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
        return true;
    if (sourceFile->module->buildCfg.byteCodeInline == false)
        return false;
    if (attributeFlags & ATTRIBUTE_INLINE)
        return true;
    if (!content)
        return false;

    if (sourceFile->module->buildCfg.byteCodeAutoInline == false)
        return false;
    if (attributeFlags & ATTRIBUTE_NO_INLINE)
        return false;

    // All short functions
    if (specFlags & AstFuncDecl::SPECFLAG_SHORT_FORM)
        return true;

    return false;
}

Utf8 AstFuncDecl::getCallName()
{
    if (attributeFlags & (ATTRIBUTE_FOREIGN | ATTRIBUTE_PUBLIC))
    {
        if (!(attributeFlags & ATTRIBUTE_SHARP_FUNC))
        {
            computeFullNameForeign(true);
            return fullnameForeign;
        }
    }

    SWAG_ASSERT(hasExtByteCode() && extByteCode()->bc);
    return extByteCode()->bc->getCallName();
}

Utf8 AstFuncDecl::getNameForUserCompiler()
{
    if (attributeFlags & ATTRIBUTE_SHARP_FUNC)
    {
        auto fct = parent;
        while (fct && (fct->kind != AstNodeKind::FuncDecl || fct->attributeFlags & ATTRIBUTE_SHARP_FUNC))
            fct = fct->parent;
        if (fct)
            return fct->getScopedName();
    }

    return getScopedName();
}

const char* AstFuncDecl::getDisplayNameC()
{
    auto res = getDisplayName();
    return _strdup(res.c_str()); // Leak and slow, but only for messages
}

Utf8 AstFuncDecl::getDisplayName()
{
    if (attributeFlags & ATTRIBUTE_AST_FUNC)
        return "'#ast' block";
    if (attributeFlags & (ATTRIBUTE_RUN_FUNC | ATTRIBUTE_RUN_GENERATED_FUNC))
        return "'#run' block";
    if (attributeFlags & ATTRIBUTE_MATCH_VALIDIF_FUNC)
        return "'#validif' block";
    if (attributeFlags & ATTRIBUTE_MATCH_VALIDIFX_FUNC)
        return "'#validifx' block";

    if (attributeFlags & ATTRIBUTE_TEST_FUNC && attributeFlags & ATTRIBUTE_SHARP_FUNC)
        return "'#test' block";
    if (attributeFlags & ATTRIBUTE_MAIN_FUNC)
        return "'#main' block";
    if (attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        return "'#message' block";
    if (attributeFlags & ATTRIBUTE_INIT_FUNC)
        return "'#init' block";
    if (attributeFlags & ATTRIBUTE_DROP_FUNC)
        return "'#drop' block";
    if (attributeFlags & ATTRIBUTE_PREMAIN_FUNC)
        return "'#premain' block";

    if (specFlags & AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION)
        return "lambda";

    if (attributeFlags & ATTRIBUTE_SHARP_FUNC)
        return Fmt("'%s' block", token.ctext());

    if (attributeFlags & ATTRIBUTE_MIXIN)
        return Fmt("mixin '%s'", token.ctext());
    if (attributeFlags & ATTRIBUTE_MACRO)
        return Fmt("macro '%s'", token.ctext());

    return Fmt("function '%s'", token.ctext());
}

void AstFuncDecl::computeFullNameForeign(bool forExport)
{
    ScopedLock lk(mutex);
    if (!fullnameForeign.empty())
        return;

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
    if (!forExport)
    {
        auto value = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
        if (value && !value->text.empty())
            fullnameForeign = value->text;
        else
            fullnameForeign = token.text;
        return;
    }

    SWAG_ASSERT(ownerScope);

    auto nameForeign = getScopedName();

    // If the symbol has overloads, i.e. more than one definition, then we
    // append the type
    if (resolvedSymbolName && resolvedSymbolName->overloads.size() > 1)
    {
        // Empty (forward) decl are counted as overloads, so be sure it's not a bunch of
        // empty functions.
        uint32_t countNoEmpty = 0;
        for (auto r : resolvedSymbolName->overloads)
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
            typeFunc->computeScopedName();
            auto pz = strstr(typeFunc->scopedName.c_str(), "(");
            SWAG_ASSERT(pz);
            nameForeign += pz;
        }
    }

    fullnameForeign = nameForeign;

    // Normalize token.text
    auto len = nameForeign.length();
    auto pz  = nameForeign.buffer;
    auto pzd = fullnameForeign.buffer;
    for (uint32_t i = 0; i < len; i++)
    {
        if (*pz == ' ')
        {
            pz++;
        }
        else if (*pz == ',' || *pz == '\'' || *pz == '-' || *pz == '>')
        {
            *pzd++ = '@';
            pz++;
        }
        else
            *pzd++ = *pz++;
    }

    *pzd++ = 0;

    fullnameForeign.count = (uint32_t) (pzd - fullnameForeign.buffer) - 1;
}

bool AstFuncDecl::cloneSubDecls(ErrorContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode)
{
    // We need to duplicate sub declarations, and register the symbol in the new corresponding scope
    for (auto f : subDecls)
    {
        ScopedLock lk(f->mutex);

        // A sub declaration node has the root of the file as parent, but has the correct scope. We need to find
        // the duplicated parent node that corresponds to the original one, in order to get the corresponding new
        // scope (if a sub declaration is declared inside an if statement scope for example, we need the duplicated
        // sub declaration to be registered in the corresponding new scope).
        auto newScopeNode = oldOwnerNode->findChildRefRec(f->ownerScope->owner, refNode);
        SWAG_ASSERT(newScopeNode);
        auto subFuncScope = newScopeNode->ownerScope;

        cloneContext.parentScope = subFuncScope;
        cloneContext.parent      = nullptr; // Register in parent will be done at the end (race condition)
        if (f->parent->kind == AstNodeKind::AttrUse)
            f = f->parent;
        auto subF         = f->clone(cloneContext);
        auto subDecl      = subF->kind == AstNodeKind::AttrUse ? ((AstAttrUse*) subF)->content : subF;
        subDecl->typeInfo = subDecl->typeInfo->clone();

        SymbolKind symKind = SymbolKind::Invalid;
        switch (subDecl->kind)
        {
        case AstNodeKind::FuncDecl:
        {
            auto nodeFunc = CastAst<AstFuncDecl>(subDecl, AstNodeKind::FuncDecl);
            nodeFunc->content->flags &= ~AST_NO_SEMANTIC;
            if (cloneContext.alternativeScope)
                nodeFunc->addAlternativeScope(cloneContext.alternativeScope);
            symKind = SymbolKind::Function;

            // Function is supposed to start semantic when captured parameters have been evaluated
            // So no semantic on it now
            if (nodeFunc->captureParameters)
                nodeFunc->flags |= AST_NO_SEMANTIC | AST_SPEC_SEMANTIC1;

            // If function is linked to a makePointerLambda, then we make a unique name for the function, and change the
            // corresponding reference.
            // This way, even if the lambda is used insided a mixin, we won't have any name collisions if the mixin is included multiple times
            // in the same scope.
            // Example 4506.
            if (nodeFunc->makePointerLambda)
            {
                int id = g_UniqueID.fetch_add(1);
                subDecl->token.text += to_string(id);
                auto idRef        = CastAst<AstIdentifier>(nodeFunc->makePointerLambda->childs.front()->childs.back(), AstNodeKind::Identifier);
                idRef->token.text = subDecl->token.text;
            }

            break;
        }
        case AstNodeKind::StructDecl:
        {
            symKind           = SymbolKind::Struct;
            auto nodeStruct   = CastAst<AstStruct>(subDecl, AstNodeKind::StructDecl);
            auto typeStruct   = CastTypeInfo<TypeInfoStruct>(subDecl->typeInfo, TypeInfoKind::Struct);
            typeStruct->scope = nodeStruct->scope;
            break;
        }
        case AstNodeKind::InterfaceDecl:
            symKind = SymbolKind::Interface;
            break;

        default:
            SWAG_ASSERT(false);
            break;
        }

        subDecl->typeInfo->removeGenericFlag();
        subDecl->typeInfo->declNode = subDecl;

        subDecl->semFlags |= SEMFLAG_FILE_JOB_PASS;
        newFctNode->subDecls.push_back(subDecl);

        // Be sure symbol is not already there. This can happen when using mixins
        if (context)
        {
            auto sym = subFuncScope->symTable.find(subDecl->token.text);
            if (sym)
            {
                Diagnostic diag{subDecl, Fmt(Err(Err0346), subDecl->token.ctext())};
                return context->report(diag);
            }
        }

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
    auto newNode      = Ast::newNode<AstFuncDecl>();
    auto cloneContext = context;
    cloneContext.forceFlags &= ~AST_SPEC_STACKSIZE;

    newNode->copyFrom(context, this, false);
    newNode->aliasMask   = aliasMask;
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;
    newNode->tokenName   = tokenName;

    newNode->specFlags &= ~(AstFuncDecl::SPECFLAG_FULL_RESOLVE | AstFuncDecl::SPECFLAG_PARTIAL_RESOLVE | AstFuncDecl::SPECFLAG_SHORT_FORM);

    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto functionScope    = Ast::newScope(newNode, newNode->token.text, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = functionScope;

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

    // We have a correponsing MakePointerLambda, which has already been duplicated.
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
                auto mpl = CastAst<AstMakePointer>(p.node, AstNodeKind::MakePointerLambda);
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
        auto bodyScope           = Ast::newScope(newNode, newNode->token.text, ScopeKind::FunctionBody, functionScope);
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

    context.propageResult(cloneContext);
    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAttrDecl>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAttrUse>();
    newNode->copyFrom(context, this);

    newNode->content = findChildRef(content, newNode);
    if (newNode->content)
        newNode->content->setOwnerAttrUse(newNode);

    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFuncCallParam>();
    newNode->copyFrom(context, this);

    // :ReverseLiteralStruct
    // Order of childs is correct, so remove flag
    newNode->flags &= ~AST_REVERSE_SEMANTIC;

    newNode->resolvedParameter = resolvedParameter;
    newNode->indexParam        = indexParam;
    return newNode;
}

AstNode* AstBinaryOpNode::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstBinaryOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstConditionalOpNode::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstConditionalOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIf>();
    newNode->copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = (AstCompilerIfBlock*) findChildRef(ifBlock, newNode);
    newNode->elseBlock      = (AstCompilerIfBlock*) findChildRef(elseBlock, newNode);
    return newNode;
}

void AstBreakable::copyFrom(CloneContext& context, AstBreakable* from)
{
    AstNode::copyFrom(context, from, false);
    breakableFlags = from->breakableFlags;
    registerIndex  = from->registerIndex;
    registerIndex1 = from->registerIndex1;
}

AstNode* AstSubstBreakContinue::clone(CloneContext& context)
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
        auto it = context.replaceTokens.find(tokenId);
        if (it != context.replaceTokens.end())
        {
            auto newNode = Ast::newNode<AstSubstBreakContinue>(nullptr, AstNodeKind::SubstBreakContinue, sourceFile, context.parent);
            newNode->allocateExtension(ExtensionKind::Semantic);
            newNode->extSemantic()->semanticBeforeFct = SemanticJob::preResolveSubstBreakContinue;

            CloneContext cloneContext = context;
            cloneContext.replaceTokens.clear();
            cloneContext.parent = newNode;

            newNode->defaultSubst      = clone(cloneContext);
            newNode->altSubst          = it->second->clone(cloneContext);
            newNode->altSubstBreakable = context.ownerBreakable;

            context.propageResult(cloneContext);
            return newNode;
        }
    }

    auto newNode = Ast::newNode<AstBreakContinue>();
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

AstNode* AstScopeBreakable::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstScopeBreakable>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstWhile>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

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
    auto newNode = Ast::newNode<AstFor>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

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
    auto newNode = Ast::newNode<AstLoop>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->specificName = findChildRef(specificName, newNode);
    newNode->expression   = findChildRef(expression, newNode);
    newNode->block        = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstVisit::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstVisit>();
    newNode->copyFrom(context, this);

    newNode->extraNameToken   = extraNameToken;
    newNode->wantPointerToken = wantPointerToken;
    newNode->expression       = findChildRef(expression, newNode);
    newNode->block            = findChildRef(block, newNode);
    newNode->aliasNames       = aliasNames;

    return newNode;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitch>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->expression = findChildRef(expression, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitchCase>();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = CastAst<AstSwitch>(context.parent, AstNodeKind::Switch);
    newNode->caseIndex   = caseIndex;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstSwitchCaseBlock::~AstSwitchCaseBlock()
{
    ownerScope->release();
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitchCaseBlock>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->copyFrom(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->ownerCase = CastAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTypeExpression>();
    newNode->copyFrom(context, this);

    newNode->identifier      = findChildRef(identifier, newNode);
    newNode->typeFromLiteral = typeFromLiteral;
    newNode->ptrCount        = ptrCount;
    newNode->literalType     = literalType;

    for (int i = 0; i < ptrCount; i++)
        newNode->ptrFlags[i] = ptrFlags[i];

    newNode->arrayDim  = arrayDim;
    newNode->typeFlags = typeFlags;

    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTypeLambda>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstArrayPointerSlicing::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstArrayPointerSlicing>();
    newNode->copyFrom(context, this);

    newNode->array      = findChildRef(array, newNode);
    newNode->lowerBound = findChildRef(lowerBound, newNode);
    newNode->upperBound = findChildRef(upperBound, newNode);
    for (auto f : structFlatParams)
        newNode->structFlatParams.push_back(findChildRef(f, newNode));

    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstExpressionList>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstArrayPointerIndex::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstArrayPointerIndex>();
    newNode->copyFrom(context, this);

    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    for (auto f : structFlatParams)
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
    auto newNode = Ast::newNode<AstStruct>();
    newNode->copyFrom(context, this, false);
    newNode->packing = packing;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;

    newNode->scope = cloneContext.parentScope;
    newNode->addAlternativeScope(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;
    newNode->validif           = validif ? validif->clone(cloneContext) : nullptr;
    newNode->flags |= AST_FROM_GENERIC;
    newNode->content->flags &= ~AST_NO_SEMANTIC;

    if (newNode->typeInfo)
    {
        newNode->typeInfo = newNode->typeInfo->clone();
        newNode->typeInfo->removeGenericFlag();
        newNode->typeInfo->declNode = newNode;
        newNode->typeInfo->forceComputeName();
    }

    context.propageResult(cloneContext);
    return newNode;
}

AstImpl::~AstImpl()
{
    if (scope)
        scope->release();
}

AstNode* AstImpl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstImpl>();
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
        auto baseScope           = cloneContext.ownerStructScope;
        auto itfName             = childs.front()->token.text;
        cloneContext.parentScope = Ast::newScope(newNode, itfName, ScopeKind::Impl, baseScope);
        newNode->scope           = cloneContext.parentScope;

        // :FakeImplForType
        auto implTypeInfo        = makeType<TypeInfoStruct>();
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

    for (auto c : childs)
    {
        auto newChild = c->clone(cloneContext);

        if (newChild->kind == AstNodeKind::FuncDecl)
        {
            auto newFunc = CastAst<AstFuncDecl>(newChild, AstNodeKind::FuncDecl);

            // Resolution of 'impl' needs all functions to have their symbol registered in the 'impl' scope,
            // in order to be able to wait for the resolution of all functions before solving the 'impl'
            // implementation.
            newNode->scope->symTable.registerSymbolName(nullptr, newFunc, SymbolKind::Function);

            if (!newFunc->genericParameters)
            {
                // The function inside the implementation was generic, so be sure we can now solve
                // its content
                newFunc->content->flags &= ~AST_NO_SEMANTIC;

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
                newFunc->flags &= ~AST_FROM_GENERIC;
                newFunc->flags |= AST_IS_GENERIC;
                for (auto g : newFunc->genericParameters->childs)
                {
                    g->flags &= ~AST_FROM_GENERIC;
                    g->flags |= AST_IS_GENERIC;
                }

                // Be sure we keep a generic typeinfo
                SWAG_ASSERT(newFunc->typeInfo);
                newFunc->typeInfo = newFunc->typeInfo->clone();
                newFunc->typeInfo->flags &= ~TYPEINFO_FROM_GENERIC;
                newFunc->typeInfo->flags |= TYPEINFO_GENERIC;
                newFunc->typeInfo->declNode = newFunc;
                newFunc->typeInfo->forceComputeName();
            }
        }
    }

    newNode->identifier    = findChildRef(identifier, newNode);
    newNode->identifierFor = findChildRef(identifierFor, newNode);

    // Cloning an impl block should be called only for generic interface implementation
    SWAG_ASSERT(newNode->identifierFor);

    context.propageResult(cloneContext);
    return newNode;
}

AstEnum::~AstEnum()
{
    if (scope)
        scope->release();
}

AstNode* AstEnum::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstEnum>();
    newNode->copyFrom(context, this, false);

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Enum, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->scope = cloneContext.parentScope;

    SWAG_ASSERT(newNode->typeInfo);
    newNode->typeInfo = newNode->typeInfo->clone();
    newNode->typeInfo->removeGenericFlag();
    newNode->typeInfo->declNode = newNode;

    return newNode;
}

AstNode* AstEnumValue::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstEnumValue>();
    newNode->copyFrom(context, this);
    newNode->attributes = attributes;
    return newNode;
}

AstNode* AstInit::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstInit>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = (AstFuncCallParams*) findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstDropCopyMove::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstDropCopyMove>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstReturn>();
    newNode->copyFrom(context, this);

    // If return in an inline block has already been solved, we need this flag !
    if (context.cloneFlags & CLONE_RAW)
        newNode->semFlags |= semFlags & SEMFLAG_EMBEDDED_RETURN;

    return newNode;
}

AstCompilerMacro::~AstCompilerMacro()
{
    if (scope)
        scope->release();
}

AstNode* AstCompilerMacro::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerMacro>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = cloneContext.parentScope;
    childs.back()->clone(cloneContext);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerMixin::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerMixin>();
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
    auto newNode = Ast::newNode<AstInline>();
    newNode->copyFrom(context, this, false);

    // If we clone an inline block after bytecode generation (this happens if we have a mixin inside an inline function
    // for example), then we do not want to copy the AST_NO_BYTECODE_CHILDS (because we want the inline block to be
    // generated).
    // :EmitInlineOnce
    newNode->flags &= ~AST_NO_BYTECODE_CHILDS;

    newNode->func = func;

    // Is this correct ? Seems a little wierd, but that way we do not have to copy the parametersScope
    // content, which is not really possible for now (12/07/2021) (i.e. no way to copy already registered symbols in the scope).
    // Because it can happen that an inline block already solved is copied.
    // For example because of convertStructParamsToTmpVar, with inline calls as parameters: titi(A{round(6)}) => round already inlined.
    // I guess one day this will hit me in the face...
    newNode->parametersScope = parametersScope;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.ownerInline = newNode;

    if (scope && scope->kind == ScopeKind::Inline)
        cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);

    newNode->scope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerIfBlock::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerIfBlock>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                 = context;
    cloneContext.parent               = newNode;
    cloneContext.ownerCompilerIfBlock = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    if (newNode->hasExtOwner() && newNode->extOwner()->ownerCompilerIfBlock)
        newNode->extOwner()->ownerCompilerIfBlock->blocks.push_back(newNode);
    return newNode;
}

AstNode* AstCompilerSpecFunc::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerSpecFunc>();

    auto cloneContext = context;
    cloneContext.forceFlags &= ~AST_SPEC_STACKSIZE;

    newNode->copyFrom(cloneContext, this, false);

    // Clone childs by hand, because a compiler block can contain a sub function, and this sub
    // function shouldn't have the ownerInline set (if the #ast is in an inline block)
    cloneContext.parent = newNode;
    for (auto p : childs)
    {
        if (p->kind == AstNodeKind::FuncDecl)
        {
            cloneContext.ownerInline = nullptr;
            cloneContext.removeFlags |= AST_RUN_BLOCK;
        }
        else
        {
            cloneContext.ownerInline = context.ownerInline;
            cloneContext.removeFlags = context.removeFlags;
        }

        p->clone(cloneContext)->flags &= ~AST_NO_SEMANTIC;
    }

    // If the compiler block has an embedded function, we need to restore the semantic pass on that function
    // content now
    if (newNode->childs.size() > 1)
    {
        // We also want to replace the name of the function (and the reference to it) in case
        // the block is in a mixin block, because in that case the function can be registered
        // more than once in the same scope.
        int  id      = g_UniqueID.fetch_add(1);
        Utf8 newName = "__cmpfunc" + to_string(id);

        auto func        = CastAst<AstFuncDecl>(newNode->childs.front(), AstNodeKind::FuncDecl);
        func->token.text = newName;
        func->flags &= ~AST_NO_SEMANTIC;
        func->content->flags &= ~AST_NO_SEMANTIC;
        func->flags |= AST_FROM_GENERIC;
        newNode->ownerScope->symTable.registerSymbolName(nullptr, func, SymbolKind::Function);

        // Ref to the function
        auto idRef                       = CastAst<AstIdentifierRef>(newNode->childs.back(), AstNodeKind::IdentifierRef);
        idRef->childs.back()->token.text = newName;
    }

    context.propageResult(cloneContext);
    return newNode;
}

AstNode* AstNameSpace::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstNameSpace>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstTryCatchAssume::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTryCatchAssume>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                = context;
    cloneContext.ownerTryCatchAssume = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstAlias::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAlias>();
    newNode->copyFrom(context, this);
    if (resolvedSymbolName)
        newNode->resolvedSymbolName = newNode->ownerScope->symTable.registerSymbolName(nullptr, newNode, resolvedSymbolName->kind);
    return newNode;
}

AstNode* AstCast::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCast>();
    newNode->copyFrom(context, this);
    newNode->toCastTypeInfo = toCastTypeInfo;
    return newNode;
}

AstNode* AstFuncCallParams::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFuncCallParams>();
    newNode->copyFrom(context, this);
    newNode->aliasNames = aliasNames;

    // Propagate aliases
    int idx = 0;
    for (auto it : newNode->aliasNames)
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
    auto newNode = Ast::newNode<AstRange>();
    newNode->copyFrom(context, this);
    newNode->expressionLow = findChildRef(expressionLow, newNode);
    newNode->expressionUp  = findChildRef(expressionUp, newNode);
    return newNode;
}

AstNode* AstMakePointer::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstMakePointer>();
    newNode->copyFrom(context, this);

    if (lambda)
    {
        // This is super hacky
        // The problem is that the relation between a lambda and the makepointer lambda is tight,
        // and with #mixin the lambda is not duplicated, but the makepointer lambda is...
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
        context.nodeRefsToUpdate.push_back({newNode, (AstNode**) &newNode->lambda});
    }

    return newNode;
}

AstNode* AstOp::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstOp>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstDefer::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstDefer>();
    newNode->copyFrom(context, this);
    newNode->deferKind = deferKind;
    return newNode;
}

AstNode* AstWith::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstWith>();
    newNode->copyFrom(context, this);
    newNode->id = id;
    return newNode;
}

AstNode* AstLiteral::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstLiteral>();
    newNode->copyFrom(context, this);
    newNode->literalType  = literalType;
    newNode->literalValue = literalValue;
    return newNode;
}