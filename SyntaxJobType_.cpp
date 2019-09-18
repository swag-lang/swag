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

bool SyntaxJob::doTypeAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::TypeAlias, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveTypeAlias;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid type name '%s'", token.text.c_str())));
    node->inheritTokenName(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatSemiCol("after type alias"));

    currentScope->allocateSymTable();
    if (!isContextDisabled())
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, node, SymbolKind::TypeAlias);

    return true;
}

bool SyntaxJob::doTypeExpressionLambda(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astTypeLambda, AstNodeKind::TypeLambda, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveTypeLambda;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto params      = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, node);
        node->parameters = params;
        while (true)
        {
			AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(nullptr, &typeExpression));

			if (token.id == TokenId::SymColon)
            {
                if (typeExpression->childs.size() != 1 || typeExpression->childs.front()->kind != AstNodeKind::IdentifierRef)
                    return sourceFile->report({sourceFile, typeExpression, format("invalid named type '%s'", token.text.c_str())});
                auto name = typeExpression->childs.front()->childs.front()->name;
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doTypeExpression(params, &typeExpression));
                typeExpression->name = name;
            }
            else
            {
                Ast::addChildBack(params, typeExpression);
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

bool SyntaxJob::doTypeExpressionTuple(AstNode* parent, AstNode** result, bool isConst)
{
    // Else this is a type expression
    auto node         = Ast::newNode(this, &g_Pool_astExpressionList, AstNodeKind::ExpressionList, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveTypeTuple;
    if (result)
        *result = node;
    node->isConst = isConst;

    SWAG_CHECK(eatToken());

    auto newScope = Ast::newScope(nullptr, "", ScopeKind::TypeList, currentScope);
    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::SymRightCurly)
        {
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(nullptr, &typeExpression));

            if (token.id == TokenId::SymColon)
            {
                if (typeExpression->childs.size() != 1 || typeExpression->childs.front()->kind != AstNodeKind::IdentifierRef)
                    return sourceFile->report({sourceFile, typeExpression, format("invalid named type '%s'", token.text.c_str())});
                auto name = typeExpression->childs.front()->childs.front()->name;
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doTypeExpression(node, &typeExpression));
                typeExpression->name = name;
            }
            else
            {
                Ast::addChildBack(node, typeExpression);
            }

            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly, "after tuple type expression"));
    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result)
{
    // ...
    if (token.id == TokenId::SymDotDotDot)
    {
        auto node         = Ast::newNode(this, &g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, parent);
        node->semanticFct = &SemanticJob::resolveTypeExpression;
        if (result)
            *result = node;
        node->token.literalType = g_TypeMgr.typeInfoVariadic;
        SWAG_CHECK(tokenizer.getToken(token));
        return true;
    }

    // This is a function
    if (token.id == TokenId::SymLeftParen)
        return doTypeExpressionLambda(parent, result);

    // Const keyword
    bool isConst = false;
    if (token.id == TokenId::KwdConst)
    {
        isConst = true;
        SWAG_CHECK(tokenizer.getToken(token));
    }

    // This is a tuple
    if (token.id == TokenId::SymLeftCurly)
        return doTypeExpressionTuple(parent, result, isConst);

    // Else this is a type expression
    auto node         = Ast::newNode(this, &g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveTypeExpression;
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE_CHILDS;
    node->isConst = isConst;

    // Array
    node->arrayDim = 0;
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
    node->ptrCount = 0;
    while (token.id == TokenId::SymAsterisk)
    {
        node->ptrCount++;
        SWAG_CHECK(tokenizer.getToken(token));
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
        return true;
    }

    return syntaxError(token, format("invalid type declaration '%s'", token.text.c_str()));
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Cast, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveExplicitCast;
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
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Cast, sourceFile->indexInModule, parent);
    node->semanticFct = &SemanticJob::resolveExplicitAutoCast;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doUnaryExpression(node));
    return true;
}
