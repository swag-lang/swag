#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "Attribute.h"
#include "SymTable.h"

bool SyntaxJob::doFuncCallParameters(AstNode* parent, AstNode** result)
{
    auto callParams         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncCallParameters, sourceFile->indexInModule, parent);
    *result                 = callParams;
    callParams->semanticFct = &SemanticJob::resolveFuncCallParams;

    if (token.id != TokenId::SymLeftParen)
    {
        auto param         = Ast::newNode(this, &g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, sourceFile->indexInModule, callParams);
        param->semanticFct = &SemanticJob::resolveFuncCallParam;
        SWAG_CHECK(doExpression(param));
    }
    else
    {
        SWAG_CHECK(eatToken(TokenId::SymLeftParen));
        while (token.id != TokenId::SymRightParen)
        {
            while (true)
            {
                auto param         = Ast::newNode(this, &g_Pool_astFuncCallParam, AstNodeKind::FuncCallParam, sourceFile->indexInModule, callParams);
                param->semanticFct = &SemanticJob::resolveFuncCallParam;
                param->token       = token;
                AstNode* paramExpression;
                SWAG_CHECK(doExpression(nullptr, &paramExpression));

                // Name
                if (token.id == TokenId::SymColon)
                {
                    if (paramExpression->kind != AstNodeKind::IdentifierRef || paramExpression->childs.size() != 1)
                        return sourceFile->report({sourceFile, paramExpression, format("invalid named parameter '%s'", token.text.c_str())});
                    param->namedParamNode = paramExpression->childs.front();
                    param->namedParam     = param->namedParamNode->name;
                    SWAG_CHECK(eatToken());
                    SWAG_CHECK(doExpression(param));
                }
                else
                {
                    Ast::addChildBack(param, paramExpression);
                }

                if (token.id != TokenId::SymComma)
                    break;
                SWAG_CHECK(eatToken(TokenId::SymComma));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightParen));
    }

    return true;
}

bool SyntaxJob::doFuncDeclParameter(AstNode* parent)
{
    auto paramNode         = Ast::newNode(this, &g_Pool_astVarDecl, AstNodeKind::FuncDeclParam, sourceFile->indexInModule, parent);
    paramNode->semanticFct = &SemanticJob::resolveVarDecl;

	// Using variable
    if (token.id == TokenId::KwdUsing)
    {
        SWAG_CHECK(eatToken());
		paramNode->flags |= AST_DECL_USING;
    }

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
    paramNode->inheritTokenName(token);

    // 'self'
    if (paramNode->name == "self")
    {
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(currentScope->parentScope->kind == ScopeKind::Struct, sourceFile->report({sourceFile, "'self' can only be used in an 'impl' block"}));
        auto typeNode         = Ast::newNode(this, &g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, paramNode);
        typeNode->semanticFct = &SemanticJob::resolveTypeExpression;
        typeNode->identifier  = Ast::createIdentifierRef(this, currentScope->parentScope->name, token, typeNode);
        paramNode->type       = typeNode;
    }
    else
    {
        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(paramNode, &paramNode->type));
            SWAG_CHECK(paramNode->type);
        }

        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));
        }
    }

    // Be sure we will be able to have a type
    if (!paramNode->type && !paramNode->assignment)
        return error(paramNode->token, "parameter must be initialized because no type is specified");

    return true;
}

bool SyntaxJob::doFuncDeclParameters(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto allParams         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, parent);
        allParams->semanticFct = &SemanticJob::resolveFuncDeclParams;
        allParams->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
        if (result)
            *result = allParams;

        while (token.id != TokenId::SymRightParen)
        {
            SWAG_CHECK(doFuncDeclParameter(allParams));
            if (token.id == TokenId::SymComma)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::KwdUsing, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));
            }
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doGenericDeclParameters(AstNode* parent, AstNode** result)
{
    auto allParams = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, parent);
    if (result)
        *result = allParams;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    SWAG_VERIFY(token.id != TokenId::SymRightParen, syntaxError(token, "missing generic parameters"));

    while (token.id != TokenId::SymRightParen)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, "missing generic name or type"));
        auto oneParam = Ast::newNode(this, &g_Pool_astVarDecl, AstNodeKind::FuncDeclParam, sourceFile->indexInModule, allParams);
        oneParam->inheritTokenName(token);
        oneParam->semanticFct = &SemanticJob::resolveVarDecl;
        oneParam->flags |= AST_IS_GENERIC;
        SWAG_CHECK(eatToken());

        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(oneParam, &oneParam->type));
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    return true;
}

bool SyntaxJob::doFuncDecl(AstNode* parent, AstNode** result)
{
    auto funcNode         = Ast::newNode(this, &g_Pool_astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, parent);
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
        int id         = g_Global.uniqueID.fetch_add(1);
        funcNode->name = "__test" + to_string(id);
        funcNode->attributeFlags |= ATTRIBUTE_TEST;
    }
    else
    {
        if (token.id == TokenId::SymLeftParen)
        {
            SWAG_CHECK(doGenericDeclParameters(funcNode, &funcNode->genericParameters));
        }

        SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::Intrinsic, syntaxError(token, format("missing function name instead of '%s'", token.text.c_str())));
        isIntrinsic = token.id == TokenId::Intrinsic;
        funcNode->inheritTokenName(token);
    }

    funcNode->computeFullName();
    SWAG_VERIFY(funcNode->name != "opInit", sourceFile->report({sourceFile, token, "'opInit' function cannot be user defined, as it is generated by the compiler"}));

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        funcNode->typeInfo = typeInfo;
        if (!isContextDisabled())
            currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    // Dispatch owners
    if (funcNode->genericParameters)
    {
        Ast::visit(funcNode->genericParameters, [&](AstNode* n) {
            n->ownerFct   = funcNode;
            n->ownerScope = newScope;
        });
    }

    // Parameters
    if (!isTest)
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters));
    }

    // Return type
    auto typeNode         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = &SemanticJob::resolveFuncDeclType;
    if (!isTest)
    {
        if (token.id == TokenId::SymMinusGreat)
        {
            Scoped    scoped(this, newScope);
            ScopedFct scopedFct(this, funcNode);
            SWAG_CHECK(eatToken(TokenId::SymMinusGreat));
            SWAG_CHECK(doTypeExpression(typeNode));
        }
    }

    // Content of function
    if (token.id == TokenId::SymSemiColon || isIntrinsic)
    {
        SWAG_CHECK(eatSemiCol("after function declaration"));
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
    auto node         = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Return, sourceFile->indexInModule, parent);
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