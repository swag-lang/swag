#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Naming.h"
#include "Scoped.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Parser::doImpl(AstNode* parent, AstNode** result)
{
    auto implNode         = Ast::newNode<AstImpl>(this, AstNodeKind::Impl, sourceFile, parent);
    *result               = implNode;
    implNode->semanticFct = Semantic::resolveImpl;

    SWAG_VERIFY(module->acceptsCompileImpl, context->report({implNode, Err(Err0296)}));

    auto scopeKind = ScopeKind::Struct;
    SWAG_CHECK(eatToken());
    const auto kindLoc = token;
    switch (token.id)
    {
    case TokenId::KwdEnum:
        scopeKind = ScopeKind::Enum;
        SWAG_CHECK(eatToken());
        break;
    default:
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
            Diagnostic diag{implNode, token, Err(Err0666)};
            diag.addRange(kindLoc, Nte(Nte0052));
            return context->report(diag);
        }

        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifierRef(implNode, &implNode->identifierFor, IDENTIFIER_NO_FCT_PARAMS));
        implNode->identifierFor->allocateExtension(ExtensionKind::Semantic);
        implNode->identifierFor->extSemantic()->semanticAfterFct = Semantic::resolveImplForAfterFor;
        implNode->semanticFct                                    = Semantic::resolveImplFor;
        implNode->allocateExtension(ExtensionKind::Semantic);
        implNode->extSemantic()->semanticAfterFct = Semantic::resolveImplForType;
        identifierStruct                          = implNode->identifierFor;
        implInterface                             = true;

        const auto last = castAst<AstIdentifier>(identifierStruct->childs.back(), AstNodeKind::Identifier);
        SWAG_VERIFY(!last->genericParameters, context->report({last->genericParameters, Err(Err0686)}));
    }
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(implNode->identifier, "as an [[impl]] block name"));
    }

    // Content of impl block
    const auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[impl]] body"));

    // Get existing scope or create a new one
    const auto& structName = identifierStruct->childs.back()->token.text;
    implNode->token.text   = structName;

    const auto newScope = Ast::newScope(implNode, structName, scopeKind, currentScope, true);
    if (scopeKind != newScope->kind)
    {
        const Diagnostic  diag{implNode, FMT(Err(Err0008), Naming::kindName(scopeKind).c_str(), implNode->token.ctext(), Naming::kindName(newScope->kind).c_str())};
        const auto        note  = Diagnostic::hereIs(newScope->owner);
        const Diagnostic* note1 = nullptr;
        if (newScope->kind == ScopeKind::Enum)
            note1 = Diagnostic::note(FMT(Nte(Nte0043), implNode->token.ctext()));
        else if (newScope->kind == ScopeKind::Struct)
            note1 = Diagnostic::note(FMT(Nte(Nte0042), implNode->token.ctext()));
        return context->report(diag, note, note1);
    }

    implNode->structScope = newScope;

    // Be sure we have associated a struct typeinfo (we can parse an impl block before the corresponding struct)
    {
        ScopedLock lk1(newScope->owner->mutex);
        const auto typeInfo = newScope->owner->typeInfo;
        if (!typeInfo)
        {
            if (scopeKind == ScopeKind::Enum)
            {
                const auto typeEnum       = makeType<TypeInfoEnum>();
                typeEnum->scope           = newScope;
                typeEnum->name            = structName;
                newScope->owner->typeInfo = typeEnum;
            }
            else
            {
                const auto typeStruct     = makeType<TypeInfoStruct>();
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

        const auto typeStruct = castTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, TypeInfoKind::Struct);
        typeStruct->cptRemainingInterfacesReg++;
        typeStruct->cptRemainingInterfaces++;

        if (implNode->hasExtOwner() && implNode->extOwner()->ownerCompilerIfBlock)
            implNode->extOwner()->ownerCompilerIfBlock->interfacesCount.push_back(typeStruct);
    }

    // For an interface implementation, creates a sub scope named like the interface
    // :SubScopeImplFor
    auto parentScope = newScope;
    if (implInterface)
    {
        ScopedLock lk(newScope->symTable.mutex);
        Utf8       itfName  = implNode->identifier->childs.back()->token.text;
        const auto symbol   = newScope->symTable.findNoLock(itfName);
        Scope*     subScope = nullptr;
        if (!symbol)
        {
            subScope = Ast::newScope(implNode, itfName, ScopeKind::Impl, newScope, false);

            // :FakeImplForType
            const auto typeInfo  = makeType<TypeInfoStruct>();
            typeInfo->name       = implNode->identifier->childs.back()->token.text;
            typeInfo->structName = typeInfo->name;
            typeInfo->scope      = subScope;
            typeInfo->declNode   = implNode;

            AddSymbolTypeInfo toAdd;
            toAdd.node       = implNode;
            toAdd.typeInfo   = typeInfo;
            toAdd.kind       = SymbolKind::Struct;
            toAdd.flags      = OVERLOAD_IMPL_IN_STRUCT;
            toAdd.aliasName  = &itfName;
            toAdd.symbolName = newScope->symTable.registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, &itfName);
            newScope->symTable.addSymbolTypeInfoNoLock(context, toAdd);
        }
        else
        {
            const auto typeInfo = castTypeInfo<TypeInfoStruct>(symbol->overloads[0]->typeInfo, TypeInfoKind::Struct);
            subScope            = typeInfo->scope;
        }

        parentScope     = subScope;
        implNode->scope = subScope;
    }

    {
        Scoped       scoped(this, parentScope);
        ScopedStruct scopedStruct(this, newScope);
        ScopedFlags  scopedFlags(this, AST_IN_IMPL);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(implNode, &dummyResult));
        }
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[impl]] body"));
    return true;
}

