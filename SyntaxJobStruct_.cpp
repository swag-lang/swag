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
    auto implNode = Ast::newNode(&g_Pool_astImpl, AstNodeKind::Impl, sourceFile->indexInModule, parent);
    implNode->inheritOwnersAndFlags(this);
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

    // Get or create scope
    auto newScope = Ast::newScope(implNode->identifier->childs.front()->name, ScopeKind::Struct, currentScope, true);
    newScope->allocateSymTable();
    implNode->structScope = newScope;

    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);
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
    auto structNode = Ast::newNode(&g_Pool_astStruct, AstNodeKind::StructDecl, sourceFile->indexInModule, parent);
    structNode->inheritOwnersAndFlags(this);
    structNode->semanticFct = &SemanticJob::resolveStruct;
    if (result)
        *result = structNode;

    SWAG_CHECK(tokenizer.getToken(token));

	// Generic arguments
	if (token.id == TokenId::SymLeftParen)
	{
		SWAG_CHECK(doGenericDeclParameters(structNode, &structNode->genericParameters));
		structNode->flags |= AST_IS_GENERIC;
	}

    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid struct name '%s'", token.text.c_str())));
    Ast::assignToken(structNode, token);

    // Add struct type and scope
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        symbol = currentScope->symTable->findNoLock(structNode->name);
        if (!symbol)
        {
            auto typeInfo = g_Pool_typeInfoStruct.alloc();
            newScope      = Ast::newScope(structNode->name, ScopeKind::Struct, currentScope, true);
            newScope->allocateSymTable();
            typeInfo->name       = structNode->name;
            typeInfo->scope      = newScope;
            structNode->typeInfo = typeInfo;
            if (!isContextDisabled())
                currentScope->symTable->registerSymbolNameNoLock(sourceFile, structNode, SymbolKind::Struct);
        }
        else
        {
            auto       firstOverload = &symbol->defaultOverload;
            Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
            return sourceFile->report(diag, &diagNote);
        }
    }

    // Raw type
    SWAG_CHECK(tokenizer.getToken(token));

    // Content of struct
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doVarDecl(structNode));
        }
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));

    // Generate an empty init function so that the user can call it
    {
        Scoped scoped(this, newScope);
        generateOpInit(structNode);
    }

    return true;
}

void SyntaxJob::generateOpInit(AstNode* node)
{
    auto structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
    auto funcNode   = Ast::newNode(&g_Pool_astFuncDecl, AstNodeKind::FuncDecl, structNode->sourceFileIdx, structNode->parent);
    funcNode->inheritOwnersAndFlags(this);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    funcNode->name        = "opInit";
    funcNode->flags |= AST_NO_BYTECODE | AST_BYTECODE_GENERATED | AST_GENERATED;
    structNode->opInit = funcNode;

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        funcNode->typeInfo = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);

        // Parameters
        funcNode->parameters = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncDeclParams, structNode->sourceFileIdx, funcNode);
        funcNode->parameters->inheritOwnersAndFlags(this);
        funcNode->parameters->byteCodeFct = &ByteCodeGenJob::emitFuncDeclParams;

        // One parameter
        auto param = Ast::newNode(&g_Pool_astVarDecl, AstNodeKind::FuncDeclParam, structNode->sourceFileIdx, funcNode->parameters);
        param->inheritOwnersAndFlags(this);
        param->semanticFct = &SemanticJob::resolveVarDecl;
        param->name        = "self";

        auto typeNode = Ast::newNode(&g_Pool_astTypeExpression, AstNodeKind::TypeExpression, structNode->sourceFileIdx, param);
        typeNode->inheritOwnersAndFlags(this);
        typeNode->semanticFct    = &SemanticJob::resolveTypeExpression;
        typeNode->typeExpression = Ast::createIdentifierRef(this, currentScope->parentScope->name, token, typeNode);
        param->type              = typeNode;
    }

    funcNode->returnType = Ast::newNode(&g_Pool_astNode, AstNodeKind::FuncDeclType, structNode->sourceFileIdx, funcNode);
    funcNode->returnType->inheritOwnersAndFlags(this);
    funcNode->returnType->semanticFct = &SemanticJob::resolveFuncDeclType;

    // Content
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        funcNode->content = Ast::newNode(&g_Pool_astNode, AstNodeKind::Statement, structNode->sourceFileIdx, funcNode);
        funcNode->content->inheritOwnersAndFlags(this);
    }
}
