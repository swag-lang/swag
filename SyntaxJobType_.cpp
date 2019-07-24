#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "SymTable.h"
#include "Scoped.h"
#include "Diagnostic.h"

bool SyntaxJob::doTypeDecl(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::TypeDecl, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeDecl;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid type name '%s'", token.text.c_str())));
    node->name = token.text;
    node->name.computeCrc();
    node->token = move(token);

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymColon));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatSemiCol("after type alias"));

    currentScope->allocateSymTable();
    if (!isContextDisabled())
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, node, SymbolKind::Type);

    return true;
}

bool SyntaxJob::doTypeExpressionLambda(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astTypeLambda, AstNodeKind::TypeLambda, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeLambda;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto params      = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, node);
        node->parameters = params;
        while (true)
        {
            SWAG_CHECK(doTypeExpression(params));
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

bool SyntaxJob::doTypeExpressionTuple(AstNode* parent, AstNode** result)
{
    // Else this is a type expression
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::ExpressionList, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeTuple;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());

    auto newScope = Ast::newScope("", ScopeKind::TypeList, currentScope);
    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTypeExpression(node));
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
    ScopedFlags sc(this, AST_NO_BYTECODE_CHILDS);

    // This is a function
    if (token.id == TokenId::SymLeftParen)
        return doTypeExpressionLambda(parent, result);

    // This is a tuple
    if (token.id == TokenId::SymLeftCurly)
        return doTypeExpressionTuple(parent, result);

    // Else this is a type expression
    auto node = Ast::newNode(&g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeExpression;
    if (result)
        *result = node;

    // Const
    node->isConst = false;
    if (token.id == TokenId::KwdConst)
    {
        node->isConst = true;
        SWAG_CHECK(tokenizer.getToken(token));
    }

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
        SWAG_CHECK(doIdentifierRef(node, &node->typeExpression));
        return true;
    }

    SWAG_CHECK(syntaxError(token, format("invalid type declaration '%s'", token.text.c_str())));
    return true;
}

bool SyntaxJob::doTemplateTypes(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::TypeList, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveTypeList;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymExclam));
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(eatToken());
        while (true)
        {
            SWAG_CHECK(doTypeExpression(node));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen, "after types"));
    }
    else
    {
        SWAG_CHECK(doTypeExpression(node));
    }

    return true;
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Cast, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveCast;
    node->inheritToken(token);
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'cast'"));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after type expression"));
    SWAG_CHECK(doUnaryExpression(node));
    return true;
}
