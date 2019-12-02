#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"

Pool<AstNode>            g_Pool_astNode;
Pool<AstAttrDecl>        g_Pool_astAttrDecl;
Pool<AstVarDecl>         g_Pool_astVarDecl;
Pool<AstFuncDecl>        g_Pool_astFuncDecl;
Pool<AstIdentifier>      g_Pool_astIdentifier;
Pool<AstIdentifierRef>   g_Pool_astIdentifierRef;
Pool<AstFuncCallParam>   g_Pool_astFuncCallParam;
Pool<AstIf>              g_Pool_astIf;
Pool<AstWhile>           g_Pool_astWhile;
Pool<AstFor>             g_Pool_astFor;
Pool<AstLoop>            g_Pool_astLoop;
Pool<AstSwitch>          g_Pool_astSwitch;
Pool<AstSwitchCase>      g_Pool_astSwitchCase;
Pool<AstSwitchCaseBlock> g_Pool_astSwitchCaseBlock;
Pool<AstBreakContinue>   g_Pool_astBreakContinue;
Pool<AstAttrUse>         g_Pool_astAttrUse;
Pool<AstTypeExpression>  g_Pool_astTypeExpression;
Pool<AstTypeLambda>      g_Pool_astTypeLambda;
Pool<AstPointerDeRef>    g_Pool_astPointerDeref;
Pool<AstProperty>        g_Pool_astProperty;
Pool<AstExpressionList>  g_Pool_astExpressionList;
Pool<AstStruct>          g_Pool_astStruct;
Pool<AstImpl>            g_Pool_astImpl;
Pool<AstInit>            g_Pool_astInit;
Pool<AstDrop>            g_Pool_astDrop;
Pool<AstInline>          g_Pool_astInline;
Pool<AstReturn>          g_Pool_astReturn;
Pool<AstCompilerIfBlock> g_Pool_astCompilerIfBlock;
Pool<AstLabelBreakable>  g_Pool_astLabelBreakable;
Pool<AstCompilerInline>  g_Pool_astCompilerInline;

void AstNode::setPassThrough()
{
    semanticAfterFct  = nullptr;
    semanticBeforeFct = nullptr;
    semanticFct       = nullptr;
    byteCodeAfterFct  = nullptr;
    byteCodeBeforeFct = nullptr;
    byteCodeFct       = ByteCodeGenJob::emitPassThrough;
}

void AstNode::inheritLocationFromChilds()
{
    if (childs.empty())
        return;
    for (auto child : childs)
        child->inheritLocationFromChilds();

    auto front = childs.front();
    if (token.startLocation.column == 0 && token.startLocation.line == 0)
        token.startLocation = front->token.startLocation;
    if (token.startLocation.line != front->token.endLocation.line || token.startLocation.column > front->token.endLocation.column)
        token.startLocation = front->token.startLocation;

    auto back = childs.back();
    if (token.endLocation.line != back->token.endLocation.line || token.endLocation.column < back->token.endLocation.column)
        token.endLocation = back->token.endLocation;
}

void AstNode::computeFullName()
{
    scoped_lock lk(mutex);
    SWAG_ASSERT(ownerScope);
    if (ownerScope->fullname.empty())
        fullnameDot = name;
    else
        fullnameDot = ownerScope->fullname + "." + name;
}

Utf8 AstNode::getKindName(AstNode* node)
{
    Utf8 result = getNakedKindName(node);
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::LetDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::Namespace:
    case AstNodeKind::TypeAlias:
    case AstNodeKind::FuncDeclParam:
    case AstNodeKind::StructDecl:
        return "a " + result;
    case AstNodeKind::EnumDecl:
    case AstNodeKind::EnumValue:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::Impl:
        return "an " + result;
    }

    return "<node>";
}

Utf8 AstNode::getNakedKindName(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::LetDecl:
        if (node->ownerScope && node->ownerScope->isGlobal())
            return "global variable";
        if (node->ownerMainNode && node->ownerMainNode->kind == AstNodeKind::StructDecl)
            return "struct member";
        return "variable";
    case AstNodeKind::ConstDecl:
        return "constant";
    case AstNodeKind::FuncDecl:
        return "function";
    case AstNodeKind::EnumDecl:
        return "enum";
    case AstNodeKind::EnumValue:
        return "enum value";
    case AstNodeKind::Namespace:
        return "namespace";
    case AstNodeKind::TypeAlias:
        return "typealias";
    case AstNodeKind::FuncDeclParam:
        return "parameter";
    case AstNodeKind::StructDecl:
        return "structure";
    case AstNodeKind::InterfaceDecl:
        return "interface";
    case AstNodeKind::Impl:
        return "implementation block";
    }

    return "<node>";
}

