#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Module.h"

bool SyntaxJob::doImpl(AstNode* parent, AstNode** result)
{
    auto implNode         = Ast::newNode<AstImpl>(this, AstNodeKind::Impl, sourceFile, parent);
    implNode->semanticFct = SemanticJob::resolveImpl;
    if (result)
        *result = implNode;

    auto scopeKind = ScopeKind::Struct;
    SWAG_CHECK(eatToken());
    switch (token.id)
    {
    case TokenId::KwdEnum:
        scopeKind = ScopeKind::Enum;
        SWAG_CHECK(eatToken());
        break;
    }

    // Identifier
    {
        ScopedFlags scopedFlags(this, AST_CAN_MATCH_INCOMPLETE);
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifier, IDENTIFIER_NO_PARAMS));
        implNode->flags |= AST_NO_BYTECODE;
    }

    // impl TITI for TOTO syntax (interface implementation for a given struct)
    bool implInterface    = false;
    auto identifierStruct = implNode->identifier;
    if (token.id == TokenId::KwdFor)
    {
        SWAG_VERIFY(scopeKind != ScopeKind::Enum, sourceFile->report({implNode, token, g_E[Err0438]}));
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifierFor, IDENTIFIER_NO_FCT_PARAMS));
        implNode->identifierFor->allocateExtension();
        implNode->identifierFor->extension->semanticAfterFct = SemanticJob::resolveImplForAfterFor;
        implNode->semanticFct                                = SemanticJob::resolveImplFor;
        implNode->allocateExtension();
        implNode->extension->semanticAfterFct = SemanticJob::resolveImplForType;
        identifierStruct                      = implNode->identifierFor;
        implInterface                         = true;

        auto last = CastAst<AstIdentifier>(identifierStruct->childs.back(), AstNodeKind::Identifier);
        SWAG_VERIFY(!last->genericParameters, sourceFile->report({last->genericParameters, g_E[Err0440]}));
    }
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(implNode->identifier, "as 'impl' name"));
    }

    // Content of impl block
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Get existing scope or create a new one
    auto& structName     = identifierStruct->childs.back()->token.text;
    implNode->token.text = structName;

    auto newScope = Ast::newScope(implNode, structName, scopeKind, currentScope, true);
    if (scopeKind != newScope->kind)
    {
        Utf8 hint;
        if (newScope->kind == ScopeKind::Enum)
            hint = Fmt(g_E[Hnt0019], implNode->token.ctext());
        else if (newScope->kind == ScopeKind::Struct)
            hint = Fmt(g_E[Hnt0020], implNode->token.ctext());
        PushErrHint errh(hint);
        Diagnostic  diag{implNode, Fmt(g_E[Err0441], Scope::getNakedKindName(scopeKind), implNode->token.ctext(), Scope::getNakedKindName(newScope->kind))};
        Diagnostic  note{newScope->owner, Fmt(g_E[Nte0027], implNode->token.ctext()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    implNode->structScope = newScope;

    // Be sure we have associated a struct typeinfo (we can parse an impl block before the corresponding struct)
    {
        ScopedLock lk1(newScope->owner->mutex);
        auto       typeInfo = newScope->owner->typeInfo;
        if (!typeInfo)
        {
            if (scopeKind == ScopeKind::Enum)
            {
                auto typeEnum             = allocType<TypeInfoEnum>();
                typeEnum->scope           = newScope;
                typeEnum->name            = structName;
                newScope->owner->typeInfo = typeEnum;
            }
            else
            {
                auto typeStruct           = allocType<TypeInfoStruct>();
                typeStruct->scope         = newScope;
                typeStruct->name          = structName;
                typeStruct->structName    = structName;
                newScope->owner->typeInfo = typeStruct;
            }
        }
    }

    // Count number of interfaces
    if (implInterface)
    {
        SWAG_ASSERT(scopeKind == ScopeKind::Struct);

        // Register the 'impl for' block name, because we are not sure that the newScope will be the correct one
        // (we will have to check in the semantic pass that what's after 'for' (the struct) is correct.
        // See test 2909 for that kind of case...
        module->addImplForToSolve(structName);

        auto typeStruct = CastTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, TypeInfoKind::Struct);
        typeStruct->cptRemainingInterfacesReg++;
        typeStruct->cptRemainingInterfaces++;

        if (implNode->ownerCompilerIfBlock)
            implNode->ownerCompilerIfBlock->interfacesCount.push_back(typeStruct);
    }

    // For an interface implementation, creates a sub scope named like the interface
    auto parentScope = newScope;
    if (implInterface)
    {
        ScopedLock lk(newScope->symTable.mutex);
        Utf8       itfName  = implNode->identifier->childs.back()->token.text;
        auto       symbol   = newScope->symTable.findNoLock(itfName);
        Scope*     subScope = nullptr;
        if (!symbol)
        {
            subScope             = Ast::newScope(implNode, itfName, ScopeKind::Impl, newScope, false);
            auto typeInfo        = allocType<TypeInfoStruct>();
            typeInfo->name       = implNode->identifier->childs.back()->token.text;
            typeInfo->structName = typeInfo->name;
            typeInfo->scope      = subScope;
            typeInfo->declNode   = implNode;
            newScope->symTable.addSymbolTypeInfoNoLock(&context, implNode, typeInfo, SymbolKind::Struct, nullptr, OVERLOAD_IMPL_IN_STRUCT, nullptr, 0, nullptr, &itfName);
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

    SWAG_VERIFY(token.id == TokenId::SymRightCurly, error(curly, g_E[Err0880]));
    SWAG_CHECK(eatToken());

    return true;
}

bool SyntaxJob::doStruct(AstNode* parent, AstNode** result)
{
    auto structNode         = Ast::newNode<AstStruct>(this, AstNodeKind::StructDecl, sourceFile, parent);
    structNode->semanticFct = SemanticJob::resolveStruct;
    structNode->allocateExtension();
    structNode->extension->semanticAfterFct = SemanticJob::sendCompilerMsgTypeDecl;
    if (result)
        *result = structNode;

    // Special case
    SyntaxStructType structType = SyntaxStructType::Struct;
    if (token.id == TokenId::KwdInterface)
    {
        structType              = SyntaxStructType::Interface;
        structNode->kind        = AstNodeKind::InterfaceDecl;
        structNode->semanticFct = SemanticJob::resolveInterface;
    }
    else if (token.id == TokenId::KwdUnion)
    {
        structNode->structFlags |= STRUCTFLAG_UNION;
    }

    SWAG_CHECK(eatToken());

    // Generic arguments
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(doGenericDeclParameters(structNode, &structNode->genericParameters));
        structNode->flags |= AST_IS_GENERIC | AST_NO_BYTECODE;
    }

    // If a struct is declared inside a generic struct, force the sub struct to have generic parameters
    else if (currentScope && currentScope->kind == ScopeKind::Struct)
    {
        auto parentStruct = CastAst<AstStruct>(currentScope->owner, AstNodeKind::StructDecl);
        if (parentStruct->genericParameters)
        {
            structNode->genericParameters = Ast::clone(parentStruct->genericParameters, structNode, AST_GENERATED_GENERIC_PARAM);
            structNode->flags |= AST_IS_GENERIC | AST_NO_BYTECODE;
        }
    }

    return doStructContent(structNode, structType);
}

bool SyntaxJob::doStructContent(AstStruct* structNode, SyntaxStructType structType)
{
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(g_E[Err0444], token.ctext())));
    structNode->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(structNode));

    // If name starts with "__", then this is generated, as a user identifier cannot start with those
    // two characters
    if (structNode->token.text.length() > 2 && structNode->token.text[0] == '_' && structNode->token.text[1] == '_')
        structNode->flags |= AST_GENERATED;

    // Add struct type and scope
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, currentScope, true);
        if (newScope->kind != ScopeKind::Struct)
        {
            if (newScope->owner->kind == AstNodeKind::Impl)
            {
                auto       implNode = CastAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic diag{implNode->identifier, Fmt(g_E[Err0441], Scope::getNakedKindName(newScope->kind), implNode->token.ctext(), Scope::getNakedKindName(ScopeKind::Struct))};
                Diagnostic note{structNode, Fmt(g_E[Nte0027], implNode->token.ctext()), DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
            else
            {
                Diagnostic diag{structNode, Fmt(g_E[Err0885], structNode->token.ctext(), Scope::getArticleKindName(newScope->kind))};
                Diagnostic note{newScope->owner, g_E[Nte0036], DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }
        }

        structNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock      lk1(newScope->owner->mutex);
        TypeInfoStruct* typeInfo = nullptr;
        if (!newScope->owner->typeInfo)
        {
            typeInfo                  = allocType<TypeInfoStruct>();
            newScope->owner->typeInfo = typeInfo;
        }
        else
        {
            typeInfo = CastTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, newScope->owner->typeInfo->kind);
        }

        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Struct);
        structNode->typeInfo = newScope->owner->typeInfo;
        typeInfo->declNode   = structNode;
        newScope->owner      = structNode;
        typeInfo->name       = structNode->token.text;
        typeInfo->structName = structNode->token.text;
        typeInfo->scope      = newScope;

        SymbolKind symbolKind = SymbolKind::Struct;
        switch (structType)
        {
        case SyntaxStructType::Interface:
            symbolKind = SymbolKind::Interface;
            break;
        }

        structNode->resolvedSymbolName = currentScope->symTable.registerSymbolNameNoLock(&context, structNode, symbolKind);
    }

    // Dispatch owners
    if (structNode->genericParameters)
    {
        Ast::visit(structNode->genericParameters, [&](AstNode* n)
                   {
                       n->ownerStructScope = newScope;
                       n->ownerScope       = newScope;
                       n->flags |= AST_IS_GENERIC;
                       if (n->kind == AstNodeKind::FuncDeclParam)
                       {
                           auto param = CastAst<AstVarDecl>(n, AstNodeKind::FuncDeclParam);
                           newScope->symTable.registerSymbolName(&context, n, param->type ? SymbolKind::Variable : SymbolKind::GenericType);
                       }
                   });
    }

    SWAG_CHECK(eatToken());

    // Content of struct
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);

        auto contentNode    = Ast::newNode<AstNode>(this, AstNodeKind::StructContent, sourceFile, structNode);
        structNode->content = contentNode;
        contentNode->allocateExtension();
        contentNode->extension->semanticBeforeFct = SemanticJob::preResolveStructContent;

        if (structType == SyntaxStructType::Tuple)
            SWAG_CHECK(doStructBodyTuple(contentNode, true));
        else
        {
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
            while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
                SWAG_CHECK(doStructBody(contentNode, structType));
            SWAG_CHECK(eatToken(TokenId::SymRightCurly));
        }
    }

    return true;
}

