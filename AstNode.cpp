#include "pch.h"
#include "Pool.h"
#include "AstNode.h"

Pool<AstNode>            g_Pool_astNode;
Pool<AstAttrDecl>        g_Pool_astAttrDecl;
Pool<AstVarDecl>         g_Pool_astVarDecl;
Pool<AstFuncDecl>        g_Pool_astFuncDecl;
Pool<AstIdentifier>      g_Pool_astIdentifier;
Pool<AstIdentifierRef>   g_Pool_astIdentifierRef;
Pool<AstFuncCallParam>   g_Pool_astFuncCallOneParam;
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

AstNode* AstNode::clone()
{
    auto result = g_Pool_astNode.alloc();
    result->copyFrom(this);
    return result;
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
    sourceFileIdx    = from->sourceFileIdx;
    bc               = from->bc;
    resultRegisterRC = from->resultRegisterRC;

    childs = from->childs;
    for (int i = 0; i < childs.size(); i++)
        childs[i] = childs[i]->clone();
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

AstNode* AstExpressionList::clone()
{
    auto newNode = g_Pool_astExpressionList.alloc();
    newNode->copyFrom(this);
    newNode->storageOffset = storageOffset;
    newNode->listKind      = listKind;
    return newNode;
}