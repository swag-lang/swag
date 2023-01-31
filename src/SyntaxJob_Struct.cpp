#include "pch.h"
#include "Ast.h"
#include "Scoped.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Module.h"

bool SyntaxJob::doImpl(AstNode* parent, AstNode** result)
{
    auto implNode         = Ast::newNode<AstImpl>(this, AstNodeKind::Impl, sourceFile, parent);
    implNode->semanticFct = SemanticJob::resolveImpl;
    if (result)
        *result = implNode;

    SWAG_VERIFY(module->acceptsCompileImpl, Report::report({implNode, Err(Syn0104)}));

    auto scopeKind = ScopeKind::Struct;
    SWAG_CHECK(eatToken());
    auto kindLoc = token;
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
        if (scopeKind == ScopeKind::Enum)
        {
            Diagnostic diag{implNode, token, Err(Syn0143)};
            diag.hint = Hnt(Hnt0061);
            diag.addRange(kindLoc, Hnt(Hnt0085));
            return Report::report(diag);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifierFor, IDENTIFIER_NO_FCT_PARAMS));
        implNode->identifierFor->allocateExtension(ExtensionKind::Semantic);
        implNode->identifierFor->extension->semantic->semanticAfterFct = SemanticJob::resolveImplForAfterFor;
        implNode->semanticFct                                          = SemanticJob::resolveImplFor;
        implNode->allocateExtension(ExtensionKind::Semantic);
        implNode->extension->semantic->semanticAfterFct = SemanticJob::resolveImplForType;
        identifierStruct                                = implNode->identifierFor;
        implInterface                                   = true;

        auto last = CastAst<AstIdentifier>(identifierStruct->childs.back(), AstNodeKind::Identifier);
        SWAG_VERIFY(!last->genericParameters, Report::report({last->genericParameters, Err(Syn0162)}));
    }
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(implNode->identifier, "as an 'impl' block name"));
    }

    // Content of impl block
    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the 'impl' body"));

    // Get existing scope or create a new one
    auto& structName     = identifierStruct->childs.back()->token.text;
    implNode->token.text = structName;

    auto newScope = Ast::newScope(implNode, structName, scopeKind, currentScope, true);
    if (scopeKind != newScope->kind)
    {
        Diagnostic diag{implNode, Fmt(Err(Syn0123), Scope::getNakedKindName(scopeKind), implNode->token.ctext(), Scope::getNakedKindName(newScope->kind))};
        if (newScope->kind == ScopeKind::Enum)
            diag.hint = Fmt(Hnt(Hnt0019), implNode->token.ctext());
        else if (newScope->kind == ScopeKind::Struct)
            diag.hint = Fmt(Hnt(Hnt0020), implNode->token.ctext());
        Diagnostic note{newScope->owner, newScope->owner->token, Fmt(Nte(Nte0027), implNode->token.ctext()), DiagnosticLevel::Note};
        return Report::report(diag, &note);
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
    // :SubScopeImplFor
    auto parentScope = newScope;
    if (implInterface)
    {
        ScopedLock lk(newScope->symTable.mutex);
        Utf8       itfName  = implNode->identifier->childs.back()->token.text;
        auto       symbol   = newScope->symTable.findNoLock(itfName);
        Scope*     subScope = nullptr;
        if (!symbol)
        {
            subScope = Ast::newScope(implNode, itfName, ScopeKind::Impl, newScope, false);

            // :FakeImplForType
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

    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the 'impl' body"));
    return true;
}

bool SyntaxJob::doStruct(AstNode* parent, AstNode** result)
{
    auto structNode         = Ast::newNode<AstStruct>(this, AstNodeKind::StructDecl, sourceFile, parent);
    structNode->semanticFct = SemanticJob::resolveStruct;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extension->semantic->semanticAfterFct = SemanticJob::sendCompilerMsgTypeDecl;
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

    SWAG_CHECK(doStructContent(structNode, structType));
    return true;
}

bool SyntaxJob::doStructContent(AstStruct* structNode, SyntaxStructType structType)
{
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0063), token.ctext())));
    structNode->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(structNode, &token));

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
                Diagnostic diag{implNode->identifier, Fmt(Err(Syn0123), Scope::getNakedKindName(newScope->kind), implNode->token.ctext(), Scope::getNakedKindName(ScopeKind::Struct))};
                Diagnostic note{structNode, Fmt(Nte(Nte0027), implNode->token.ctext()), DiagnosticLevel::Note};
                return Report::report(diag, &note);
            }
            else
            {
                Diagnostic diag{structNode->sourceFile, token, Fmt(Err(Err0394), structNode->token.ctext(), Scope::getArticleKindName(newScope->kind))};
                Diagnostic note{newScope->owner, Nte(Nte0036), DiagnosticLevel::Note};
                return Report::report(diag, &note);
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

        SWAG_ASSERT(typeInfo->isStruct());
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
                       } });
    }

    SWAG_CHECK(eatToken());

    // Content of struct
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);

        auto contentNode    = Ast::newNode<AstNode>(this, AstNodeKind::StructContent, sourceFile, structNode);
        structNode->content = contentNode;
        contentNode->allocateExtension(ExtensionKind::Semantic);
        contentNode->extension->semantic->semanticBeforeFct = SemanticJob::preResolveStructContent;

        if (structType == SyntaxStructType::Tuple)
            SWAG_CHECK(doTupleBody(contentNode, true));
        else
        {
            auto startLoc = token.startLocation;
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the struct body"));
            while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
                SWAG_CHECK(doStructBody(contentNode, structType));
            SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the struct body"));
        }
    }

    return true;
}

