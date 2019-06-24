#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Scope.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "Attribute.h"
#include "SymTable.h"

bool SyntaxJob::doFuncDeclParameter(AstNode* parent)
{
    auto paramNode = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::FuncDeclParam, sourceFile->indexInModule, parent);
    paramNode->inheritOwnersAndFlags(this);
    paramNode->semanticFct = &SemanticJob::resolveVarDecl;

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    paramNode->inheritToken(token);

    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id == TokenId::SymColon)
    {
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doTypeExpression(paramNode, &paramNode->astType));
        SWAG_CHECK(paramNode->astType);
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken(TokenId::SymEqual));
        SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->astAssignment));
    }

    // Be sure we will be able to have a type
    if (!paramNode->astType && !paramNode->astAssignment)
        return error(paramNode->token, "parameter must be initialized because no type is specified");

    return true;
}

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParamsNode = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, parent);
        allParamsNode->inheritOwnersAndFlags(this);
        allParamsNode->semanticFct = &SemanticJob::resolveFuncDeclParams;
        allParamsNode->byteCodeFct = &ByteCodeGenJob::emitFuncDeclParams;
        allParamsNode->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
        if (result)
            *result = allParamsNode;

        while (token.id != TokenId::SymRightParen)
        {
            SWAG_CHECK(doFuncDeclParameter(allParamsNode));
            if (token.id == TokenId::SymComma)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
            }
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doFuncDecl(AstNode* parent, AstNode** result)
{
    auto funcNode = Ast::newNode(&g_Pool_astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, parent);
    funcNode->inheritOwnersAndFlags(this);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    if (result)
        *result = funcNode;

    bool isTest      = false;
    bool isIntrinsic = false;

    if (token.id == TokenId::CompilerTest)
        isTest = true;
    SWAG_CHECK(tokenizer.getToken(token));

    // Name
    if (isTest)
    {
        Ast::assignToken(funcNode, token);
        int id         = g_Global.uniqueID.fetch_add(1);
        funcNode->name = "__test" + to_string(id);
        funcNode->attributeFlags |= ATTRIBUTE_TEST;
    }
    else
    {
        if (token.id == TokenId::IntrisicPrint || token.id == TokenId::IntrisicAssert)
            isIntrinsic = true;

        SWAG_VERIFY(token.id == TokenId::Identifier || isIntrinsic, syntaxError(token, format("invalid function name '%s'", token.text.c_str())));
        Ast::assignToken(funcNode, token);
    }

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(sourceFile, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        typeInfo->name     = funcNode->name;
        funcNode->typeInfo = typeInfo;
        if (!isContextDisabled())
            currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    // Parameters
    if (!isTest)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    // Return type
    auto typeNode = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode);
    typeNode->inheritOwnersAndFlags(this);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;
    if (!isTest)
    {
        if (token.id == TokenId::SymMinusGreat)
        {
            SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
            SWAG_CHECK(doTypeExpression(typeNode));
        }
    }

    // Content of function
    if (token.id == TokenId::SymSemiColon || isIntrinsic)
    {
        SWAG_CHECK(eatToken(TokenId::SymSemiColon));
        return true;
    }

    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
        funcNode->content->token = token;
    }

    return true;
}

bool SyntaxJob::doReturn(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&g_Pool_astNode, AstNodeKind::Return, sourceFile->indexInModule, parent);
    node->inheritOwnersAndFlags(this);
    node->semanticFct = &SemanticJob::resolveReturn;
    node->byteCodeFct = &ByteCodeGenJob::emitReturn;
    if (result)
        *result = node;

    // Return value
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != TokenId::SymSemiColon)
    {
        SWAG_CHECK(doExpression(node));
    }

    return true;
}