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
    auto implNode         = Ast::newNode<AstImpl>(AstNodeKind::Impl, this, parent);
    *result               = implNode;
    implNode->semanticFct = Semantic::resolveImpl;

    SWAG_VERIFY(module->acceptsCompileImpl, context->report({implNode, toErr(Err0296)}));

    auto scopeKind = ScopeKind::Struct;
    SWAG_CHECK(eatToken());
    const auto kindLoc = tokenParse;
    switch (tokenParse.token.id)
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
        implNode->addAstFlag(AST_NO_BYTECODE);
    }

    // impl TITI for TOTO syntax (interface implementation for a given struct)
    bool implInterface    = false;
    auto identifierStruct = implNode->identifier;
    if (tokenParse.is(TokenId::KwdFor))
    {
        if (scopeKind == ScopeKind::Enum)
        {
            Diagnostic err{implNode, tokenParse.token, toErr(Err0666)};
            err.addNote(kindLoc.token, toNte(Nte0052));
            return context->report(err);
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

        const auto last = castAst<AstIdentifier>(identifierStruct->lastChild(), AstNodeKind::Identifier);
        SWAG_VERIFY(!last->genericParameters, context->report({last->genericParameters, toErr(Err0686)}));
    }
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(implNode->identifier, "as an [[impl]] block name"));
    }

    // Content of impl block
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[impl]] body"));

    // Get existing scope or create a new one
    const auto& structName = identifierStruct->lastChild()->token.text;
    implNode->token.text   = structName;

    const auto newScope = Ast::newScope(implNode, structName, scopeKind, currentScope, true);
    if (scopeKind != newScope->kind)
    {
        Diagnostic err{implNode, formErr(Err0008, Naming::kindName(scopeKind).c_str(), implNode->token.c_str(), Naming::kindName(newScope->kind).c_str())};
        err.addNote(Diagnostic::hereIs(newScope->owner));
        if (newScope->is(ScopeKind::Enum))
            err.addNote(formNte(Nte0043, implNode->token.c_str()));
        else if (newScope->is(ScopeKind::Struct))
            err.addNote(formNte(Nte0042, implNode->token.c_str()));
        return context->report(err);
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

        if (implNode->hasOwnerCompilerIfBlock())
            implNode->ownerCompilerIfBlock()->interfacesCount.push_back(typeStruct);
    }

    // For an interface implementation, creates a sub scope named like the interface
    // :SubScopeImplFor
    auto parentScope = newScope;
    if (implInterface)
    {
        ScopedLock lk(newScope->symTable.mutex);
        const Utf8 itfName  = implNode->identifier->lastChild()->token.text;
        const auto symbol   = newScope->symTable.findNoLock(itfName);
        Scope*     subScope = nullptr;
        if (!symbol)
        {
            subScope = Ast::newScope(implNode, itfName, ScopeKind::Impl, newScope, false);

            // :FakeImplForType
            const auto typeInfo  = makeType<TypeInfoStruct>();
            typeInfo->name       = implNode->identifier->lastChild()->token.text;
            typeInfo->structName = typeInfo->name;
            typeInfo->scope      = subScope;
            typeInfo->declNode   = implNode;

            AddSymbolTypeInfo toAdd;
            toAdd.node       = implNode;
            toAdd.typeInfo   = typeInfo;
            toAdd.kind       = SymbolKind::Struct;
            toAdd.flags      = OVERLOAD_IMPL_IN_STRUCT;
            toAdd.aliasName  = itfName;
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
        while (tokenParse.isNot(TokenId::EndOfFile) && tokenParse.isNot(TokenId::SymRightCurly))
        {
            SWAG_CHECK(doTopLevelInstruction(implNode, &dummyResult));
        }
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[impl]] body"));
    return true;
}

bool Parser::doStruct(AstNode* parent, AstNode** result)
{
    const auto structNode   = Ast::newNode<AstStruct>(AstNodeKind::StructDecl, this, parent);
    *result                 = structNode;
    structNode->semanticFct = Semantic::resolveStruct;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticAfterFct = Semantic::sendCompilerMsgTypeDecl;

    if (!tokenParse.commentJustBefore.empty())
    {
        structNode->allocateExtension(ExtensionKind::Misc);
        structNode->extMisc()->commentJustBefore = std::move(tokenParse.commentJustBefore);
    }

    // Special case
    auto structType = SyntaxStructType::Struct;
    if (tokenParse.is(TokenId::KwdInterface))
    {
        structType              = SyntaxStructType::Interface;
        structNode->kind        = AstNodeKind::InterfaceDecl;
        structNode->semanticFct = Semantic::resolveInterface;
    }
    else if (tokenParse.is(TokenId::KwdUnion))
    {
        structNode->addSpecFlag(AstStruct::SPEC_FLAG_UNION);
    }

    SWAG_CHECK(eatToken());

    // Generic arguments
    if (tokenParse.is(TokenId::SymLeftParen))
    {
        SWAG_CHECK(doGenericDeclParameters(structNode, &structNode->genericParameters));
        structNode->addAstFlag(AST_IS_GENERIC | AST_NO_BYTECODE);
    }

    // If a struct is declared inside a generic struct, force the sub struct to have generic parameters
    else if (currentScope && currentScope->is(ScopeKind::Struct) && currentScope->owner->is(AstNodeKind::StructDecl))
    {
        const auto parentStruct = castAst<AstStruct>(currentScope->owner, AstNodeKind::StructDecl);
        if (parentStruct->genericParameters)
        {
            structNode->genericParameters = Ast::clone(parentStruct->genericParameters, structNode, AST_GENERATED_GENERIC_PARAM);
            structNode->addAstFlag(AST_IS_GENERIC | AST_NO_BYTECODE);
        }
    }

    SWAG_CHECK(doStructContent(structNode, structType));
    return true;
}

