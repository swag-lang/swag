#include "pch.h"
#include "Pool.h"
#include "AstNode.h"
#include "Scope.h"

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
    assert(ownerScope);
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
    case AstNodeKind::TypeDecl:
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
    case AstNodeKind::TypeDecl:
        return "type";
    case AstNodeKind::FuncDeclParam:
        return "parameter";
    }

    return "something else";
}

AstNode* AstNode::clone()
{
    auto result = g_Pool_astNode.alloc();
    result->copyFrom(this);
    return result;
}

AstNode* AstNode::findChildRef(AstNode* ref, AstNode* fromChild)
{
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i] == ref)
            return fromChild->childs[i];
    }

    return nullptr;
}

void AstNode::copyFrom(AstNode* from)
{
    ownerScope     = from->ownerScope;
    ownerBreakable = from->ownerBreakable;
    ownerFct       = from->ownerFct;
    ownerFlags     = from->ownerFlags;

    typeInfo               = from->typeInfo;
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

    kind             = from->kind;
    flags            = from->flags;
    computedValue    = from->computedValue;
    name             = from->name;
    fullname         = from->fullname;
    sourceFileIdx    = from->sourceFileIdx;
    bc               = from->bc;
    resultRegisterRC = from->resultRegisterRC;

    childs = from->childs;
    for (int i = 0; i < childs.size(); i++)
        childs[i] = childs[i]->clone();
}

AstNode* AstIdentifierRef::clone()
{
    auto newNode        = g_Pool_astIdentifierRef.alloc();
    newNode->startScope = startScope;
    return newNode;
}

AstNode* AstIdentifier::clone()
{
    auto newNode                  = g_Pool_astIdentifier.alloc();
    newNode->fctCallStorageOffset = fctCallStorageOffset;
    newNode->callParameters       = findChildRef(callParameters, newNode);

    // Find the parent identifierRef
    newNode->identifierRef = static_cast<AstIdentifierRef*>(newNode->parent);
    while (newNode->identifierRef->kind != AstNodeKind::IdentifierRef)
        newNode->identifierRef = static_cast<AstIdentifierRef*>(newNode->identifierRef->parent);

    return newNode;
}

AstNode* AstFuncDecl::clone()
{
    auto newNode        = g_Pool_astFuncDecl.alloc();
    newNode->stackSize  = stackSize;
    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    newNode->content    = findChildRef(content, newNode);
    return newNode;
}

AstNode* AstAttrDecl::clone()
{
    auto newNode        = g_Pool_astAttrDecl.alloc();
    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone()
{
    auto newNode    = g_Pool_astAttrUse.alloc();
    newNode->values = values;
    return newNode;
}

AstNode* AstFuncCallParam::clone()
{
    auto newNode                = g_Pool_astFuncCallParam.alloc();
    newNode->namedParam         = namedParam;
    newNode->namedParamNode     = namedParamNode;
    newNode->resolvedParameter  = resolvedParameter;
    newNode->index              = index;
    newNode->mustSortParameters = mustSortParameters;
    return newNode;
}

AstNode* AstIf::clone()
{
    auto newNode            = g_Pool_astIf.alloc();
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = findChildRef(ifBlock, newNode);
    newNode->elseBlock      = findChildRef(elseBlock, newNode);
    return newNode;
}

void AstBreakable::copyFrom(AstNode* from)
{
    auto fromBreakeable = (AstBreakable*) from;
    breakableFlags      = fromBreakeable->breakableFlags;
    registerIndex       = fromBreakeable->registerIndex;

    parentBreakable = findChildRef(fromBreakeable->parentBreakable, this);
    for (auto expr : fromBreakeable->breakList)
        breakList.push_back((AstBreakContinue*) findChildRef(expr, this));
    for (auto expr : fromBreakeable->continueList)
        continueList.push_back((AstBreakContinue*) findChildRef(expr, this));
}

AstNode* AstBreakContinue::clone()
{
    auto newNode = g_Pool_astBreakContinue.alloc();
    return newNode;
}

AstNode* AstWhile::clone()
{
    auto newNode = g_Pool_astWhile.alloc();
    newNode->AstBreakable::copyFrom(this);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstFor::clone()
{
    auto newNode = g_Pool_astFor.alloc();
    newNode->AstBreakable::copyFrom(this);
    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstLoop::clone()
{
    auto newNode = g_Pool_astLoop.alloc();
    newNode->AstBreakable::copyFrom(this);
    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstSwitch::clone()
{
    auto newNode = g_Pool_astSwitch.alloc();
    newNode->AstBreakable::copyFrom(this);
    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = (AstSwitch*) findChildRef(block, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone()
{
    auto newNode = g_Pool_astSwitchCase.alloc();
    newNode->copyFrom(this);
    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = (AstSwitch*) findChildRef(ownerSwitch, newNode);
    newNode->isDefault   = isDefault;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCaseBlock::clone()
{
    auto newNode = g_Pool_astSwitchCaseBlock.alloc();
    newNode->copyFrom(this);
    newNode->ownerCase = (AstSwitchCase*) findChildRef(ownerCase, newNode);
    newNode->isDefault = isDefault;
    return newNode;
}

AstNode* AstTypeExpression::clone()
{
    auto newNode = g_Pool_astTypeExpression.alloc();
    newNode->copyFrom(this);
    newNode->typeExpression = findChildRef(typeExpression, newNode);
    newNode->ptrCount       = ptrCount;
    newNode->arrayDim       = arrayDim;
    newNode->isSlice        = isSlice;
    newNode->isConst        = isConst;
    return newNode;
}

AstNode* AstTypeLambda::clone()
{
    auto newNode = g_Pool_astTypeLambda.alloc();
    newNode->copyFrom(this);
    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstPointerDeRef::clone()
{
    auto newNode = g_Pool_astPointerDeref.alloc();
    newNode->copyFrom(this);
    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    return newNode;
}

AstNode* AstProperty::clone()
{
    auto newNode = g_Pool_astProperty.alloc();
    newNode->copyFrom(this);
    newNode->expression = findChildRef(expression, newNode);
    newNode->prop       = prop;
    return newNode;
}

AstNode* AstExpressionList::clone()
{
    auto newNode = g_Pool_astExpressionList.alloc();
    newNode->copyFrom(this);
    newNode->storageOffset = storageOffset;
    newNode->listKind      = listKind;
    return newNode;
}

AstNode* AstStruct::clone()
{
    auto newNode = g_Pool_astStruct.alloc();
    newNode->copyFrom(this);
    newNode->opInit = opInit;
    return newNode;
}

AstNode* AstImpl::clone()
{
    auto newNode = g_Pool_astImpl.alloc();
    newNode->copyFrom(this);
    newNode->structScope = structScope;
    newNode->identifier  = findChildRef(identifier, newNode);
    return newNode;
}
