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

    auto scopeKind = ScopeKind::Struct;
    SWAG_CHECK(tokenizer.getToken(token));
    switch (token.id)
    {
    case TokenId::KwdEnum:
        scopeKind = ScopeKind::Enum;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    }

    // Identifier
    {
        ScopedMainNode scopedMainNode(this, implNode);
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifier));
        implNode->flags |= AST_NO_BYTECODE;
    }

    // impl TITI for TOTO syntax (interface implementation for a given struct)
    bool implInterface    = false;
    auto identifierStruct = implNode->identifier;
    if (token.id == TokenId::KwdFor)
    {
        SWAG_VERIFY(scopeKind == ScopeKind::Struct, sourceFile->report({implNode, token, "'for' is invalid for an enum implementation block"}));
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
    auto& structName = identifierStruct->childs.front()->name;
    implNode->name   = structName;
    auto newScope    = Ast::newScope(implNode, structName, scopeKind, currentScope, true);
    if (scopeKind != newScope->kind)
    {
        Diagnostic diag{implNode->identifier, implNode->identifier->token, format("the implementation block kind (%s) does not match the type of '%s' (%s)", Scope::getNakedKindName(scopeKind), implNode->name.c_str(), Scope::getNakedKindName(newScope->kind))};
        Diagnostic note{newScope->owner, newScope->owner->token, format("this is the declaration of '%s'", implNode->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    implNode->structScope = newScope;

    // Be sure we have associated a struct typeinfo (we can parse an impl block before the corresponding struct)
    {
        scoped_lock lk1(newScope->owner->mutex);
        auto        typeInfo = newScope->owner->typeInfo;
        if (!typeInfo)
        {
            if (scopeKind == ScopeKind::Enum)
            {
                auto typeEnum             = g_Allocator.alloc<TypeInfoEnum>();
                typeEnum->scope           = newScope;
                typeEnum->nakedName       = structName;
                typeEnum->name            = structName;
                newScope->owner->typeInfo = typeEnum;
            }
            else
            {
                auto typeStruct           = g_Allocator.alloc<TypeInfoStruct>();
                typeStruct->scope         = newScope;
                typeStruct->nakedName     = structName;
                typeStruct->name          = structName;
                newScope->owner->typeInfo = typeStruct;
            }
        }
    }

    // Count number of interfaces
    if (implInterface)
    {
        SWAG_ASSERT(scopeKind == ScopeKind::Struct);
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, TypeInfoKind::Struct);
        typeStruct->cptRemainingInterfaces++;
        if (implNode->ownerCompilerIfBlock)
            implNode->ownerCompilerIfBlock->interfacesCount.push_back(typeStruct);
    }

    // For an interface implementation, creates a sub scope named like the interface
    auto parentScope = newScope;
    if (implInterface)
    {
        scoped_lock lk(newScope->symTable.mutex);
        auto&       itfName  = implNode->identifier->childs.back()->name;
        auto        symbol   = newScope->symTable.findNoLock(itfName);
        Scope*      subScope = nullptr;
        if (!symbol)
        {
            subScope        = Ast::newScope(implNode, itfName, ScopeKind::Impl, newScope, false);
            auto typeInfo   = g_Allocator.alloc<TypeInfoStruct>();
            typeInfo->name  = implNode->identifier->childs.back()->name;
            typeInfo->scope = subScope;
            newScope->symTable.addSymbolTypeInfoNoLock(&context, implNode, typeInfo, SymbolKind::Struct, nullptr, OVERLOAD_IMPL, nullptr, 0, &itfName);
        }
        else
        {
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(symbol->overloads[0]->typeInfo, TypeInfoKind::Struct);
            subScope      = typeInfo->scope;
        }

        parentScope     = subScope;
        implNode->scope = subScope;
    }

    {
        Scoped       scoped(this, parentScope);
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
    SWAG_CHECK(isValidUserName(structNode));

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
            if (newScope->kind != ScopeKind::Struct)
            {
                auto       implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic diag{implNode->identifier, implNode->identifier->token, format("the implementation block kind (%s) does not match the type of '%s' (%s)", Scope::getNakedKindName(newScope->kind), implNode->name.c_str(), Scope::getNakedKindName(ScopeKind::Struct))};
                Diagnostic note{structNode, structNode->token, format("this is the declaration of '%s'", implNode->name.c_str()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }

            structNode->scope = newScope;

            // If an 'impl' came first, then typeinfo has already been defined
            scoped_lock     lk1(newScope->owner->mutex);
            TypeInfoStruct* typeInfo = (TypeInfoStruct*) newScope->owner->typeInfo;
            if (!typeInfo)
            {
                typeInfo                  = g_Allocator.alloc<TypeInfoStruct>();
                newScope->owner->typeInfo = typeInfo;
            }

            SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Struct);
            structNode->typeInfo           = newScope->owner->typeInfo;
            typeInfo->declNode             = structNode;
            newScope->owner                = structNode;
            typeInfo->name                 = structNode->name;
            typeInfo->nakedName            = structNode->name;
            typeInfo->structName           = structNode->name;
            typeInfo->scope                = newScope;
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
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, nullptr));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
            break;
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, nullptr, CompilerAstKind::StructVarDecl));
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;
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
        case TokenId::SymRightCurly:
            SWAG_CHECK(eatToken());
            return true;

        case TokenId::SymAttrStart:
            SWAG_CHECK(doAttrUse(parent));
            break;

        case TokenId::KwdAlias:
            SWAG_CHECK(doAlias(parent));
            break;
        case TokenId::KwdUsing:
        {
            SWAG_CHECK(eatToken());

            auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;

            AstNode* varDecl;
            SWAG_CHECK(doVarDecl(stmt, &varDecl, AstNodeKind::VarDecl));
            varDecl->flags |= AST_DECL_USING;
            if (!waitCurly)
                return true;
            break;
        }

        case TokenId::KwdReadOnly:
        case TokenId::KwdReadWrite:
        {
            auto attrBlockNode         = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
            attrBlockNode->semanticFct = SemanticJob::resolveAttrUse;

            if (token.id == TokenId::KwdReadOnly)
                attrBlockNode->attributeFlags |= ATTRIBUTE_READONLY;
            else if (token.id == TokenId::KwdReadWrite)
                attrBlockNode->attributeFlags |= ATTRIBUTE_READWRITE;

            SWAG_CHECK(eatToken());

            if (token.id == TokenId::SymLeftCurly)
                continue;

            auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
            parent->ownerMainNode->flags |= AST_STRUCT_COMPOUND;

            SWAG_CHECK(doVarDecl(stmt, nullptr, AstNodeKind::VarDecl));
            if (!waitCurly)
                return true;
            break;
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