bool SyntaxJob::doStructBodyTuple(AstNode* parent, bool acceptEmpty)
{
    auto curly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Tuple without a content
    if (token.id == TokenId::SymRightCurly)
    {
        if (acceptEmpty)
        {
            SWAG_CHECK(eatToken(TokenId::SymRightCurly));
            return true;
        }

        Diagnostic diag{sourceFile, token, Fmt(g_E[Err0447], token.ctext())};
        Diagnostic note{sourceFile, curly, g_E[Hlp0003], DiagnosticLevel::Help};
        return sourceFile->report(diag, &note);
    }

    int idx = 0;
    while (token.id != TokenId::EndOfFile)
    {
        auto structFieldNode = Ast::newVarDecl(sourceFile, "", parent, nullptr);
        structFieldNode->flags |= AST_GENERATED;

        AstTypeExpression* typeExpression = nullptr;
        AstNode*           expression;
        Token              prevToken = token;
        SWAG_CHECK(doTypeExpression(parent, &expression));
        Ast::removeFromParent(expression);

        // Name followed by ':'
        if (token.id == TokenId::SymColon)
        {
            typeExpression = CastAst<AstTypeExpression>(expression, AstNodeKind::TypeExpression);
            SWAG_VERIFY(prevToken.id == TokenId::Identifier, error(prevToken, g_E[Err0448]));
            SWAG_ASSERT(typeExpression->identifier);
            SWAG_CHECK(checkIsSingleIdentifier(typeExpression->identifier, "as a tuple field name"));
            SWAG_CHECK(checkIsValidVarName(typeExpression->identifier->childs.back()));
            structFieldNode->token.text = typeExpression->identifier->childs.back()->token.text;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(structFieldNode, &structFieldNode->type));
            expression = structFieldNode->type;
        }
        else
        {
            Ast::addChildBack(structFieldNode, expression);
            structFieldNode->type       = expression;
            structFieldNode->token.text = Fmt("item%u", idx);
            structFieldNode->flags |= AST_AUTO_NAME;
        }

        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(structFieldNode, EXPR_FLAG_NONE, &structFieldNode->assignment));
        }

        idx++;

        SWAG_VERIFY(token.id == TokenId::SymComma || token.id == TokenId::SymRightCurly, error(token, Fmt(g_E[Err0449], token.ctext())));
        if (token.id == TokenId::SymRightCurly)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatToken(TokenId::SymRightCurly, "after tuple type expression"));
    return true;
}