bool SyntaxJob::doTupleBody(AstNode* parent, bool acceptEmpty)
{
    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the tuple body"));

    // Tuple without a content
    if (token.id == TokenId::SymRightCurly)
    {
        if (acceptEmpty)
        {
            SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, nullptr));
            return true;
        }

        Diagnostic diag{sourceFile, token, Fmt(Err(Syn0046), token.ctext())};
        return Report::report(diag);
    }

    bool        lastWasAlone  = false;
    bool        curIsAlone    = false;
    AstVarDecl* lastParameter = nullptr;
    bool        thisIsAType   = false;

    int idx = 0;
    while (token.id != TokenId::EndOfFile)
    {
        curIsAlone = true;

        Token constToken;
        if (token.id == TokenId::CompilerType)
        {
            thisIsAType = true;
            curIsAlone  = false;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0196), token.ctext())));
        }

        // Accept a parameter name
        Token namedParam;
        if (token.id == TokenId::Identifier)
        {
            auto tokenName = token;
            tokenizer.saveState(token);
            SWAG_CHECK(eatToken());
            if (token.id != TokenId::SymColon)
            {
                tokenizer.restoreState(token);
            }
            else
            {
                namedParam = tokenName;
                SWAG_CHECK(eatToken());
                curIsAlone  = false;
                thisIsAType = false;
            }
        }
        else
        {
            thisIsAType = true;
            curIsAlone  = false;
        }

        auto structFieldNode = Ast::newVarDecl(sourceFile, "", parent, nullptr);
        structFieldNode->flags |= AST_GENERATED;
        SWAG_CHECK(doTypeExpression(structFieldNode, &structFieldNode->type));

        AstTypeExpression* typeExpr = CastAst<AstTypeExpression>(structFieldNode->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            if (!testIsSingleIdentifier(typeExpr->identifier))
            {
                thisIsAType = true;
                curIsAlone  = false;
            }
        }

        if (namedParam.text.empty())
        {
            structFieldNode->token.text = Fmt("item%u", idx);
            structFieldNode->flags |= AST_AUTO_NAME;
        }
        else
        {
            structFieldNode->token = namedParam;
        }

        if (token.id == TokenId::SymEqual)
        {
            thisIsAType = false;
            curIsAlone  = false;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doExpression(structFieldNode, EXPR_FLAG_NONE, &structFieldNode->assignment));
        }

        if (lastWasAlone && !curIsAlone && !thisIsAType)
        {
            Token tokenAmb         = structFieldNode->token;
            tokenAmb.startLocation = lastParameter->token.startLocation;
            tokenAmb.endLocation   = token.startLocation;

            Diagnostic diag{sourceFile, tokenAmb, Err(Syn0198)};
            Diagnostic note{lastParameter, Fmt(Nte(Nte0077), lastParameter->type->token.ctext()), DiagnosticLevel::Note};
            note.hint = Fmt(Hnt(Hnt0103), lastParameter->type->token.ctext());
            context.report(diag, &note);

            return false;
        }

        lastWasAlone  = curIsAlone;
        lastParameter = structFieldNode;

        idx++;

        SWAG_VERIFY(token.id == TokenId::SymComma || token.id == TokenId::SymRightCurly, error(token, Fmt(Err(Syn0175), token.ctext())));
        if (token.id == TokenId::SymRightCurly)
            break;
        SWAG_CHECK(eatToken());
    }

    parent->token.endLocation = token.endLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the tuple body"));
    return true;
}