bool Parser::doStructContent(AstStruct* structNode, SyntaxStructType structType)
{
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse.token, toErr(Err0581)));
    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0378, tokenParse.token.c_str())));
    structNode->inheritTokenName(tokenParse.token);
    structNode->tokenName = tokenParse.token;
    SWAG_CHECK(checkIsValidUserName(structNode, &tokenParse.token));

    // If name starts with "__", then this is generated, as a user identifier cannot start with those
    // two characters
    if (structNode->token.text.length() > 2 && structNode->token.text[0] == '_' && structNode->token.text[1] == '_')
        structNode->addAstFlag(AST_GENERATED);

    // Add struct type and scope
    Scope* newScope = nullptr;
    {
        ScopedLock lk(currentScope->symTable.mutex);
        newScope = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, currentScope, true);
        if (newScope->isNot(ScopeKind::Struct))
        {
            if (newScope->owner->is(AstNodeKind::Impl))
            {
                const auto implNode = castAst<AstImpl>(newScope->owner, AstNodeKind::Impl);
                Diagnostic err{implNode->identifier, formErr(Err0008, Naming::kindName(newScope->kind).c_str(), implNode->token.c_str(), Naming::kindName(ScopeKind::Struct).c_str())};
                err.addNote(Diagnostic::hereIs(structNode));
                return context->report(err);
            }

            const Utf8 asA = form("as %s", Naming::aKindName(newScope->kind).c_str());
            Diagnostic err{structNode->token.sourceFile, tokenParse.token, formErr(Err0626, "struct", structNode->token.c_str(), asA.c_str())};
            err.addNote(newScope->owner, newScope->owner->getTokenName(), toNte(Nte0071));
            return context->report(err);
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

        structNode->setResolvedSymbolName(currentScope->symTable.registerSymbolNameNoLock(context, structNode, symbolKind));
    }

    // Dispatch owners
    if (structNode->genericParameters)
    {
        Ast::visit(structNode->genericParameters, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
            n->addAstFlag(AST_IS_GENERIC);
            if (n->is(AstNodeKind::FuncDeclParam))
            {
                const auto param = castAst<AstVarDecl>(n, AstNodeKind::FuncDeclParam);
                newScope->symTable.registerSymbolName(context, n, param->type ? SymbolKind::Variable : SymbolKind::GenericType);
            }
        });
    }

    SWAG_CHECK(eatToken());

    // '#validif' block
    if (tokenParse.is(TokenId::CompilerValidIfx))
        return error(tokenParse.token, toErr(Err0659));
    if (tokenParse.is(TokenId::CompilerValidIf))
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);
        SWAG_CHECK(doCompilerValidIf(structNode, &structNode->validif));
    }

    // Content of struct
    {
        Scoped       scoped(this, newScope);
        ScopedStruct scopedStruct(this, newScope);

        const auto contentNode = Ast::newNode<AstNode>(AstNodeKind::StructContent, this, structNode);
        structNode->content    = contentNode;
        contentNode->allocateExtension(ExtensionKind::Semantic);
        contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;

        const auto startLoc = tokenParse.token.startLocation;
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[struct]] body"));
        while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(contentNode, structType, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[struct]] body"));
    }

    return true;
}

