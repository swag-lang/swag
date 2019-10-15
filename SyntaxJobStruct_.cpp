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
    auto implNode         = Ast::newNode(this, &g_Pool_astImpl, AstNodeKind::Impl, sourceFile->indexInModule, parent);
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
    auto newScope = Ast::newScope(implNode, implNode->identifier->childs.front()->name, ScopeKind::Struct, currentScope, true);
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
    auto structNode         = Ast::newNode(this, &g_Pool_astStruct, AstNodeKind::StructDecl, sourceFile->indexInModule, parent);
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
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
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
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope, structNode);

        auto contentNode               = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::StructContent, sourceFile->indexInModule, structNode);
        structNode->content            = contentNode;
        contentNode->semanticBeforeFct = &SemanticJob::preResolveStruct;

        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            if (token.id == TokenId::SymAttrStart)
                SWAG_CHECK(doAttrUse(contentNode));
            else
                SWAG_CHECK(doVarDecl(contentNode, nullptr, AstNodeKind::VarDecl));
        }
    }

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, syntaxError(curly, "no matching '}' found"));
    SWAG_CHECK(tokenizer.getToken(token));

    // Generate an empty init function so that the user can call it
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);
        structNode->defaultOpInit = generateOpInit(structNode->parent, structNode->name, structNode->genericParameters);
    }

    return true;
}

void SyntaxJob::setupSelfType(AstIdentifier* node, const Utf8& name, AstNode* genericParameters)
{
    if (!node->ownerStructScope)
        return;
    node->name = name;

    if (genericParameters)
    {
        auto allParams          = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, node);
        allParams->semanticFct  = &SemanticJob::resolveFuncDeclParams;
        node->genericParameters = allParams;

        if (genericParameters)
        {
            for (auto genParam : genericParameters->childs)
            {
                Ast::createIdentifierRef(this, genParam->name, genParam->token, allParams);
            }
        }
    }
}

AstNode* SyntaxJob::generateOpInit(AstNode* node, const Utf8& structName, AstNode* genericParameters)
{
    // Generate impl
    auto implNode        = Ast::newNode(this, &g_Pool_astImpl, AstNodeKind::Impl, sourceFile->indexInModule, node);
    implNode->identifier = Ast::createIdentifierRef(this, structName, node->token, implNode);
    auto structScope     = Ast::newScope(implNode, structName, ScopeKind::Struct, currentScope, true);
    structScope->allocateSymTable();
    implNode->structScope = structScope;

    auto funcNode         = Ast::newNode(this, &g_Pool_astFuncDecl, AstNodeKind::FuncDecl, sourceFile->indexInModule, implNode);
    funcNode->semanticFct = &SemanticJob::resolveFuncDecl;
    funcNode->name        = "opInit";
    funcNode->flags |= AST_GENERATED;

    // Register function name
    Scope* newScope = nullptr;
    currentScope->allocateSymTable();
    {
        scoped_lock lk(currentScope->symTable->mutex);
        auto        typeInfo = g_Pool_typeInfoFuncAttr.alloc();
        newScope             = Ast::newScope(funcNode, funcNode->name, ScopeKind::Function, currentScope);
        newScope->allocateSymTable();
        funcNode->typeInfo = typeInfo;
        currentScope->symTable->registerSymbolNameNoLock(sourceFile, funcNode, SymbolKind::Function);
    }

    {
        Scoped       scoped(this, newScope);
        ScopedFct    scopedFct(this, funcNode);
        ScopedStruct scopedStruct(this, structScope);

        // Parameters
        funcNode->parameters              = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclParams, sourceFile->indexInModule, funcNode);
        funcNode->parameters->semanticFct = &SemanticJob::resolveFuncDeclParams;

        // One parameter
        auto param         = Ast::newNode(this, &g_Pool_astVarDecl, AstNodeKind::FuncDeclParam, sourceFile->indexInModule, funcNode->parameters);
        param->semanticFct = &SemanticJob::resolveVarDecl;
        param->name        = "self";

        auto typeNode         = Ast::newNode(this, &g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, param);
        typeNode->ptrCount    = 1;
        typeNode->semanticFct = &SemanticJob::resolveTypeExpression;
        typeNode->identifier  = Ast::createIdentifierRef(this, "Self", token, typeNode);
        param->type           = typeNode;
        setupSelfType(CastAst<AstIdentifier>(typeNode->identifier->childs.front(), AstNodeKind::Identifier), structName, genericParameters);
    }

    funcNode->returnType              = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::FuncDeclType, sourceFile->indexInModule, funcNode);
    funcNode->returnType->semanticFct = &SemanticJob::resolveFuncDeclType;

    // Content
    {
        Scoped    scoped(this, newScope);
        ScopedFct scopedFct(this, funcNode);
        funcNode->content = Ast::newNode(this, &g_Pool_astNode, AstNodeKind::Statement, sourceFile->indexInModule, funcNode);
    }

    return funcNode;
}
