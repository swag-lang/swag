#include "pch.h"
#include "Ast.h"
#include "Generic.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneHie)
{
    kind = from->kind;

    flags = from->flags;
    flags &= ~AST_IS_GENERIC;
    flags |= context.forceFlags;
    flags &= ~context.removeFlags;

    // Copy some specific flags
    doneFlags |= from->doneFlags & AST_DONE_INLINED;
    semFlags |= from->semFlags & AST_SEM_STRUCT_REGISTERED;

    ownerStructScope     = context.ownerStructScope ? context.ownerStructScope : from->ownerStructScope;
    ownerScope           = context.parentScope ? context.parentScope : from->ownerScope;
    ownerBreakable       = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable;
    ownerInline          = context.ownerInline ? context.ownerInline : from->ownerInline;
    ownerFct             = context.ownerFct ? context.ownerFct : from->ownerFct;
    ownerCompilerIfBlock = context.ownerCompilerIfBlock ? context.ownerCompilerIfBlock : from->ownerCompilerIfBlock;

    if (context.ownerTryCatchAssume || (from->extension && from->extension->ownerTryCatchAssume))
    {
        allocateExtension();
        extension->ownerTryCatchAssume = context.ownerTryCatchAssume ? context.ownerTryCatchAssume : from->extension->ownerTryCatchAssume;
    }

    // Replace a type by another one during generic instantiation
    typeInfo = Generic::doTypeSubstitution(context.replaceTypes, from->typeInfo);
    if (typeInfo != from->typeInfo)
        flags |= AST_FROM_GENERIC;

    // This should not be copied. It will be recomputed if necessary.
    // This can cause some problems with inline functions and autocast, as inline functions are evaluated
    // as functions, and also each time they are inlined.
    if (context.rawClone)
        castedTypeInfo = from->castedTypeInfo;

    resolvedSymbolName     = from->resolvedSymbolName;
    resolvedSymbolOverload = from->resolvedSymbolOverload;

    token = from->token;
    if (context.forceLocation)
    {
        token.startLocation = context.forceLocation->startLocation;
        token.endLocation   = context.forceLocation->endLocation;
    }

    semanticFct = from->semanticFct;
    byteCodeFct = from->byteCodeFct;
    if (from->extension)
    {
        allocateExtension();
        extension->semanticBeforeFct            = from->extension->semanticBeforeFct;
        extension->semanticAfterFct             = from->extension->semanticAfterFct;
        extension->byteCodeBeforeFct            = from->extension->byteCodeBeforeFct;
        extension->byteCodeAfterFct             = from->extension->byteCodeAfterFct;
        extension->bc                           = from->extension->bc;
        extension->resolvedUserOpSymbolOverload = from->extension->resolvedUserOpSymbolOverload;
        extension->collectTypeInfo              = from->extension->collectTypeInfo;
        extension->exportNode                   = from->extension->exportNode;
        extension->castOffset                   = from->extension->castOffset;
        extension->stackOffset                  = from->extension->stackOffset;
        extension->anyTypeSegment               = from->extension->anyTypeSegment;
        extension->anyTypeOffset                = from->extension->anyTypeOffset;
    }

    if (from->computedValue)
    {
        computedValue  = g_Allocator.alloc<ComputedValue>();
        *computedValue = *from->computedValue;
    }

    token.text = from->token.text;
    sourceFile = from->sourceFile;

    attributeFlags = from->attributeFlags;
    specFlags      = from->specFlags;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneHie)
    {
        cloneChilds(context, from);

        // Force semantic on specific nodes on generic instantiation
        if ((from->flags & AST_IS_GENERIC) && (from->semFlags & AST_SEM_ON_CLONE))
        {
            for (auto one : childs)
                one->flags &= ~AST_NO_SEMANTIC;
        }
    }
}

void AstNode::cloneChilds(CloneContext& context, AstNode* from)
{
    auto oldParent = context.parent;
    context.parent = this;
    auto num       = from->childs.size();
    for (int i = 0; i < num; i++)
    {
        // Do not duplicate a struct if it's a child of something else (i.e. another struct), because
        // in case of generics, we do want the normal generic stuff to be done (cloning)
        if (from->childs[i]->kind != AstNodeKind::StructDecl)
            from->childs[i]->clone(context);
    }

    context.parent = oldParent;
}

