#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"
#include "Scoped.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Module.h"
#include "LanguageSpec.h"

bool SyntaxJob::doAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Alias, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveUsing;
    if (result)
        *result = node;
    SWAG_CHECK(tokenizer.getToken(token));

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid alias name '%s'", token.text.c_str())));
    node->inheritTokenName(token);
    SWAG_CHECK(isValidUserName(node));

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymEqual));
    AstNode* expr;
    SWAG_CHECK(doPrimaryExpression(node, &expr));

    SWAG_CHECK(eatSemiCol("after alias"));

    // This is a type alias
    if (expr->kind == AstNodeKind::TypeExpression || expr->kind == AstNodeKind::TypeLambda)
    {
        node->semanticFct = SemanticJob::resolveTypeAlias;
        currentScope->symTable.registerSymbolName(&context, node, SymbolKind::TypeAlias);
    }
    else
    {
        node->semanticFct        = SemanticJob::resolveAlias;
        node->resolvedSymbolName = currentScope->symTable.registerSymbolName(&context, node, SymbolKind::Alias);
    }

    return true;
}

bool SyntaxJob::doTypeExpressionLambda(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveTypeLambda;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto params      = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters = params;
        while (true)
        {
            bool isConst = false;
            if (token.id == TokenId::KwdConst)
            {
                isConst = true;
                SWAG_CHECK(eatToken());
            }

            if (token.text == "self")
            {
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(currentScope->kind == ScopeKind::Struct, sourceFile->report({sourceFile, "invalid 'self' usage in that context"}));
                auto typeNode        = Ast::newTypeExpression(sourceFile, params);
                typeNode->ptrCount   = 1;
                typeNode->isConst    = isConst;
                typeNode->isSelf     = true;
                typeNode->identifier = Ast::newIdentifierRef(sourceFile, currentScope->name, typeNode, this);
            }
            else
            {
                AstNode* typeExpr;
                SWAG_CHECK(doTypeExpression(params, &typeExpr));
                ((AstTypeExpression*) typeExpr)->isConst = isConst;
            }

            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(node, &node->returnType));
    }

    return true;
}