bool Parser::doStruct(AstNode* parent, AstNode** result)
{
    const auto structNode   = Ast::newNode<AstStruct>(this, AstNodeKind::StructDecl, sourceFile, parent);
    *result                 = structNode;
    structNode->semanticFct = Semantic::resolveStruct;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticAfterFct = Semantic::sendCompilerMsgTypeDecl;

    if (!tokenizer.comment.empty())
    {
        structNode->allocateExtension(ExtensionKind::Misc);
        structNode->extMisc()->docComment = std::move(tokenizer.comment);
    }

    // Special case
    auto structType = SyntaxStructType::Struct;
    if (token.id == TokenId::KwdInterface)
    {
        structType              = SyntaxStructType::Interface;
        structNode->kind        = AstNodeKind::InterfaceDecl;
        structNode->semanticFct = Semantic::resolveInterface;
    }
    else if (token.id == TokenId::KwdUnion)
    {
        structNode->addSpecFlag(AstStruct::SPECFLAG_UNION);
    }

    SWAG_CHECK(eatToken());

    // Generic arguments
    if (token.id == TokenId::SymLeftParen)
    {
        SWAG_CHECK(doGenericDeclParameters(structNode, &structNode->genericParameters));
        structNode->flags |= AST_IS_GENERIC | AST_NO_BYTECODE;
    }

    // If a struct is declared inside a generic struct, force the sub struct to have generic parameters
    else if (currentScope && currentScope->kind == ScopeKind::Struct && currentScope->owner->kind == AstNodeKind::StructDecl)
    {
        const auto parentStruct = castAst<AstStruct>(currentScope->owner, AstNodeKind::StructDecl);
        if (parentStruct->genericParameters)
        {
            structNode->genericParameters = Ast::clone(parentStruct->genericParameters, structNode, AST_GENERATED_GENERIC_PARAM);
            structNode->flags |= AST_IS_GENERIC | AST_NO_BYTECODE;
        }
    }

    SWAG_CHECK(doStructContent(structNode, structType));
    return true;
}

