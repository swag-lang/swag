#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"

bool SyntaxJob::doImpl(AstNode* parent, AstNode** result)
{
    auto implNode         = Ast::newNode<AstImpl>(this, AstNodeKind::Impl, sourceFile, parent);
    implNode->semanticFct = SemanticJob::resolveImpl;
    if (result)
        *result = implNode;

    // Identifier
    {
        ScopedMainNode scopedMainNode(this, implNode);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifier));
        implNode->flags |= AST_NO_BYTECODE;
    }

    // impl TITI for TOTO syntax (interface implementation for a given struct)
    bool implInterface    = false;
    auto identifierStruct = implNode->identifier;
    if (token.id == TokenId::KwdFor)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifierFor));
        implNode->semanticFct = SemanticJob::resolveImplFor;
        identifierStruct      = implNode->identifierFor;
        implInterface         = true;
    }

    // Content of impl block
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Get existing scope or create a new one
    auto& structName      = identifierStruct->childs.front()->name;
    implNode->name        = structName;
    auto newScope         = Ast::newScope(implNode, structName, ScopeKind::Struct, currentScope, true);
    implNode->structScope = newScope;

    // Be sure we have associated a struct typeinfo (we can parse an impl block before the corresponding struct)
    {
        scoped_lock lk1(newScope->owner->mutex);
        auto        typeInfo = newScope->owner->typeInfo;
        if (!typeInfo)
        {
            auto typeStruct           = g_Allocator.alloc<TypeInfoStruct>();
            typeStruct->scope         = newScope;
            newScope->owner->typeInfo = typeStruct;
        }
    }

    // Count number of interfaces
    if (implInterface)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, TypeInfoKind::Struct);
        typeStruct->cptRemainingInterfaces++;
        if (implNode->ownerCompilerIfBlock)
            implNode->ownerCompilerIfBlock->interfacesCount.push_back(typeStruct);
    }

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
    auto structNode         = Ast::newNode<AstStruct>(this, AstNodeKind::StructDecl, sourceFile, parent);
    structNode->semanticFct = SemanticJob::resolveStruct;
    if (result)
        *result = structNode;

    // Special case
    if (token.id == TokenId::KwdUnion)
    {
        structNode->packing = 0;
    }
    else if (token.id == TokenId::KwdInterface)
    {
        structNode->kind        = AstNodeKind::InterfaceDecl;
        structNode->semanticFct = SemanticJob::resolveInterface;
    }

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
    {
        scoped_lock lk(currentScope->symTable.mutex);
        auto        symbol = currentScope->symTable.findNoLock(structNode->name);
        if (!symbol)
        {
            newScope = Ast::newScope(structNode, structNode->name, ScopeKind::Struct, currentScope, true);

            // If an 'impl' came first, then typeinfo has already been defined
            scoped_lock     lk1(newScope->owner->mutex);
            TypeInfoStruct* typeInfo = (TypeInfoStruct*) newScope->owner->typeInfo;
            if (!typeInfo)
            {
                typeInfo                  = g_Allocator.alloc<TypeInfoStruct>();
                newScope->owner->typeInfo = typeInfo;
            }

            structNode->typeInfo           = newScope->owner->typeInfo;
            typeInfo->structNode           = structNode;
            newScope->owner                = structNode;
            typeInfo->name                 = structNode->name;
            typeInfo->structName           = typeInfo->name;
            typeInfo->scope                = newScope;
            structNode->scope              = newScope;
            auto symbolKind                = structNode->kind == AstNodeKind::StructDecl ? SymbolKind::Struct : SymbolKind::Interface;
            structNode->resolvedSymbolName = currentScope->symTable.registerSymbolNameNoLock(&context, structNode, symbolKind);
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
        Ast::visit(structNode->genericParameters, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
            if (n->kind == AstNodeKind::FuncDeclParam)
            {
                auto param = CastAst<AstVarDecl>(n, AstNodeKind::FuncDeclParam);
                newScope->symTable.registerSymbolName(&context, n, param->type ? SymbolKind::Variable : SymbolKind::GenericType);
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

        auto contentNode               = Ast::newNode<AstNode>(this, AstNodeKind::StructContent, sourceFile, structNode);
        structNode->content            = contentNode;
        contentNode->semanticBeforeFct = SemanticJob::preResolveStruct;

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
            return syntaxError(token, "no matching '}' found in struct declaration");

        switch (token.id)
        {
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, nullptr, CompilerAstKind::StructVarDecl));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
            break;

        case TokenId::SymRightCurly:
            SWAG_CHECK(eatToken());
            return true;

        case TokenId::SymAttrStart:
            SWAG_CHECK(doAttrUse(parent));
            break;

        case TokenId::DocComment:
            SWAG_CHECK(doDocComment(parent));
            break;

        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIfFor(parent, nullptr, AstNodeKind::StructDecl));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
            break;

        case TokenId::SymLeftCurly:
        {
            auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            SWAG_CHECK(doStructContent(stmt));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
            break;
        }

        case TokenId::KwdUsing:
        {
            SWAG_CHECK(eatToken());
            AstNode* varDecl;
            SWAG_CHECK(doVarDecl(parent, &varDecl, AstNodeKind::VarDecl));
            varDecl->flags |= AST_DECL_USING;
            if (!waitCurly)
                return true;
            break;
        }

        case TokenId::KwdInternal:
        {
            auto attrBlockNode         = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
            attrBlockNode->semanticFct = SemanticJob::resolveAttrUse;
            attrBlockNode->attributeFlags |= ATTRIBUTE_INTERNAL;
            SWAG_CHECK(eatToken());
            continue;
        }

        case TokenId::KwdReadOnly:
        {
            auto attrBlockNode         = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
            attrBlockNode->semanticFct = SemanticJob::resolveAttrUse;
            attrBlockNode->attributeFlags |= ATTRIBUTE_READONLY;
            SWAG_CHECK(eatToken());
            continue;
        }

        default:
            SWAG_CHECK(doVarDecl(parent, nullptr, AstNodeKind::VarDecl));
            if (!waitCurly)
                return true;
            break;
        }
    }

    return true;
}