bool SyntaxJob::convertExpressionListToStruct(AstNode* parent, AstNode** result, bool isConst)
{
    auto structNode = Ast::newStructDecl(sourceFile, nullptr, this);

    auto contentNode               = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode, this);
    structNode->content            = contentNode;
    contentNode->semanticBeforeFct = SemanticJob::preResolveStruct;

    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    Utf8 name = "__" + sourceFile->scopePrivate->name + "_tuple_";
    int  idx  = 0;
    while (token.id != TokenId::EndOfFile)
    {
        auto structFieldNode = Ast::newVarDecl(sourceFile, "", contentNode, nullptr);
        structFieldNode->flags |= AST_GENERATED;

        AstTypeExpression* typeExpression = nullptr;
        AstNode*           expression;
        SWAG_CHECK(doTypeExpression(nullptr, &expression));

        // Name
        if (token.id == TokenId::SymColon)
        {
            typeExpression = (AstTypeExpression*) expression;
            if (!typeExpression->identifier || typeExpression->identifier->kind != AstNodeKind::IdentifierRef || typeExpression->identifier->childs.size() != 1)
                return sourceFile->report({expression, format("invalid named field '%s'", token.text.c_str())});
            structFieldNode->name = typeExpression->identifier->childs.front()->name;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(structFieldNode, &structFieldNode->type));
            expression = structFieldNode->type;
            name += structFieldNode->name;
            name += "_";
        }
        else
        {
            Ast::addChildBack(structFieldNode, expression);
            structFieldNode->type = expression;
            structFieldNode->name = format("item%u", idx);
            structFieldNode->flags |= AST_AUTO_NAME;
        }

        idx++;

        // Name
        typeExpression = (AstTypeExpression*) expression;
        for (int i = 0; i < typeExpression->ptrCount; i++)
            name += "*";
        name += typeExpression->token.text;
        if (typeExpression->identifier)
            name += typeExpression->identifier->childs.back()->name;
        Ast::normalizeIdentifierName(name);

        SWAG_VERIFY(token.id == TokenId::SymComma || token.id == TokenId::SymRightCurly, syntaxError(token, format("invalid token '%s' in tuple type, ',' or '}' are expected here", token.text.c_str())));
        if (token.id == TokenId::SymRightCurly)
            break;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly, "after tuple type expression"));

    // Compute structure name
    structNode->name = move(name);

    // Reference to that struct
    auto identifier = Ast::newIdentifierRef(sourceFile, structNode->name, parent, this);
    if (result)
        *result = identifier;

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->fromTests)
        rootScope = sourceFile->scopePrivate;
    else
        rootScope = sourceFile->module->scopeRoot;

    scoped_lock lk(rootScope->symTable.mutex);
    auto        symbol = rootScope->symTable.findNoLock(structNode->name);
    if (!symbol)
    {
        auto typeInfo        = g_Allocator.alloc<TypeInfoStruct>();
        auto newScope        = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, rootScope, true);
        typeInfo->declNode   = structNode;
        typeInfo->name       = structNode->name;
        typeInfo->nakedName  = structNode->name;
        typeInfo->structName = structNode->name;
        typeInfo->scope      = newScope;
        typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
        structNode->typeInfo   = typeInfo;
        structNode->scope      = newScope;
        structNode->ownerScope = newScope->parentScope;
        rootScope->symTable.registerSymbolNameNoLock(&context, structNode, SymbolKind::Struct);

        Ast::addChildBack(sourceFile->astRoot, structNode);
        structNode->inheritOwners(sourceFile->astRoot);

        Ast::visit(structNode->content, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
        });
    }

    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
    // Code
    if (token.id == TokenId::KwdCode)
    {
        auto node = Ast::newTypeExpression(sourceFile, parent, this);
        if (result)
            *result = node;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeInfo = g_TypeMgr.typeInfoCode;
        node->isCode   = true;
        SWAG_CHECK(eatToken());
        return true;
    }

    // This is a function
    if (token.id == TokenId::KwdFunc)
    {
        SWAG_CHECK(eatToken());
        return doTypeExpressionLambda(parent, result);
    }

    // Const keyword
    bool isConst = false;
    if (token.id == TokenId::KwdConst)
    {
        isConst = true;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    // Else this is a type expression
    auto node = Ast::newTypeExpression(sourceFile, parent, this);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE_CHILDS;
    node->isConst  = isConst;
    node->arrayDim = 0;
    node->ptrCount = 0;
    node->isRef    = false;

    // This is a @typeof
    if (token.id == TokenId::IntrinsicTypeOf || token.id == TokenId::IntrinsicKindOf)
    {
        AstNode* typeOfNode = nullptr;
        SWAG_CHECK(doIdentifierRef(node, &typeOfNode));
        node->isTypeOf          = true;
        auto typeNode           = CastAst<AstIntrinsicProp>(typeOfNode->childs.front(), AstNodeKind::IntrinsicProp);
        typeNode->typeOfAsType  = true;
        typeNode->typeOfAsConst = isConst;
        return true;
    }

    // Array
    if (token.id == TokenId::SymLeftSquare)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        while (true)
        {
            // Size of array can be nothing
            if (token.id == TokenId::SymRightSquare)
            {
                node->arrayDim = UINT32_MAX;
                break;
            }

            // Slice
            if (token.id == TokenId::SymDotDot)
            {
                node->isSlice = true;
                SWAG_CHECK(tokenizer.getToken(token));
                break;
            }

            node->arrayDim++;
            SWAG_CHECK(doExpression(node));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(tokenizer.getToken(token));
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    // Pointers
    while (token.id == TokenId::SymAsterisk)
    {
        node->ptrCount++;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    if (token.id == TokenId::SymAmpersand)
    {
        node->isRef = true;
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_VERIFY(!node->ptrCount, syntaxError(token, "'&' is invalid for a pointer"));
    }

    if (token.id == TokenId::NativeType)
    {
        node->token = move(token);
        SWAG_CHECK(tokenizer.getToken(token));
        return true;
    }

    if (token.id == TokenId::Identifier)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier));
        if (inTypeVarDecl)
            node->identifier->childs.back()->flags |= AST_IN_TYPE_VAR_DECLARATION;
        return true;
    }
    else if (token.id == TokenId::KwdStruct)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(convertExpressionListToStruct(node, &node->identifier, isConst));
        return true;
    }
    else if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(convertExpressionListToStruct(node, &node->identifier, isConst));
        return true;
    }

    return syntaxError(token, format("invalid type declaration '%s'", token.text.c_str()));
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Cast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitCast;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'cast'"));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after type expression"));

    SWAG_CHECK(doUnaryExpression(node));
    return true;
}

bool SyntaxJob::doAutoCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstNode>(this, AstNodeKind::AutoCast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitAutoCast;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doUnaryExpression(node));
    return true;
}