bool Parser::doStructContent(AstStruct* structNode, SyntaxStructType structType)
{
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(token, Err(Err0581)));
    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0378), token.ctext())));
    structNode->inheritTokenName(token);
    structNode->tokenName = static_cast<Token>(token);
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
                const auto       implNode = castAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                const Diagnostic diag{implNode->identifier,
                                      FMT(Err(Err0008), Naming::kindName(newScope->kind).c_str(), implNode->token.ctext(), Naming::kindName(ScopeKind::Struct).c_str())};
                const auto note = Diagnostic::hereIs(structNode);
                return context->report(diag, note);
            }
            const Utf8       asA = FMT("as %s", Naming::aKindName(newScope->kind).c_str());
            const Diagnostic diag{structNode->sourceFile, token, FMT(Err(Err0626), "struct", structNode->token.ctext(), asA.c_str())};
            const auto       note = Diagnostic::note(newScope->owner, newScope->owner->getTokenName(), Nte(Nte0071));
            return context->report(diag, note);
        }

        structNode->scope = newScope;

        // If an 'impl' came first, then typeinfo has already been defined
        ScopedLock      lk1(newScope->owner->mutex);
        TypeInfoStruct* typeInfo = nullptr;
        if (!newScope->owner->typeInfo)
        {
            typeInfo                  = makeType<TypeInfoStruct>();
            newScope->owner->typeInfo = typeInfo;
        }
        else
        {
            typeInfo = castTypeInfo<TypeInfoStruct>(newScope->owner->typeInfo, newScope->owner->typeInfo->kind);
        }

        SWAG_ASSERT(typeInfo->isStruct());
        structNode->typeInfo = newScope->owner->typeInfo;
        typeInfo->declNode   = structNode;
        newScope->owner      = structNode;
        typeInfo->name       = structNode->token.text;
        typeInfo->structName = structNode->token.text;
        typeInfo->scope      = newScope;

        auto symbolKind = SymbolKind::Struct;
        switch (structType)
        {
        case SyntaxStructType::Interface:
            symbolKind = SymbolKind::Interface;
            break;
        default:
            break;
        }

        structNode->resolvedSymbolName = currentScope->symTable.registerSymbolNameNoLock(context, structNode, symbolKind);
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
                const auto param = castAst<AstVarDecl>(n, AstNodeKind::FuncDeclParam);
                newScope->symTable.registerSymbolName(context, n, param->type ? SymbolKind::Variable : SymbolKind::GenericType);
            }
        });
    }

    SWAG_CHECK(eatToken());

    // '#validif' block
    if (token.id == TokenId::CompilerValidIfx)
        return error(token, Err(Err0659));
    if (token.id == TokenId::CompilerValidIf)
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);
        SWAG_CHECK(doCompilerValidIf(structNode, &structNode->validif));
    }

    // Content of struct
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);

        const auto contentNode = Ast::newNode<AstNode>(this, AstNodeKind::StructContent, sourceFile, structNode);
        structNode->content    = contentNode;
        contentNode->allocateExtension(ExtensionKind::Semantic);
        contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;

        const auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[struct]] body"));
        while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(contentNode, structType, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[struct]] body"));
    }

    return true;
}

