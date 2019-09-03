#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
#include "Scoped.h"
#include "SymTable.h"

bool SyntaxJob::doVarDecl(AstNode* parent, AstNode** result)
{
    // First variable
    AstNodeKind kind = AstNodeKind::VarDecl;
    switch (token.id)
    {
    case TokenId::KwdConst:
        kind = AstNodeKind::ConstDecl;
        break;
    case TokenId::KwdLet:
        kind = AstNodeKind::LetDecl;
        break;
    }

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));

    AstVarDecl* varNode = Ast::newNode(this, &g_Pool_astVarDecl, kind, sourceFile->indexInModule, parent);
    varNode->semanticFct = SemanticJob::resolveVarDecl;
    varNode->inheritTokenName(token);
    if (result)
        *result = varNode;
    SWAG_CHECK(tokenizer.getToken(token));

    // Multiple declaration
    vector<AstVarDecl*> otherVariables;
    while (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));

        AstVarDecl* otherVarNode = Ast::newNode(this, &g_Pool_astVarDecl, kind, sourceFile->indexInModule, parent);
        otherVarNode->semanticFct = SemanticJob::resolveVarDecl;
        otherVarNode->inheritTokenName(token);
        SWAG_CHECK(tokenizer.getToken(token));
        otherVariables.push_back(otherVarNode);
    }

    if (token.id == TokenId::SymColon)
    {
		ScopedFlags scopedFlags(this, AST_IN_TYPE_VAR_DECLARATION);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(varNode, &varNode->type));
    }

    if (token.id == TokenId::SymEqual)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doInitializationExpression(varNode, &varNode->assignment));
    }
    else if (!varNode->type)
    {
        return syntaxError(token, format("invalid token '%s' after variable name", token.text.c_str()));
    }

    // Be sure we will be able to have a type
    if (!varNode->type && !varNode->assignment)
    {
        return error(varNode->token, "variable must be initialized because no type is specified");
    }

    SWAG_CHECK(eatSemiCol("at the end of a variable declation"));

    if (varNode->assignment)
    {
        // When initialization is supposed to be constexpr, we just duplicate the initialization
        SWAG_ASSERT(currentScope);
        if (currentScope->kind == ScopeKind::Struct || currentScope->kind == ScopeKind::File)
        {
            for (auto otherVar : otherVariables)
            {
                CloneContext cloneContext;
                otherVar->assignment = varNode->assignment->clone(cloneContext);
                Ast::addChild(otherVar, otherVar->assignment);
            }
        }
        // Otherwise, we generate initialization for other variables (init to the first var)
        else
        {
            for (auto otherVar : otherVariables)
            {
                otherVar->assignment = Ast::createIdentifierRef(this, varNode->name, varNode->token, otherVar);
            }
        }
    }
    else
    {
        CloneContext cloneContext;
        for (auto otherVar : otherVariables)
        {
            otherVar->type = varNode->type->clone(cloneContext);
            Ast::addChild(otherVar, otherVar->type);
        }
    }

    // Register symbol name
    if (!isContextDisabled())
    {
        currentScope->allocateSymTable();
        SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, varNode, SymbolKind::Variable));
        for (auto otherVar : otherVariables)
            SWAG_CHECK(currentScope->symTable->registerSymbolNameNoLock(sourceFile, otherVar, SymbolKind::Variable));
    }

    // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
    if (varNode->type && varNode->type->kind == AstNodeKind::TypeExpression)
    {
        auto typeExpression = CastAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
        if (typeExpression->identifier)
        {
            auto identifier = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
            if (identifier->callParameters)
            {
                typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
                typeExpression->flags |= AST_HAS_STRUCT_PARAMETERS;
            }
        }
    }

    return true;
}
