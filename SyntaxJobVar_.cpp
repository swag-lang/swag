#include "pch.h"
#include "Global.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scope.h"
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

    AstVarDecl* varNode = Ast::newNode(&g_Pool_astVarDecl, kind, sourceFile->indexInModule, parent);
    varNode->inheritOwnersAndFlags(this);
    varNode->semanticFct = SemanticJob::resolveVarDecl;
    varNode->inheritToken(token);
    if (result)
        *result = varNode;
    SWAG_CHECK(tokenizer.getToken(token));

    // Multiple declaration
    vector<AstVarDecl*> otherVariables;
    while (token.id == TokenId::SymComma)
    {
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid variable name '%s'", token.text.c_str())));

        AstVarDecl* otherVarNode = Ast::newNode(&g_Pool_astVarDecl, kind, sourceFile->indexInModule, parent);
        otherVarNode->inheritOwnersAndFlags(this);
        otherVarNode->semanticFct = SemanticJob::resolveVarDecl;
        otherVarNode->inheritToken(token);
        SWAG_CHECK(tokenizer.getToken(token));
        otherVariables.push_back(otherVarNode);
    }

    if (token.id == TokenId::SymColon)
    {
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

    /*if (varNode->type && varNode->type->childs.size() && varNode->type->childs.back()->kind == AstNodeKind::IdentifierRef)
    {
        auto identifierRef = CastAst<AstIdentifierRef>(varNode->type->childs.back(), AstNodeKind::IdentifierRef);
        auto identifier    = CastAst<AstIdentifier>(identifierRef->childs.back(), AstNodeKind::Identifier);
        if (identifier->callParameters)
        {
            string opInitCall       = varNode->name + ".opInit";
            auto   newIdentifierRef = Ast::createIdentifierRef(this, opInitCall, varNode->token, varNode->parent);
            varNode->flags |= AST_DISABLED_INIT;
            auto         newIdentifier = CastAst<AstIdentifier>(newIdentifierRef->childs.back(), AstNodeKind::Identifier);
            CloneContext cloneContext;
            cloneContext.parent           = newIdentifier;
            newIdentifier->callParameters = identifier->callParameters->clone(cloneContext);
        }
    }*/

    return true;
}
