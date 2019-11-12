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

bool SyntaxJob::doTypeAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::TypeAlias, sourceFile, parent);
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
    currentScope->symTable->registerSymbolNameNoLock(sourceFile, node, SymbolKind::TypeAlias);
    return true;
}

bool SyntaxJob::doTypeExpressionLambda(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode(this, &g_Pool_astTypeLambda, AstNodeKind::TypeLambda, sourceFile, parent);
    node->semanticFct = &SemanticJob::resolveTypeLambda;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto params      = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile, node);
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

bool SyntaxJob::convertExpressionListToStruct(AstNode* parent, AstNode** result, bool isConst)
{
    auto structNode = Ast::newStructDecl(sourceFile, nullptr, this);

    auto contentNode               = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode, this);
    structNode->content            = contentNode;
    contentNode->semanticBeforeFct = &SemanticJob::preResolveStruct;

    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    string name = "__" + sourceFile->scopePrivate->name + "_tuple_";
    int    idx  = 0;
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        auto structFieldNode = Ast::newVarDecl(sourceFile, "", contentNode, nullptr);

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
            structFieldNode->name = format("val%u", idx);
        }

        idx++;

        // Name
        typeExpression = (AstTypeExpression*) expression;
        for (int i = 0; i < typeExpression->ptrCount; i++)
            name += "P";
        name += typeExpression->token.text;
        if (typeExpression->identifier)
            name += typeExpression->identifier->childs.back()->name;
		name += "_";

        SWAG_VERIFY(token.id == TokenId::SymComma || token.id == TokenId::SymRightCurly, syntaxError(token, format("invalid token '%s'", token.text.c_str())));
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
	auto rootScope = sourceFile->scopeRoot;
    rootScope->allocateSymTable();
    scoped_lock lk(rootScope->symTable->mutex);
    auto        symbol = rootScope->symTable->findNoLock(structNode->name);
    if (symbol)
    {
        // Must release struct node, it's useless
    }
    else
    {
        auto typeInfo = g_Pool_typeInfoStruct.alloc();
        auto newScope = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, rootScope, true);
        newScope->allocateSymTable();
        typeInfo->name  = structNode->name;
        typeInfo->scope = newScope;
        typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
        structNode->typeInfo = typeInfo;
        structNode->scope    = newScope;
        rootScope->symTable->registerSymbolNameNoLock(sourceFile, structNode, SymbolKind::Struct);

        Ast::addChildBack(sourceFile->astRoot, structNode);
        structNode->inheritOwners(sourceFile->astRoot);

        Ast::visit(structNode->content, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
        });

        // Generate an empty init function so that the user can call it
        {
            Scoped       scoped(this, newScope);
            ScopedStruct scopedStruct(this, newScope);
            structNode->defaultOpInit = generateOpInit(structNode, structNode->name, structNode->genericParameters);
        }
    }

    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
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

    // Else this is a type expression
    auto node = Ast::newTypeExpression(sourceFile, parent, this);
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
		ScopedFlags scopedFlags(this, AST_CAN_INSTANCIATE_TYPE);
        SWAG_CHECK(doIdentifierRef(node, &node->identifier));
		if (inTypeVarDecl)
			node->identifier->childs.back()->flags |= AST_IN_TYPE_VAR_DECLARATION;
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
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Cast, sourceFile, parent);
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
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Cast, sourceFile, parent);
    node->semanticFct = &SemanticJob::resolveExplicitAutoCast;
    if (result)
        *result = node;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doUnaryExpression(node));
    return true;
}