AstNode* AstNode::clone(CloneContext& context)
{
    auto result = g_Pool_astNode.alloc();
    result->copyFrom(context, this);
    return result;
}

AstNode* AstNode::findChildRef(AstNode* ref, AstNode* fromChild)
{
    if (!ref)
        return nullptr;
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i] == ref)
            return fromChild->childs[i];
    }

    return nullptr;
}

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneChilds)
{
    kind  = from->kind;
    flags = from->flags;
    flags &= ~AST_IS_GENERIC;

    ownerStructScope = context.ownerStructScope ? context.ownerStructScope : from->ownerStructScope;
    ownerMainNode    = context.ownerMainNode ? context.ownerMainNode : from->ownerMainNode;
    ownerScope       = context.parentScope ? context.parentScope : from->ownerScope;
    ownerBreakable   = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable;
    ownerInline      = context.ownerInline ? context.ownerInline : from->ownerInline;
    ownerFct         = context.ownerFct ? context.ownerFct : from->ownerFct;
    ownerFlags       = from->ownerFlags;

    // Replace a type by another one during generic instantiation
    if (from->typeInfo && context.replaceTypes.size() > 0)
    {
        auto it = context.replaceTypes.find(from->typeInfo);
        if (it != context.replaceTypes.end())
        {
            typeInfo = it->second;
            flags |= AST_FROM_GENERIC;
        }
        else
            typeInfo = from->typeInfo;
    }
    else
    {
        typeInfo = from->typeInfo;
    }

    castedTypeInfo         = from->castedTypeInfo;
    resolvedSymbolName     = from->resolvedSymbolName;
    resolvedSymbolOverload = from->resolvedSymbolOverload;

    parentAttributes = from->parentAttributes;
    parentAttributes = from->parentAttributes;
    attributeFlags   = from->attributeFlags;
    token            = from->token;

    semanticFct       = from->semanticFct;
    semanticBeforeFct = from->semanticBeforeFct;
    semanticAfterFct  = from->semanticAfterFct;
    byteCodeFct       = from->byteCodeFct;
    byteCodeBeforeFct = from->byteCodeBeforeFct;
    byteCodeAfterFct  = from->byteCodeAfterFct;

    computedValue        = from->computedValue;
    name                 = from->name;
    fullnameDot          = from->fullnameDot;
    sourceFile           = from->sourceFile;
    bc                   = from->bc;
    resultRegisterRC     = from->resultRegisterRC;
    fctCallStorageOffset = from->fctCallStorageOffset;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneChilds)
    {
        auto cloneContext   = context;
        cloneContext.parent = this;
        for (int i = 0; i < from->childs.size(); i++)
            from->childs[i]->clone(cloneContext);
    }
}

AstNode* AstVarDecl::clone(CloneContext& context)
{
    auto newNode = g_Pool_astVarDecl.alloc();
    newNode->copyFrom(context, this);

    newNode->type       = findChildRef(type, newNode);
    newNode->assignment = findChildRef(assignment, newNode);
    return newNode;
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = g_Pool_astIdentifierRef.alloc();
    newNode->copyFrom(context, this);
    return newNode;
}

void AstIdentifierRef::computeName()
{
    name.clear();
    for (auto child : childs)
    {
        if (!name.empty())
            name += ".";
        name += child->name;
    }

    name.computeCrc();
}

