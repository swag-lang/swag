#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Ast.h"
#include "Scope.h"
#include "Scoped.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Attribute.h"

bool SyntaxJob::doImpl(AstNode* parent, AstNode** result)
{
    auto implNode         = Ast::newNode(this, &g_Pool_astImpl, AstNodeKind::Impl, sourceFile, parent);
    implNode->semanticFct = &SemanticJob::resolveImpl;
    if (result)
        *result = implNode;

    // Identifier
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifier));
    implNode->flags |= AST_NO_BYTECODE;

    // Content of impl block
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Get existing scope or create a new one
    auto newScope = Ast::newScope(implNode, implNode->identifier->childs.front()->name, ScopeKind::Struct, currentScope, true);
    newScope->allocateSymTable();
    implNode->structScope = newScope;

    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);
        ScopedFlags  scopedFlags(this, AST_INSIDE_IMPL);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(implNode));
        }
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));

    return true;
}

bool SyntaxJob::doStruct(AstNode* parent, AstNode** result)
{
    auto structNode         = Ast::newNode(this, &g_Pool_astStruct, AstNodeKind::StructDecl, sourceFile, parent);
    structNode->semanticFct = &SemanticJob::resolveStruct;
    if (result)
        *result = structNode;

    // Structure layout
    if (token.id == TokenId::KwdUnion)
        structNode->packing = 0;

    SWAG_CHECK(tokenizer.getToken(token));

    // Generic arguments
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(doGenericDeclParameters(structNode, &structNode->genericParameters));
        structNode->flags |= AST_IS_GENERIC | AST_NO_BYTECODE;
    }

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid struct name '%s'", token.text.c_str())));
    structNode->inheritTokenName(token);

    // If name starts with "__", then this is generated, as a user identifier cannot start with those
    // two characters
    if (structNode->name.length() > 2 && structNode->name[0] == '_' && structNode->name[1] == '_')
        structNode->flags |= AST_GENERATED;

    // Add struct type and scope
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(structNode->name);
        if (!symbol)
        {
            auto typeInfo = g_Pool_typeInfoStruct.alloc();
            newScope      = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, currentScope, true);
            newScope->allocateSymTable();
            typeInfo->name                 = structNode->name;
            typeInfo->scope                = newScope;
            structNode->typeInfo           = typeInfo;
            structNode->scope              = newScope;
            structNode->resolvedSymbolName = currentScope->symTable->registerSymbolNameNoLock(sourceFile, structNode, SymbolKind::Struct);
        }
        else
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
    }

    // Dispatch owners
    if (structNode->genericParameters)
    {
        scoped_lock lock(newScope->symTable->mutex);
        Ast::visit(structNode->genericParameters, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
            if (n->kind == AstNodeKind::FuncDeclParam)
            {
                auto param = CastAst<AstVarDecl>(n, AstNodeKind::FuncDeclParam);
                newScope->symTable->registerSymbolNameNoLock(sourceFile, n, param->type ? SymbolKind::Variable : SymbolKind::GenericType);
            }
        });
    }

    // Raw type
    SWAG_CHECK(tokenizer.getToken(token));

    // Content of struct
    {
        Scoped         scoped(this, newScope);
        ScopedStruct   scopedStruct(this, newScope);
        ScopedMainNode scopedMainNode(this, structNode);

        auto contentNode               = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::StructContent, sourceFile, structNode);
        structNode->content            = contentNode;
        contentNode->semanticBeforeFct = &SemanticJob::preResolveStruct;

        SWAG_CHECK(doStructContent(contentNode));
    }

    return true;
}

bool SyntaxJob::doStructContent(AstNode* parent)
{
    bool waitCurly = false;
    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
        waitCurly = true;
    }

    while (true)
    {
        if (token.id == TokenId::EndOfFile)
        {
            SWAG_CHECK(syntaxError(token, "no matching '}' found in struct declaration"));
            break;
        }

        if (token.id == TokenId::SymRightCurly)
        {
            SWAG_CHECK(eatToken());
            break;
        }

        if (token.id == TokenId::SymAttrStart)
        {
            SWAG_CHECK(doAttrUse(parent));
        }
        else if (token.id == TokenId::CompilerIf)
        {
            SWAG_CHECK(doCompilerIfFor(parent, nullptr, AstNodeKind::StructDecl));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
        }
        else if (token.id == TokenId::SymLeftCurly)
        {
            auto stmt = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Statement, sourceFile, parent);
            SWAG_CHECK(doStructContent(stmt));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
        }
        else
        {
            SWAG_CHECK(doVarDecl(parent, nullptr, AstNodeKind::VarDecl));
        }
    }

    return true;
}