bool Parser::doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result)
{
    const auto startLoc = token.startLocation;
    if (token.id == TokenId::SymLeftCurly)
    {
        const auto stmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        *result         = stmt;
        SWAG_CHECK(eatToken());
        while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(stmt, structType, &dummyResult));
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
        SWAG_CHECK(doCompilerIfFor(parent, result, structType == SyntaxStructType::Interface ? AstNodeKind::InterfaceDecl : AstNodeKind::StructDecl));
        parent->ownerStructScope->owner->flags |= AST_STRUCT_COMPOUND;
        break;

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doStructBody(parent, structType, &dummyResult));
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
    case TokenId::KwdTypeAlias:
        SWAG_CHECK(doTypeAlias(parent, result));
        break;
    case TokenId::KwdNameAlias:
        SWAG_CHECK(doNameAlias(parent, result));
        break;

    // Using on a struct member
    case TokenId::KwdUsing:
    {
        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        SWAG_VERIFY(structType != SyntaxStructType::Interface, context->report({parent, token, Err(Err0478)}));
        SWAG_CHECK(eatToken());
        const auto structNode = castAst<AstStruct>(parent->ownerStructScope->owner, AstNodeKind::StructDecl);
        structNode->addSpecFlag(AstStruct::SPECFLAG_HAS_USING);
        AstNode* varDecl;
        SWAG_CHECK(doVarDecl(parent, &varDecl, AstNodeKind::VarDecl, true));
        varDecl->flags |= AST_DECL_USING;
        *result = varDecl;
        break;
    }

    case TokenId::KwdConst:
    {
        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::ConstDecl, true));
        break;
    }

    case TokenId::KwdVar:
    {
        const Diagnostic diag{parent, token, Err(Err0672)};
        return context->report(diag);
    }

    case TokenId::KwdMethod:
    case TokenId::KwdFunc:
    {
        SWAG_VERIFY(structType == SyntaxStructType::Interface, error(token, Err(Err0503)));

        const auto kind = token.id;
        SWAG_CHECK(eatToken());

        SWAG_VERIFY(token.id != TokenId::SymLeftParen, error(token, Err(Err0685)));

        const bool isMethod      = kind == TokenId::KwdMethod;
        bool       isConstMethod = false;

        if (token.id == TokenId::KwdConst)
        {
            SWAG_VERIFY(isMethod, error(token, Err(Err0458)));
            isConstMethod = true;
            SWAG_CHECK(eatToken());
        }

        const auto funcNode = Ast::newNode<AstFuncDecl>(this, AstNodeKind::FuncDecl, sourceFile, nullptr);

        SWAG_CHECK(checkIsValidUserName(funcNode));
        SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0295), token.ctext())));
        SWAG_CHECK(eatToken());

        const auto scope = Ast::newScope(funcNode, "", ScopeKind::Function, parent->ownerStructScope);

        {
            Scoped scoped(this, scope);
            SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, false, nullptr, isMethod, isConstMethod, true));
        }

        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        const auto  varNode = Ast::newVarDecl(sourceFile, funcNode->token.text, parent, this);
        varNode->inheritTokenLocation(funcNode->token);
        Semantic::setVarDeclResolve(varNode);
        varNode->flags |= AST_R_VALUE;

        const auto typeNode   = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, varNode);
        typeNode->semanticFct = Semantic::resolveTypeLambdaClosure;
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
            SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeNode->returnType));

            const auto retNode = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclType, sourceFile, funcNode);
            retNode->addSpecFlag(AstFuncDecl::SPECFLAG_RETURN_DEFINED);
            funcNode->returnType = retNode;
            funcNode->returnType->allocateExtension(ExtensionKind::Misc);
            funcNode->returnType->extMisc()->exportNode = typeNode->returnType;

            CloneContext cloneContext;
            cloneContext.parent = retNode;
            typeNode->returnType->clone(cloneContext);
        }

        if (token.id == TokenId::KwdThrow)
        {
            SWAG_CHECK(eatToken());
            typeNode->addSpecFlag(AstTypeLambda::SPECFLAG_CAN_THROW);
            funcNode->addSpecFlag(AstFuncDecl::SPECFLAG_THROW);
        }

        varNode->allocateExtension(ExtensionKind::Misc);
        varNode->extMisc()->exportNode = funcNode;

        if (!tokenizer.comment.empty())
        {
            varNode->allocateExtension(ExtensionKind::Misc);
            varNode->extMisc()->docComment = std::move(tokenizer.comment);
        }

        SWAG_CHECK(eatSemiCol("interface function definition"));
        *result = varNode;
        break;
    }

    // A normal declaration
    default:
    {
        SWAG_VERIFY(structType != SyntaxStructType::Interface, error(token, FMT(Err(Err0293), token.ctext())));
        ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
        SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::VarDecl, true));
        break;
    }
    }

    return true;
}
