#include "pch.h"
#include "Pool.h"
#include "Ast.h"
#include "Scope.h"
#include "Scoped.h"

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

void AstNode::computeFullName()
{
    SWAG_ASSERT(ownerScope);
    fullname = ownerScope->fullname + "_" + name;
    replaceAll(fullname, '.', '_');
}

const char* AstNode::getKindName(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
        return "a variable";
    case AstNodeKind::FuncDecl:
        return "a function";
    case AstNodeKind::EnumDecl:
        return "an enum";
    case AstNodeKind::EnumValue:
        return "an enum value";
    case AstNodeKind::Namespace:
        return "a namespace";
    case AstNodeKind::TypeAlias:
        return "a type";
    case AstNodeKind::FuncDeclParam:
        return "a function parameter";
    }

    return "something else";
}

const char* AstNode::getNakedKindName(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
        return "variable";
    case AstNodeKind::FuncDecl:
        return "function";
    case AstNodeKind::EnumDecl:
        return "enum";
    case AstNodeKind::EnumValue:
        return "enum value";
    case AstNodeKind::Namespace:
        return "namespace";
    case AstNodeKind::TypeAlias:
        return "type";
    case AstNodeKind::FuncDeclParam:
        return "parameter";
    }

    return "something else";
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

    ownerScopeStruct = context.ownerScopeStruct ? context.ownerScopeStruct : from->ownerScopeStruct;
    ownerScope       = context.parentScope ? context.parentScope : from->ownerScope;
    ownerBreakable   = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable;
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
    fullname             = from->fullname;
    sourceFileIdx        = from->sourceFileIdx;
    bc                   = from->bc;
    resultRegisterRC     = from->resultRegisterRC;
    fctCallStorageOffset = from->fctCallStorageOffset;

    parent = context.parent;
    if (parent)
        Ast::addChild(parent, this);

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

AstNode* AstIdentifier::clone(CloneContext& context)
{
    auto newNode = g_Pool_astIdentifier.alloc();
    newNode->copyFrom(context, this);

    newNode->identifierRef     = CastAst<AstIdentifierRef>(context.parent, AstNodeKind::IdentifierRef);
    newNode->callParameters    = findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);
    return newNode;
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode = g_Pool_astFuncDecl.alloc();

    newNode->copyFrom(context, this, false);
    newNode->stackSize = stackSize;

    auto cloneContext        = context;
    cloneContext.ownerFct    = newNode;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, newNode->name, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.parentScope->allocateSymTable();

    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;

    cloneContext.parentScope = context.parentScope;
    newNode->returnType      = returnType ? returnType->clone(cloneContext) : nullptr;

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

void AstBreakable::copyFrom(CloneContext& context, AstNode* from)
{
    AstNode::copyFrom(context, from);

    auto fromBreakeable = (AstBreakable*) from;
    breakableFlags      = fromBreakeable->breakableFlags;
    registerIndex       = fromBreakeable->registerIndex;

    parentBreakable = findChildRef(fromBreakeable->parentBreakable, this);
    for (auto expr : fromBreakeable->breakList)
        breakList.push_back((AstBreakContinue*) findChildRef(expr, this));
    for (auto expr : fromBreakeable->continueList)
        continueList.push_back((AstBreakContinue*) findChildRef(expr, this));
}

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    auto newNode = g_Pool_astBreakContinue.alloc();
    newNode->copyFrom(context, this);

    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = g_Pool_astWhile.alloc();
    newNode->AstBreakable::copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstFor::clone(CloneContext& context)
{
    auto newNode = g_Pool_astFor.alloc();
    newNode->AstBreakable::copyFrom(context, this);

    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstLoop::clone(CloneContext& context)
{
    auto newNode = g_Pool_astLoop.alloc();
    newNode->AstBreakable::copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitch.alloc();
    newNode->AstBreakable::copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = (AstSwitch*) findChildRef(block, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitchCase.alloc();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = (AstSwitch*) findChildRef(ownerSwitch, newNode);
    newNode->isDefault   = isDefault;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    auto newNode = g_Pool_astSwitchCaseBlock.alloc();
    newNode->copyFrom(context, this);

    newNode->ownerCase = (AstSwitchCase*) findChildRef(ownerCase, newNode);
    newNode->isDefault = isDefault;
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = g_Pool_astTypeExpression.alloc();
    newNode->copyFrom(context, this);

    newNode->identifier = findChildRef(identifier, newNode);
    newNode->ptrCount   = ptrCount;
    newNode->arrayDim   = arrayDim;
    newNode->isSlice    = isSlice;
    newNode->isConst    = isConst;
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

    newNode->storageOffset = storageOffset;
    newNode->listKind      = listKind;
    return newNode;
}

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = g_Pool_astStruct.alloc();
    newNode->copyFrom(context, this, false);
    newNode->defaultOpInit = defaultOpInit;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, newNode->name, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.parentScope->allocateSymTable();
    cloneContext.ownerScopeStruct = cloneContext.parentScope;

    newNode->scope = cloneContext.parentScope;
    newNode->scope->alternativeScopes.push_back(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;

    return newNode;
}

AstNode* AstImpl::clone(CloneContext& context)
{
    SWAG_ASSERT(false);
    return nullptr;
}