AstNode* AstNode::clone(CloneContext& context)
{
    switch (kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDeclParam:
        return ((AstVarDecl*) this)->clone(context);
    case AstNodeKind::IdentifierRef:
        return ((AstIdentifierRef*) this)->clone(context);
    case AstNodeKind::Identifier:
    case AstNodeKind::FuncCall:
        return ((AstIdentifier*) this)->clone(context);
    case AstNodeKind::FuncDecl:
        return ((AstFuncDecl*) this)->clone(context);
    case AstNodeKind::AttrDecl:
        return ((AstAttrDecl*) this)->clone(context);
    case AstNodeKind::AttrUse:
        return ((AstAttrUse*) this)->clone(context);
    case AstNodeKind::FuncCallParam:
        return ((AstFuncCallParam*) this)->clone(context);
    case AstNodeKind::BinaryOp:
        return ((AstBinaryOpNode*) this)->clone(context);
    case AstNodeKind::ConditionalExpression:
        return ((AstConditionalOpNode*) this)->clone(context);
    case AstNodeKind::If:
    case AstNodeKind::CompilerIf:
        return ((AstIf*) this)->clone(context);
    case AstNodeKind::Break:
    case AstNodeKind::Continue:
    case AstNodeKind::FallThrough:
        return ((AstBreakContinue*) this)->clone(context);
    case AstNodeKind::LabelBreakable:
        return ((AstLabelBreakable*) this)->clone(context);
    case AstNodeKind::While:
        return ((AstWhile*) this)->clone(context);
    case AstNodeKind::For:
        return ((AstFor*) this)->clone(context);
    case AstNodeKind::Loop:
        return ((AstLoop*) this)->clone(context);
    case AstNodeKind::Visit:
        return ((AstVisit*) this)->clone(context);
    case AstNodeKind::Switch:
        return ((AstSwitch*) this)->clone(context);
    case AstNodeKind::SwitchCase:
        return ((AstSwitchCase*) this)->clone(context);
    case AstNodeKind::SwitchCaseBlock:
        return ((AstSwitchCaseBlock*) this)->clone(context);
    case AstNodeKind::TypeExpression:
        return ((AstTypeExpression*) this)->clone(context);
    case AstNodeKind::TypeLambda:
        return ((AstTypeLambda*) this)->clone(context);
    case AstNodeKind::ArrayPointerSlicing:
        return ((AstArrayPointerSlicing*) this)->clone(context);
    case AstNodeKind::ArrayPointerIndex:
        return ((AstArrayPointerIndex*) this)->clone(context);
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
        return ((AstStruct*) this)->clone(context);
    case AstNodeKind::Impl:
        return ((AstImpl*) this)->clone(context);
    case AstNodeKind::EnumDecl:
        return ((AstEnum*) this)->clone(context);
    case AstNodeKind::EnumValue:
        return ((AstEnumValue*) this)->clone(context);
    case AstNodeKind::Init:
        return ((AstInit*) this)->clone(context);
    case AstNodeKind::Drop:
    case AstNodeKind::PostCopy:
    case AstNodeKind::PostMove:
        return ((AstDropCopyMove*) this)->clone(context);
    case AstNodeKind::Return:
        return ((AstReturn*) this)->clone(context);
    case AstNodeKind::CompilerInline:
        return ((AstCompilerInline*) this)->clone(context);
    case AstNodeKind::CompilerMacro:
        return ((AstCompilerMacro*) this)->clone(context);
    case AstNodeKind::CompilerMixin:
        return ((AstCompilerMixin*) this)->clone(context);
    case AstNodeKind::Inline:
        return ((AstInline*) this)->clone(context);
    case AstNodeKind::CompilerIfBlock:
        return ((AstCompilerIfBlock*) this)->clone(context);
    case AstNodeKind::CompilerRun:
    case AstNodeKind::CompilerSelectIf:
    case AstNodeKind::CompilerCheckIf:
    case AstNodeKind::CompilerAst:
        return ((AstCompilerSpecFunc*) this)->clone(context);
    case AstNodeKind::Namespace:
        return ((AstNameSpace*) this)->clone(context);
    case AstNodeKind::Try:
    case AstNodeKind::Catch:
    case AstNodeKind::Assume:
    case AstNodeKind::Throw:
        return ((AstTryCatchAssume*) this)->clone(context);
    case AstNodeKind::Alias:
        return ((AstAlias*) this)->clone(context);
    case AstNodeKind::Cast:
    case AstNodeKind::AutoCast:
    case AstNodeKind::BitCast:
        return ((AstCast*) this)->clone(context);
    case AstNodeKind::FuncCallParams:
        return ((AstFuncCallParams*) this)->clone(context);
    case AstNodeKind::Range:
        return ((AstRange*) this)->clone(context);
    case AstNodeKind::MakePointerLambda:
        return ((AstMakePointerLambda*) this)->clone(context);

    default:
    {
        auto newNode = Ast::newNode<AstNode>();
        if (flags & AST_NEED_SCOPE)
        {
            auto cloneContext        = context;
            cloneContext.parentScope = Ast::newScope(newNode, newNode->token.text, ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
            newNode->copyFrom(cloneContext, this);
            context.propageResult(cloneContext);
        }
        else
            newNode->copyFrom(context, this);
        return newNode;
    }
    }
}

AstNode* AstVarDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstVarDecl>();
    newNode->copyFrom(context, this);
    newNode->publicName = publicName;
    newNode->attributes = attributes;

    newNode->type       = (AstTypeExpression*) findChildRef(type, newNode);
    newNode->assignment = findChildRef(assignment, newNode);

    // Is there an alias ?
    auto it = context.replaceNames.find(newNode->token.text);
    if (it != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(it->second);
        newNode->token.text = it->second;
    }

    return newNode;
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIdentifierRef>();
    newNode->copyFrom(context, this);
    return newNode;
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

    auto idRef = context.parent;
    while (idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->parent;
    newNode->identifierRef     = CastAst<AstIdentifierRef>(idRef, AstNodeKind::IdentifierRef);
    newNode->callParameters    = (AstFuncCallParams*) findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);
    newNode->aliasNames        = aliasNames;

    // Check if we need to replace the token.text with a type substitution
    // That way the new resolveIdentifier will just try to keep the typeinfo
    auto it = context.replaceTypes.find(newNode->token.text);
    if (it != context.replaceTypes.end())
    {
        if (!it->second->isNative(NativeTypeKind::Undefined))
            newNode->token.text = it->second->name;
        newNode->typeInfo = it->second;
        if (newNode->typeInfo->declNode)
        {
            newNode->resolvedSymbolName     = newNode->typeInfo->declNode->resolvedSymbolName;
            newNode->resolvedSymbolOverload = newNode->typeInfo->declNode->resolvedSymbolOverload;
        }

        newNode->flags |= AST_FROM_GENERIC_REPLACE;
        newNode->flags |= AST_FROM_GENERIC;
    }

    return newNode;
}

