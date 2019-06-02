#include "pch.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"
#include "Scoped.h"
#include "ByteCodeGenJob.h"

bool SyntaxJob::doFuncDeclParameter(AstNode* parent)
{
    auto paramNode = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent, false);
    paramNode->inheritOwners(this);
    paramNode->semanticFct = &SemanticJob::resolveVarDecl;

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    paramNode->inheritToken(token);

    vector<AstVarDecl*> allVars;
    SWAG_CHECK(tokenizer.getToken(token));
    while (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken(TokenId::SymComma));
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid parameter name '%s'", token.text.c_str())));

        auto node         = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent, false);
        node->semanticFct = &SemanticJob::resolveVarDecl;
        node->inheritOwners(this);
        node->inheritToken(token);
        allVars.push_back(node);

        SWAG_CHECK(tokenizer.getToken(token));
    }

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
    {
        if (allVars.size() == 0)
            return error(paramNode->token, "parameter must be initialized because no type is specified");
        return error(paramNode->token.startLocation, allVars.back()->token.endLocation, "parameters must be initialized because no type is specified");
    }

    // All additional vars will point to the same type and assignment. We do not put them as childs, this way
    // we're sure nodes won't be evaluated twice
    for (auto var : allVars)
    {
        var->astType       = paramNode->astType;
        var->astAssignment = paramNode->astAssignment;
    }

    return true;
}

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParamsNode = Ast::newNode(&sourceFile->poolFactory->astVarDecl, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, parent, false);
        allParamsNode->inheritOwners(this);
        allParamsNode->semanticFct = &SemanticJob::resolveFuncDeclParams;
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
    auto funcNode = Ast::newNode(&sourceFile->poolFactory->astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, parent, false);
    funcNode->inheritOwners(this);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    if (result)
        *result = funcNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::Intrisic, syntaxError(token, format("invalid function name '%s'", token.text.c_str())));
    Ast::assignToken(funcNode, token);

    bool isIntrinsic = token.id == TokenId::Intrisic;

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = sourceFile->poolFactory->typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(sourceFile, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        typeInfo->name     = funcNode->name;
        funcNode->typeInfo = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(eatToken(TokenId::SymColon));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    // Return type
    auto typeNode = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode, false);
    typeNode->inheritOwners(this);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;
    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
        SWAG_CHECK(doTypeExpression(typeNode));
    }

    // Content of function
    if (isIntrinsic)
    {
        SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    }
    else
    {

        auto curly = move(token);
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

        {
            Scoped    scoped(this, newScope);
            ScopedFct scopedFct(this, funcNode);

            auto stmtNode = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::FuncContent, sourceFile->indexInModule, funcNode, false);
            stmtNode->inheritOwners(this);
            stmtNode->inheritToken(token);
            funcNode->content = stmtNode;
            while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
            {
                SWAG_CHECK(doEmbeddedInstruction(stmtNode));
            }

            stmtNode->token.endLocation = token.startLocation;
        }

        SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
        SWAG_CHECK(tokenizer.getToken(token));
    }

    return true;
}

bool SyntaxJob::doReturn(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeKind::Return, sourceFile->indexInModule, parent, false);
    node->inheritOwners(this);
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

    SWAG_CHECK(eatToken(TokenId::SymSemiColon));
    return true;
}