bool SyntaxJob::doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
    {
        auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(stmt, structType));
        SWAG_CHECK(eatToken(TokenId::SymRightCurly));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        return true;
    }

    switch (token.id)
    {
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent, result));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(doCompilerRunEmbedded(parent, result));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent, result));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;
    case TokenId::CompilerIf:
        SWAG_CHECK(doCompilerIfFor(parent, result, AstNodeKind::StructDecl));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doStructBody(parent, structType));
        break;

    case TokenId::SymAttrStart:
    {
        AstAttrUse* attrUse;
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        SWAG_CHECK(doStructBody(attrUse, structType, &attrUse->content));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        if (attrUse->content)
            attrUse->content->setOwnerAttrUse(attrUse);
        break;
    }

    // Sub definitions
    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent, result));
        break;
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdInterface:
        SWAG_CHECK(doStruct(parent, result));
        break;

    // A user alias
    case TokenId::KwdAlias:
        SWAG_CHECK(doAlias(parent, result));
        break;

    // Using on a struct member
    case TokenId::KwdUsing:
    {
        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        SWAG_VERIFY(structType != SyntaxStructType::Interface, sourceFile->report({parent, token, g_E[Err0451]}));
        SWAG_CHECK(eatToken());
        auto structNode = CastAst<AstStruct>(parent->ownerStructScope->owner, AstNodeKind::StructDecl);
        structNode->specFlags |= AST_SPEC_STRUCTDECL_HAS_USING;
        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(parent, &varDecl, AstNodeKind::VarDecl));
        varDecl->flags |= AST_DECL_USING;
        if (result)
            *result = varDecl;
        break;
    }

    case TokenId::KwdConst:
    {
        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::ConstDecl));
        break;
    }

    case TokenId::KwdVar:
    {
        PushErrHint errh(g_E[Hnt0026]);
        return sourceFile->report({parent, token, g_E[Err0453]});
    }

    // A normal declaration
    default:
    {
        // If this a function call ?
        // (a mixin)
        bool isFunc = false;
        if (token.id == TokenId::Identifier)
        {
            tokenizer.saveState(token);
            eatToken();
            if (token.id == TokenId::SymDot || token.id == TokenId::SymLeftParen)
                isFunc = true;
            tokenizer.restoreState(token);
        }

        if (isFunc)
        {
            ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
            parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
            AstNode* idRef = nullptr;
            SWAG_CHECK(doIdentifierRef(parent, &idRef));
            if (result)
                *result = idRef;
            idRef->flags |= AST_GLOBAL_MIXIN_CALL;
        }
        else
        {
            ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
            SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::VarDecl));
        }

        break;
    }
    }

    return true;
}