bool AstFuncDecl::cloneSubDecls(JobContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode)
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
            nodeFunc->allocateExtension();
            if (cloneContext.alternativeScope)
                nodeFunc->extension->alternativeScopes.push_back(cloneContext.alternativeScope);
            symKind = SymbolKind::Function;
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

        subDecl->typeInfo->flags &= ~TYPEINFO_GENERIC;
        subDecl->typeInfo->declNode = subDecl;

        subDecl->doneFlags |= AST_DONE_FILE_JOB_PASS;
        newFctNode->subDecls.push_back(subDecl);

        // Be sure symbol is not already there. This can happen when using mixins
        if (context)
        {
            auto sym = subFuncScope->symTable.find(subDecl->token.text);
            if (sym)
            {
                Diagnostic diag{subDecl, subDecl->token, Utf8::format(Msg0346, subDecl->token.text.c_str())};
                return context->report(diag);
            }
        }

        // We need to add the parent scope of the inline function (the global one), in order for
        // the inline content to be resolved in the same context as the original function
        auto globalScope = f->ownerScope;
        while (!globalScope->isGlobalOrImpl())
            globalScope = globalScope->parentScope;
        subDecl->allocateExtension();
        subDecl->extension->alternativeScopes.push_back(globalScope);

        subDecl->resolvedSymbolName     = subFuncScope->symTable.registerSymbolName(nullptr, subDecl, symKind);
        subDecl->resolvedSymbolOverload = nullptr;

        // Do it last to avoid a race condition with the file job
        Ast::addChildBack(f->parent, subF);
    }

    return true;
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFuncDecl>();

    newNode->copyFrom(context, this, false);
    newNode->aliasMask   = aliasMask;
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;

    auto cloneContext     = context;
    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto functionScope    = Ast::newScope(newNode, newNode->token.text, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = functionScope;

    cloneContext.parentScope   = functionScope;
    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->selectIf          = selectIf ? selectIf->clone(cloneContext) : nullptr;

    //cloneContext.parentScope = context.parentScope;
    newNode->returnType = returnType ? returnType->clone(cloneContext) : nullptr;
    if (newNode->returnType)
        newNode->returnType->ownerScope = context.parentScope ? context.parentScope : ownerScope;

    if (content)
    {
        auto bodyScope           = Ast::newScope(newNode, newNode->token.text, ScopeKind::FunctionBody, functionScope);
        cloneContext.parentScope = bodyScope;
        newNode->content         = content->clone(cloneContext);
        bodyScope->owner         = newNode->content;
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

    newNode->namedParam        = namedParam;
    newNode->namedParamNode    = namedParamNode;
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

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    // Do the token replacement only if this is a break/continue in the original breakable,
    // not a 'new' breakable in the user code
    if (context.ownerBreakable == context.replaceTokensBreakable)
    {
        auto it = context.replaceTokens.find(token.id);
        if (it != context.replaceTokens.end())
        {
            CloneContext cloneContext = context;
            cloneContext.replaceTokens.clear();
            auto result = it->second->clone(cloneContext);
            context.propageResult(cloneContext);
            return result;
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

AstNode* AstLabelBreakable::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstLabelBreakable>();
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

    newNode->identifier  = findChildRef(identifier, newNode);
    newNode->literalType = literalType;
    newNode->ptrCount    = ptrCount;

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

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstStruct>();
    newNode->copyFrom(context, this, false);
    newNode->packing     = packing;
    newNode->structFlags = structFlags;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;

    newNode->scope = cloneContext.parentScope;
    newNode->allocateExtension();
    newNode->extension->alternativeScopes.push_back(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;
    newNode->flags |= AST_FROM_GENERIC;
    newNode->content->flags &= ~AST_NO_SEMANTIC;

    if (newNode->typeInfo)
    {
        newNode->typeInfo = newNode->typeInfo->clone();
        newNode->typeInfo->flags &= ~TYPEINFO_GENERIC;
        newNode->typeInfo->declNode = newNode;
        newNode->typeInfo->forceComputeName();
    }

    context.propageResult(cloneContext);
    return newNode;
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

    for (auto c : childs)
    {
        auto newChild = c->clone(cloneContext);

        if (newChild->kind == AstNodeKind::FuncDecl)
        {
            auto newFunc = CastAst<AstFuncDecl>(newChild, AstNodeKind::FuncDecl);

            // Resolution of 'impl' needs all functions to have their symbol registered in the 'impl' scope,
            // in order to be able to wait for the resolution of all function before solving the 'impl'
            // implementation.
            newNode->scope->symTable.registerSymbolName(nullptr, newFunc, SymbolKind::Function);

            // The function inside the implementation was generic, so be sure we can now solve
            // its content
            newFunc->content->flags &= ~AST_NO_SEMANTIC;

            // Be sure we have a specific no generic typeinfo
            SWAG_ASSERT(newFunc->typeInfo);
            newFunc->typeInfo = newFunc->typeInfo->clone();
            newFunc->typeInfo->flags &= ~TYPEINFO_GENERIC;
            newFunc->typeInfo->declNode = newFunc;
            newFunc->typeInfo->forceComputeName();
        }
    }

    newNode->identifier    = findChildRef(identifier, newNode);
    newNode->identifierFor = findChildRef(identifierFor, newNode);

    // Cloning an impl block should be called only for generic interface implementation
    SWAG_ASSERT(newNode->identifierFor);

    context.propageResult(cloneContext);
    return newNode;
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
    newNode->typeInfo->flags &= ~TYPEINFO_GENERIC;
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
    if (context.rawClone)
        newNode->semFlags |= semFlags & AST_SEM_EMBEDDED_RETURN;

    return newNode;
}

AstNode* AstCompilerInline::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerInline>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = cloneContext.parentScope;
    childs.back()->clone(cloneContext);
    context.propageResult(cloneContext);

    return newNode;
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

AstNode* AstInline::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstInline>();
    newNode->copyFrom(context, this, false);
    newNode->func = func;

    // Is this correct ? Seems a little wierd, but that way we do not have to copy the parametersScope
    // content, which is not really possible for now (12/07/2021) (i.e. no way to copy already registered symbols in the scope).
    // Because it can happen that an inline block already solved is copied.
    // For example because of createTmpVarStruct, with inline calls as parameters: titi(A{round(6)}) => round already inlined.
    // I guess one day this will hit me in the face...
    newNode->parametersScope = parametersScope;
    //newNode->parametersScope = Ast::newScope(newNode, "", ScopeKind::Statement, nullptr);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.ownerInline = newNode;
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

    SWAG_ASSERT(symbols.empty());
    SWAG_ASSERT(interfacesCount.empty());
    SWAG_ASSERT(methodsCount.empty());

    if (newNode->ownerCompilerIfBlock)
        newNode->ownerCompilerIfBlock->blocks.push_back(newNode);
    newNode->numTestErrors   = numTestErrors;
    newNode->numTestWarnings = numTestWarnings;
    return newNode;
}

AstNode* AstCompilerSpecFunc::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerSpecFunc>();
    newNode->copyFrom(context, this, false);

    // Clone childs by hand, because a compiler block can contain a sub function, and this sub
    // function shouldn't have the ownerInline set (if the #ast is in an inline block)
    auto cloneContext   = context;
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

    newNode->embeddedKind = embeddedKind;

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

AstNode* AstMakePointerLambda::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstMakePointerLambda>();
    newNode->copyFrom(context, this);
    newNode->lambda = lambda;
    return newNode;
}