bool SyntaxJob::doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result)
{
    auto startLoc = token.startLocation;
    if (token.id == TokenId::SymLeftCurly)
    {
        auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(stmt, structType));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the struct body"));
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
    case TokenId::CompilerError:
        SWAG_CHECK(doCompilerError(parent, result));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;
    case TokenId::CompilerWarning:
        SWAG_CHECK(doCompilerWarning(parent, result));
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
        SWAG_VERIFY(structType != SyntaxStructType::Interface, Report::report({parent, token, Err(Syn0029)}));
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
        Diagnostic diag{parent, token, Err(Syn0030)};
        diag.hint = Hnt(Hnt0026);
        return Report::report(diag);
    }

    case TokenId::KwdMethod:
    case TokenId::KwdConstMethod:
    case TokenId::KwdFunc:
    {
        SWAG_VERIFY(structType == SyntaxStructType::Interface, error(token, Err(Syn0163)));

        auto kind = token.id;
        SWAG_CHECK(eatToken());

        auto funcNode = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, nullptr);
        SWAG_CHECK(checkIsValidUserName(funcNode));
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0089), token.ctext())));
        SWAG_CHECK(eatToken());

        auto scope = Ast::newScope(funcNode, "", ScopeKind::Function, parent->ownerStructScope);

        {
            Scoped scoped(this, scope);
            SWAG_CHECK(doFuncDeclParameters(funcNode,
                                            &funcNode->parameters,
                                            false,
                                            nullptr,
                                            kind == TokenId::KwdMethod || kind == TokenId::KwdConstMethod,
                                            kind == TokenId::KwdConstMethod,
                                            true));
        }

        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        auto        varNode = Ast::newVarDecl(sourceFile, funcNode->token.text, parent, this);
        varNode->inheritTokenLocation(funcNode->token);
        varNode->specFlags |= AST_SPEC_VARDECL_GEN_ITF;
        SemanticJob::setVarDeclResolve(varNode);
        varNode->flags |= AST_R_VALUE;

        auto typeNode         = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, varNode);
        typeNode->semanticFct = SemanticJob::resolveTypeLambdaClosure;
        varNode->type         = typeNode;
        varNode->type->inheritTokenLocation(funcNode->token);
        Ast::removeFromParent(funcNode->parameters);
        Ast::addChildFront(typeNode, funcNode->parameters);
        typeNode->parameters = funcNode->parameters;
        if (typeNode->parameters)
            typeNode->parameters->flags |= AST_IN_TYPE_VAR_DECLARATION;

        if (token.id == TokenId::SymMinusGreat)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(typeNode, &typeNode->returnType));
        }

        if (token.id == TokenId::KwdThrow)
        {
            SWAG_CHECK(eatToken(TokenId::KwdThrow));
            typeNode->specFlags |= AST_SPEC_TYPELAMBDA_CANTHROW;
        }

        break;
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
            idRef->flags |= AST_GLOBAL_CALL;
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