bool Parser::doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result)
{
    const auto startLoc = tokenParse.token.startLocation;
    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        const auto stmt = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
        *result         = stmt;
        SWAG_CHECK(eatToken());
        while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(stmt, structType, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the struct body"));
        parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
        return true;
    }

    switch (tokenParse.token.id)
    {
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerRun:
            SWAG_CHECK(doCompilerRunEmbedded(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerPrint:
            SWAG_CHECK(doCompilerPrint(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerError:
            SWAG_CHECK(doCompilerError(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerWarning:
            SWAG_CHECK(doCompilerWarning(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, result));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;
        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIfFor(parent, result, structType == SyntaxStructType::Interface ? AstNodeKind::InterfaceDecl : AstNodeKind::StructDecl));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
            break;

        case TokenId::SymLeftCurly:
            SWAG_CHECK(doStructBody(parent, structType, &dummyResult));
            break;

        case TokenId::SymAttrStart:
        {
            AstAttrUse* attrUse;
            SWAG_CHECK(doAttrUse(parent, reinterpret_cast<AstNode**>(&attrUse)));
            SWAG_CHECK(doStructBody(attrUse, structType, &attrUse->content));
            parent->ownerStructScope->owner->addAstFlag(AST_STRUCT_COMPOUND);
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
            SWAG_VERIFY(structType != SyntaxStructType::Interface, context->report({parent, tokenParse.token, toErr(Err0478)}));
            SWAG_CHECK(eatToken());
            const auto structNode = castAst<AstStruct>(parent->ownerStructScope->owner, AstNodeKind::StructDecl);
            structNode->addSpecFlag(AstStruct::SPEC_FLAG_HAS_USING);
            AstNode* varDecl;
            SWAG_CHECK(doVarDecl(parent, &varDecl, AstNodeKind::VarDecl, true));
            varDecl->addAstFlag(AST_DECL_USING);
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
            const Diagnostic err{parent, tokenParse.token, toErr(Err0672)};
            return context->report(err);
        }

        case TokenId::KwdMethod:
        case TokenId::KwdFunc:
        {
            SWAG_VERIFY(structType == SyntaxStructType::Interface, error(tokenParse.token, toErr(Err0503)));

            const auto kind = tokenParse.token.id;
            SWAG_CHECK(eatToken());

            SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftParen), error(tokenParse.token, toErr(Err0685)));

            const bool isMethod      = kind == TokenId::KwdMethod;
            bool       isConstMethod = false;

            if (tokenParse.is(TokenId::KwdConst))
            {
                SWAG_VERIFY(isMethod, error(tokenParse.token, toErr(Err0458)));
                isConstMethod = true;
                SWAG_CHECK(eatToken());
            }

            const auto funcNode = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, this, nullptr);

            SWAG_CHECK(checkIsValidUserName(funcNode));
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0295, tokenParse.token.c_str())));
            SWAG_CHECK(eatToken());

            const auto scope = Ast::newScope(funcNode, "", ScopeKind::Function, parent->ownerStructScope);

            {
                Scoped scoped(this, scope);
                SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, false, nullptr, isMethod, isConstMethod, true));
            }

            ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
            const auto  varNode = Ast::newVarDecl(funcNode->token.text, this, parent);
            varNode->inheritTokenLocation(funcNode->token);
            Semantic::setVarDeclResolve(varNode);
            varNode->addAstFlag(AST_R_VALUE);

            const auto typeNode   = Ast::newNode<AstTypeLambda>(AstNodeKind::TypeLambda, this, varNode);
            typeNode->semanticFct = Semantic::resolveTypeLambdaClosure;
            varNode->type         = typeNode;
            varNode->type->inheritTokenLocation(funcNode->token);
            Ast::removeFromParent(funcNode->parameters);
            Ast::addChildFront(typeNode, funcNode->parameters);
            typeNode->parameters = funcNode->parameters;
            if (typeNode->parameters)
                typeNode->parameters->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);

            if (tokenParse.is(TokenId::SymMinusGreat))
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeNode->returnType));

                const auto retNode = Ast::newNode<AstNode>(AstNodeKind::FuncDeclType, this, funcNode);
                retNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED);
                funcNode->returnType = retNode;
                funcNode->returnType->addExtraPointer(ExtraPointerKind::ExportNode, typeNode->returnType);

                CloneContext cloneContext;
                cloneContext.parent = retNode;
                typeNode->returnType->clone(cloneContext);
            }

            if (tokenParse.is(TokenId::KwdThrow))
            {
                SWAG_CHECK(eatToken());
                typeNode->addSpecFlag(AstTypeLambda::SPEC_FLAG_CAN_THROW);
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_THROW);
            }

            varNode->addExtraPointer(ExtraPointerKind::ExportNode, funcNode);

            if (!tokenParse.commentJustBefore.empty())
            {
                varNode->allocateExtension(ExtensionKind::Misc);
                varNode->extMisc()->commentJustBefore = std::move(tokenParse.commentJustBefore);
            }

            SWAG_CHECK(eatSemiCol("interface function definition"));
            *result = varNode;
            break;
        }

        // A normal declaration
        default:
        {
            SWAG_VERIFY(structType != SyntaxStructType::Interface, error(tokenParse.token, formErr(Err0293, tokenParse.token.c_str())));
            ScopedFlags scopedFlags(this, AST_STRUCT_MEMBER);
            SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::VarDecl, true));
            break;
        }
    }

    return true;
}