AstNode* AstIdentifier::clone(CloneContext& context)
{
    auto newNode = g_Pool_astIdentifier.alloc();
    newNode->copyFrom(context, this);

    auto idRef = context.parent;
    while (idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->parent;
    newNode->identifierRef     = CastAst<AstIdentifierRef>(idRef, AstNodeKind::IdentifierRef);
    newNode->callParameters    = findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);
    return newNode;
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode = g_Pool_astFuncDecl.alloc();

    newNode->copyFrom(context, this, false);
    newNode->stackSize = stackSize;

    auto cloneContext     = context;
    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto parentScope      = Ast::newScope(newNode, newNode->name, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = parentScope;

    cloneContext.parentScope   = parentScope;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;

    cloneContext.parentScope = context.parentScope;
    newNode->returnType      = returnType ? returnType->clone(cloneContext) : nullptr;

    cloneContext.parentScope = parentScope;
    newNode->content         = content ? content->clone(cloneContext) : nullptr;

    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    auto newNode = g_Pool_astAttrDecl.alloc();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    auto newNode    = g_Pool_astAttrUse.alloc();
    newNode->values = values;
    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    auto newNode = g_Pool_astFuncCallParam.alloc();
    newNode->copyFrom(context, this);

    newNode->namedParam         = namedParam;
    newNode->namedParamNode     = namedParamNode;
    newNode->resolvedParameter  = resolvedParameter;
    newNode->index              = index;
    newNode->mustSortParameters = mustSortParameters;
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    auto newNode = g_Pool_astIf.alloc();
    newNode->copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = findChildRef(ifBlock, newNode);
    newNode->elseBlock      = findChildRef(elseBlock, newNode);
    return newNode;
}

void AstBreakable::copyFrom(CloneContext& context, AstBreakable* curParentBreakable, AstBreakable* from)
{
    AstNode::copyFrom(context, from);
    breakableFlags  = from->breakableFlags;
    registerIndex   = from->registerIndex;
    parentBreakable = curParentBreakable;
}

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    auto newNode = g_Pool_astBreakContinue.alloc();
    newNode->copyFrom(context, this);

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
    auto newNode = g_Pool_astLabelBreakable.alloc();

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->AstBreakable::copyFrom(cloneContext, context.ownerBreakable, this);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = g_Pool_astWhile.alloc();

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->AstBreakable::copyFrom(cloneContext, context.ownerBreakable, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstFor::clone(CloneContext& context)
{
    auto newNode = g_Pool_astFor.alloc();

    auto cloneContext           = context;
    cloneContext.parentScope    = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerBreakable = newNode;
    newNode->AstBreakable::copyFrom(cloneContext, context.ownerBreakable, this);

    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstLoop::clone(CloneContext& context)
{
    auto newNode = g_Pool_astLoop.alloc();

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->AstBreakable::copyFrom(cloneContext, context.ownerBreakable, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitch.alloc();

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->AstBreakable::copyFrom(cloneContext, context.ownerBreakable, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = findChildRef(block, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitchCase.alloc();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = CastAst<AstSwitch>(context.parent, AstNodeKind::Switch);
    newNode->isDefault   = isDefault;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitchCaseBlock.alloc();
    newNode->copyFrom(context, this);

    newNode->ownerCase = CastAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    newNode->isDefault = isDefault;
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = g_Pool_astTypeExpression.alloc();
    newNode->copyFrom(context, this);

    newNode->identifier     = findChildRef(identifier, newNode);
    newNode->ptrCount       = ptrCount;
    newNode->arrayDim       = arrayDim;
    newNode->isSlice        = isSlice;
    newNode->isConst        = isConst;
    newNode->isCode         = isCode;
    newNode->forceConstType = forceConstType;
    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    auto newNode = g_Pool_astTypeLambda.alloc();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstPointerDeRef::clone(CloneContext& context)
{
    auto newNode = g_Pool_astPointerDeref.alloc();
    newNode->copyFrom(context, this);

    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    return newNode;
}

AstNode* AstProperty::clone(CloneContext& context)
{
    auto newNode = g_Pool_astProperty.alloc();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->prop       = prop;
    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    auto newNode = g_Pool_astExpressionList.alloc();
    newNode->copyFrom(context, this);

    newNode->storageOffset        = storageOffset;
    newNode->storageOffsetSegment = storageOffsetSegment;
    newNode->listKind             = listKind;
    newNode->isConst              = isConst;
    newNode->forceConstType       = forceConstType;

    return newNode;
}

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = g_Pool_astStruct.alloc();
    newNode->copyFrom(context, this, false);
    newNode->packing = packing;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->name, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    cloneContext.ownerMainNode    = newNode;

    newNode->scope = cloneContext.parentScope;
    newNode->alternativeScopes.push_back(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;

    return newNode;
}

AstNode* AstImpl::clone(CloneContext& context)
{
    SWAG_ASSERT(false);
    return nullptr;
}

AstNode* AstInit::clone(CloneContext& context)
{
    auto newNode = g_Pool_astInit.alloc();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstDrop::clone(CloneContext& context)
{
    auto newNode = g_Pool_astDrop.alloc();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    auto newNode = g_Pool_astReturn.alloc();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstCompilerInline::clone(CloneContext& context)
{
    auto newNode = g_Pool_astCompilerInline.alloc();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", token.id == TokenId::CompilerInline ? ScopeKind::Inline : ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    childs.back()->clone(cloneContext);

    return newNode;
}

AstNode* AstInline::clone(CloneContext& context)
{
    auto newNode = g_Pool_astInline.alloc();
    newNode->copyFrom(context, this, false);
    newNode->func = func;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);

    newNode->scope = cloneContext.parentScope;
    func->content->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerIfBlock::clone(CloneContext& context)
{
    auto newNode = g_Pool_astCompilerIfBlock.alloc();
    newNode->copyFrom(context, this);
    return newNode;
}