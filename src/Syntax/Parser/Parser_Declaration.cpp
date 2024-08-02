#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

bool Parser::doCheckPublicInternalPrivate(const Token& tokenAttr) const
{
    // Check following instruction
    switch (tokenParse.token.id)
    {
        case TokenId::SymLeftCurly:
        case TokenId::KwdFunc:
        case TokenId::KwdMethod:
        case TokenId::KwdAttr:
        case TokenId::KwdVar:
        case TokenId::KwdLet:
        case TokenId::KwdConst:
        case TokenId::KwdEnum:
        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::KwdImpl:
        case TokenId::KwdInterface:
        case TokenId::KwdTypeAlias:
        case TokenId::KwdNameAlias:
        case TokenId::KwdNamespace:
        case TokenId::CompilerGlobal:
            break;
        case TokenId::SymAttrStart:
            return error(tokenParse.token, formErr(Err0496, tokenAttr.c_str(), tokenAttr.c_str()));
        case TokenId::KwdPublic:
        case TokenId::KwdPrivate:
        case TokenId::KwdInternal:
            return error(tokenParse.token, formErr(Err0013, tokenParse.token.c_str(), tokenAttr.c_str()));
        default:
            return error(tokenParse.token, formErr(Err0482, tokenAttr.c_str(), tokenParse.token.c_str(), tokenParse.token.c_str()));
    }

    return true;
}

bool Parser::doPublicInternal(AstNode* parent, AstNode** result, bool forGlobal)
{
    SWAG_ASSERT(tokenParse.is(TokenId::KwdPublic) || tokenParse.is(TokenId::KwdInternal));
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(tokenParse.token, formErr(Err0483, tokenParse.token.c_str())));
    SWAG_VERIFY(!sourceFile->hasFlag(FILE_FORCE_EXPORT), error(tokenParse.token, formErr(Err0621, tokenParse.token.c_str())));

    Scope*     orgScope  = currentScope;
    Scope*     newScope  = currentScope;
    const auto tokenAttr = tokenParse;

    AttributeFlags attr;
    if (tokenParse.is(TokenId::KwdPublic))
        attr = ATTRIBUTE_PUBLIC;
    else
        attr = ATTRIBUTE_INTERNAL;

    if (forGlobal)
        sourceFile->globalAttr.add(attr);
    if (newScope->flags.has(SCOPE_FILE))
        newScope = newScope->parentScope;

    SWAG_ASSERT(newScope);
    ParserPushScope scoped(this, newScope);
    const auto      attrUse = Ast::newNode<AstAttrUse>(AstNodeKind::AttrUse, this, parent);
    *result                 = attrUse;
    attrUse->addAstFlag(AST_GENERATED | AST_GENERATED_USER);
    attrUse->attributeFlags = attr;
    SWAG_CHECK(eatToken());

    AstNode* topStmt = nullptr;

    if (!forGlobal)
    {
        SWAG_CHECK(doCheckPublicInternalPrivate(tokenAttr.token));
        SWAG_CHECK(doTopLevelInstruction(attrUse, &topStmt));
    }
    else
    {
        topStmt = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, attrUse);
        attrUse->addAstFlag(AST_GLOBAL_NODE);
        topStmt->addAstFlag(AST_GLOBAL_NODE);
        while (tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doTopLevelInstruction(topStmt, &dummyResult));
    }

    attrUse->content = topStmt;
    attrUse->content->setOwnerAttrUse(attrUse);

    // Add original scope
    if (topStmt && orgScope != newScope)
        topStmt->addAlternativeScope(orgScope);

    return true;
}

bool Parser::doPrivate(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(tokenParse.token, formErr(Err0483, tokenParse.token.c_str())));

    auto       privName = tokenParse;
    const auto attrUse  = Ast::newNode<AstAttrUse>(AstNodeKind::AttrUse, this, parent);
    *result             = attrUse;
    attrUse->addAstFlag(AST_GENERATED | AST_GENERATED_USER);
    attrUse->attributeFlags = ATTRIBUTE_PRIVATE;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doCheckPublicInternalPrivate(privName.token));
    privName.token.id = TokenId::Identifier;
    if (!sourceFile->privateId)
        sourceFile->privateId = g_UniqueID.fetch_add(1);
    privName.token.text = form("__private%d", sourceFile->privateId);

    AstNode* namespaceNode;
    SWAG_CHECK(doNamespaceOnName(attrUse, &namespaceNode, false, true, &privName.token));
    namespaceNode->addAstFlag(AST_GENERATED | AST_GENERATED_USER);
    namespaceNode->addSpecFlag(AstNameSpace::SPEC_FLAG_PRIVATE);

    attrUse->content = namespaceNode;
    attrUse->content->setOwnerAttrUse(attrUse);

    return true;
}

bool Parser::doUsing(AstNode* parent, AstNode** result, bool isGlobal)
{
    auto savedToken = tokenParse;
    SWAG_CHECK(eatToken());

    switch (tokenParse.token.id)
    {
        case TokenId::KwdNamespace:
            SWAG_CHECK(doNamespace(parent, result, false, true));
            FormatAst::inheritFormatBefore(this, *result, &savedToken);
            return true;

        case TokenId::KwdVar:
        {
            AstNode* varNode;
            SWAG_CHECK(doVarDecl(parent, &varNode));

            if (!varNode->is(AstNodeKind::VarDecl))
            {
                Diagnostic err{varNode->token.sourceFile, varNode->firstChild()->token.startLocation, varNode->lastChild()->token.endLocation, toErr(Err0331)};
                err.addNote(varNode->token.sourceFile, savedToken.token, toNte(Nte0057));
                return context->report(err);
            }

            varNode->addAstFlag(AST_DECL_USING);
            FormatAst::inheritFormatBefore(this, varNode, &savedToken);
            return true;
        }
    }

    // We must ensure that no job can be run before the using
    if (!parent->ownerFct)
    {
        for (const auto child : parent->children)
        {
            switch (child->kind)
            {
                case AstNodeKind::CompilerImport:
                case AstNodeKind::CompilerAssert:
                case AstNodeKind::CompilerForeignLib:
                case AstNodeKind::Using:
                case AstNodeKind::IdentifierRef:
                case AstNodeKind::Namespace:
                case AstNodeKind::CompilerDependencies:
                case AstNodeKind::CompilerGlobal:
                    break;
                case AstNodeKind::AttrUse:
                    if (child->hasSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL))
                        break;
                    [[fallthrough]];
                default:
                {
                    Diagnostic err{sourceFile, tokenParse.token, toErr(Err0514)};
                    err.addNote(child, child->token, toNte(Nte0074));
                    return context->report(err);
                }
            }
        }
    }

    const auto node = Ast::newNode<AstUsing>(AstNodeKind::Using, this, parent);
    *result         = node;
    if (isGlobal)
        node->addAstFlag(AST_GLOBAL_NODE);
    node->semanticFct = Semantic::resolveUsing;
    FormatAst::inheritFormatBefore(this, node, &savedToken);

    while (true)
    {
        SWAG_CHECK(doIdentifierRef(node, &dummyResult, IDENTIFIER_NO_PARAMS));

        if (tokenParse.isNot(TokenId::SymComma))
        {
            SWAG_CHECK(eatSemiCol("[[using]] declaration"));
            break;
        }

        SWAG_CHECK(eatToken());
    }

    return true;
}

bool Parser::doNamespace(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(doNamespace(parent, result, false, false));
    return true;
}

bool Parser::doNamespace(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing)
{
    auto savedToken = tokenParse;
    SWAG_CHECK(eatToken());
    SWAG_CHECK(doNamespaceOnName(parent, result, forGlobal, forUsing));
    FormatAst::inheritFormatBefore(this, *result, &savedToken);
    return true;
}

bool Parser::doNamespaceOnName(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing, const Token* privName)
{
    // There is only one swag namespace, defined in the bootstrap. So if we redeclared it
    // in runtime, use the one from the bootstrap
    if (sourceFile->hasFlag(FILE_RUNTIME) && tokenParse.token.is(g_LangSpec->name_Swag))
        currentScope = g_Workspace->bootstrapModule->files[0]->astRoot->ownerScope;

    Scope*        oldScope       = currentScope;
    Scope*        newScope       = nullptr;
    AstNameSpace* namespaceNode  = nullptr;
    AstNameSpace* firstNameSpace = nullptr;
    const bool    forPrivate     = privName != nullptr;

    while (true)
    {
        namespaceNode = Ast::newNode<AstNameSpace>(AstNodeKind::Namespace, this, parent);
        if (privName)
            namespaceNode->token.text = privName->text;
        if (!firstNameSpace)
            firstNameSpace = namespaceNode;
        else
        {
            namespaceNode->addSpecFlag(AstNameSpace::SPEC_FLAG_SUB_NAME);
            firstNameSpace->multiNames.push_back(namespaceNode->token.text);
        }
        if (forGlobal)
            namespaceNode->addAstFlag(AST_GLOBAL_NODE);
        if (forUsing)
            namespaceNode->addSpecFlag(AstNameSpace::SPEC_FLAG_USING);
        namespaceNode->addAttribute(sourceFile->globalAttr);

        switch (tokenParse.token.id)
        {
            case TokenId::Identifier:
                break;
            case TokenId::SymLeftCurly:
                if (!forPrivate)
                    return unexpectedTokenError(tokenParse.token, formErr(Err0575, "{"));
                break;
            case TokenId::SymSemiColon:
                return unexpectedTokenError(tokenParse.token, formErr(Err0575, ";"));
            default:
                if (!forPrivate)
                    return error(tokenParse.token, formErr(Err0335, tokenParse.token.c_str()));
                break;
        }

        // Be sure this is not the swag namespace, except for a runtime file
        if (!sourceFile->acceptsInternalStuff())
            SWAG_VERIFY(!namespaceNode->token.text.compareNoCase(g_LangSpec->name_Swag), error(tokenParse.token, formErr(Err0624, tokenParse.token.c_str())));

        // Add/Get namespace
        {
            ScopedLock lk(currentScope->symTable.mutex);
            const auto symbol = currentScope->symTable.findNoLock(namespaceNode->token.text);
            if (!symbol)
            {
                const auto typeInfo = makeType<TypeInfoNamespace>();
                typeInfo->name      = namespaceNode->token.text;
                newScope            = Ast::newScope(namespaceNode, namespaceNode->token.text, ScopeKind::Namespace, currentScope);
                if (forPrivate)
                    newScope->flags.add(SCOPE_FILE_PRIVATE);
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                AddSymbolTypeInfo toAdd;
                toAdd.node       = namespaceNode;
                toAdd.typeInfo   = typeInfo;
                toAdd.kind       = SymbolKind::Namespace;
                toAdd.symbolName = currentScope->symTable.registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, &namespaceNode->token.text);
                currentScope->symTable.addSymbolTypeInfoNoLock(context, toAdd);
            }
            else if (symbol->isNot(SymbolKind::Namespace))
            {
                return SemanticError::duplicatedSymbolError(context, sourceFile, tokenParse.token, SymbolKind::Namespace, symbol->name, symbol->kind, symbol->nodes.front());
            }
            else
                newScope = castTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
        }

        if (!forPrivate)
            SWAG_CHECK(eatToken());

        if (tokenParse.isNot(TokenId::SymDot))
        {
            if (forUsing)
            {
                parent = parent->findParentOrMe(AstNodeKind::File);
                parent->addAlternativeScope(newScope);
            }

            break;
        }

        SWAG_CHECK(eatToken());
        parent = namespaceNode;
        namespaceNode->addSpecFlag(AstNameSpace::SPEC_FLAG_NO_CURLY);
        currentScope = newScope;
    }

    currentScope   = oldScope;
    auto openCurly = tokenParse;

    if (forGlobal)
    {
        SWAG_CHECK(eatSemiCol("[[#global namespace]]"));
        ParserPushScope scoped(this, newScope);
        while (tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
    }
    else
    {
        const auto startLoc = tokenParse.token.startLocation;

        // Content of namespace is toplevel
        if (tokenParse.is(TokenId::SymLeftCurly))
        {
            eatToken();
            ParserPushScope scoped(this, newScope);
            while (tokenParse.isNot(TokenId::EndOfFile) && tokenParse.isNot(TokenId::SymRightCurly))
                SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
            SWAG_CHECK(eatFormat(namespaceNode));
            SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[namespace]] body"));
        }
        else if (tokenParse.is(TokenId::SymSemiColon))
        {
            return error(tokenParse.token, toErr(Err0334));
        }
        else
        {
            ParserPushScope scoped(this, newScope);
            SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
            firstNameSpace->addSpecFlag(AstNameSpace::SPEC_FLAG_NO_CURLY);
        }
    }

    *result = firstNameSpace;
    return true;
}

bool Parser::doGlobalCurlyStatement(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
    *result         = node;
    node->addSpecFlag(AstStatement::SPEC_FLAG_CURLY);

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the statement block"));
    while (tokenParse.isNot(TokenId::EndOfFile) && tokenParse.isNot(TokenId::SymRightCurly))
        SWAG_CHECK(doTopLevelInstruction(node, &dummyResult));
    SWAG_CHECK(eatFormat(node));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doCurlyStatement(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
    *result         = node;
    node->addSpecFlag(AstStatement::SPEC_FLAG_CURLY);

    const bool isGlobal = currentScope->isGlobalOrImpl();
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the statement block"));

    if (isGlobal)
    {
        while (tokenParse.isNot(TokenId::EndOfFile) && tokenParse.isNot(TokenId::SymRightCurly))
        {
            SWAG_CHECK(doTopLevelInstruction(node, &dummyResult));
        }
    }
    else
    {
        while (tokenParse.isNot(TokenId::EndOfFile) && tokenParse.isNot(TokenId::SymRightCurly))
        {
            SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
        }
    }

    node->token.endLocation = tokenParse.token.startLocation;
    SWAG_CHECK(eatFormat(node));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind)
{
    const auto      newScope = Ast::newScope(parent, "", scopeKind, currentScope);
    ParserPushScope scoped(this, newScope);

    AstNode* node;
    SWAG_CHECK(doCurlyStatement(parent, &node));
    *result = node;

    const auto statement = castAst<AstStatement>(node, AstNodeKind::Statement, AstNodeKind::StatementNoScope);
    newScope->owner      = statement;
    statement->addSpecFlag(AstStatement::SPEC_FLAG_NEED_SCOPE);
    statement->byteCodeFct = ByteCodeGen::emitDebugNop;
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;

    return true;
}

bool Parser::doScopedStatement(AstNode* parent, const Token& forToken, AstNode** result, bool mustHaveDo)
{
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymSemiColon), error(tokenParse.token, toErr(Err0265), toNte(Nte0054)));

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        SWAG_CHECK(doScopedCurlyStatement(parent, result));
        return true;
    }

    const auto      newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    ParserPushScope scoped(this, newScope);
    AstNode*        statement = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, parent);
    *result                   = statement;

    if (mustHaveDo)
    {
        if (tokenParse.isNot(TokenId::KwdDo))
        {
            Diagnostic err{sourceFile, tokenParse.token, formErr(Err0537, tokenParse.token.c_str())};
            err.addNote(parent, forToken, formNte(Nte0016, forToken.c_str()));
            return context->report(err);
        }

        const auto tokenDo = tokenParse;
        SWAG_CHECK(eatToken());

        if (tokenParse.is(TokenId::SymLeftCurly))
        {
            const Diagnostic err{sourceFile, tokenDo.token, toErr(Err0461)};
            return context->report(err);
        }
    }

    SWAG_ASSERT(!currentScope->isGlobalOrImpl());

    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = Semantic::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = Semantic::resolveScopedStmtAfter;
    statement->addSpecFlag(AstStatement::SPEC_FLAG_NEED_SCOPE);
    newScope->owner = statement;
    SWAG_CHECK(doEmbeddedInstruction(statement, &dummyResult));
    return true;
}

void Parser::registerSubDecl(AstNode* subDecl)
{
    // When we are in format mode, no need to move the sub declaration at the top level. We keep it where it is
    if (parserFlags.has(PARSER_TRACK_FORMAT))
        return;

    SWAG_ASSERT(subDecl->ownerFct);
    const auto orgSubDecl = subDecl;
    subDecl->ownerFct->subDecl.push_back(subDecl);
    subDecl->addAstFlag(AST_NO_SEMANTIC | AST_SUB_DECL | AST_INTERNAL);

    AstAttrUse* newAttrUse = nullptr;

    // If parent is an attribute, then we will move it with the sub-decl
    if (subDecl->parent->is(AstNodeKind::AttrUse))
        subDecl = subDecl->parent;

    // Else if we are in an attr-use block, we need to duplicate each of them, in order
    // for the sub-decl to have its own attr-use
    else if (subDecl->parent->is(AstNodeKind::Statement))
    {
        auto testParent  = subDecl->parent;
        int  idxToInsert = 0;
        while (testParent)
        {
            while (testParent && testParent->is(AstNodeKind::Statement))
                testParent = testParent->parent;
            if (!testParent)
                break;
            if (testParent->isNot(AstNodeKind::AttrUse))
                break;
            if (!newAttrUse)
                newAttrUse = Ast::newNode<AstAttrUse>(AstNodeKind::AttrUse, this, subDecl->parent);

            // Clone all attributes
            CloneContext cloneContext;
            for (uint32_t i = 0; i < testParent->childCount() - 1; i++) // Do not clone content
            {
                cloneContext.parent = newAttrUse;
                const auto child    = testParent->children[i]->clone(cloneContext);

                // Only the last attribute of the block needs to have a semanticAfterFct, so
                // we rest it, and we will set it later for the last child
                // :AttrUseLastChild
                if (child->hasExtSemantic())
                    child->extSemantic()->semanticAfterFct = nullptr;

                // Need to add attributes in the correct order (top level first)
                Ast::removeFromParent(child);
                Ast::insertChild(newAttrUse, child, idxToInsert++);
            }

            idxToInsert = 0;
            testParent  = testParent->parent;
        }
    }

    auto       newParent = subDecl->parent;
    const auto orgParent = newParent;
    Ast::removeFromParent(subDecl);

    // :SubDeclParent
    while (newParent != sourceFile->astRoot && !newParent->hasAstFlag(AST_GLOBAL_NODE) && newParent->isNot(AstNodeKind::Namespace))
        newParent = newParent->parent;

    // Force the parent to be the new attribute, if defined
    if (newAttrUse)
    {
        // The last child must have the semanticAfterFct set
        // :AttrUseLastChild
        const auto back = newAttrUse->lastChild();
        back->allocateExtension(ExtensionKind::Semantic);
        SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
        back->extSemantic()->semanticAfterFct = Semantic::resolveAttrUse;

        Ast::removeFromParent(newAttrUse);
        Ast::addChildBack(newParent, newAttrUse);
        newAttrUse->content = subDecl;
        subDecl->setOwnerAttrUse(newAttrUse);
        newParent = newAttrUse;
    }

    Ast::addChildBack(newParent, subDecl);

    // We want to solve the sub-decl in the correct context, with all the previous nodes (inside the function)
    // solved, in case we make a reference to it (like in 5296, the @decltype).
    // So we add a fake makePointerLambda which will authorise the solving of the corresponding sub-decl
    // when it is evaluated.
    if (orgSubDecl->isNot(AstNodeKind::FuncDecl) || !orgSubDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IS_LAMBDA_EXPRESSION))
    {
        const auto solver   = Ast::newNode<AstRefSubDecl>(AstNodeKind::RefSubDecl, this, orgParent);
        solver->semanticFct = Semantic::resolveSubDeclRef;
        solver->addAstFlag(AST_GENERATED | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
        solver->refSubDecl = orgSubDecl;
        orgSubDecl->addAstFlag(AST_NO_SEMANTIC | AST_SPEC_SEMANTIC3 | AST_SPEC_SEMANTIC_HAS3);
    }
}

bool Parser::doCompilerScopeBreakable(AstNode* parent, AstNode** result)
{
    const auto labelNode   = Ast::newNode<AstScopeBreakable>(AstNodeKind::ScopeBreakable, this, parent);
    *result                = labelNode;
    labelNode->semanticFct = Semantic::resolveScopeBreakable;

    SWAG_CHECK(eatToken());
    if (tokenParse.isNot(TokenId::SymLeftCurly))
    {
        labelNode->addSpecFlag(AstScopeBreakable::SPEC_FLAG_NAMED);
        SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0141, tokenParse.token.c_str())));
        labelNode->inheritTokenName(tokenParse.token);
        labelNode->inheritTokenLocation(tokenParse.token);
        SWAG_CHECK(eatToken());
    }

    ParserPushBreakable scoped(this, labelNode);
    SWAG_CHECK(doEmbeddedInstruction(labelNode, &labelNode->block));
    return true;
}

bool Parser::doLeftInstruction(AstNode* parent, AstNode** result, const AstWith* withNode)
{
    switch (tokenParse.token.id)
    {
        case TokenId::KwdReturn:
            SWAG_CHECK(doReturn(parent, result));
            SWAG_CHECK(eatSemiCol("return expression"));
            break;
        case TokenId::KwdIf:
            SWAG_CHECK(doIf(parent, result));
            break;
        case TokenId::KwdWhile:
            SWAG_CHECK(doWhile(parent, result));
            break;
        case TokenId::KwdFor:
            SWAG_CHECK(doFor(parent, result));
            break;
        case TokenId::KwdSwitch:
            SWAG_CHECK(doSwitch(parent, result));
            break;
        case TokenId::KwdLoop:
            SWAG_CHECK(doLoop(parent, result));
            break;
        case TokenId::KwdVisit:
            SWAG_CHECK(doVisit(parent, result));
            break;
        case TokenId::KwdTry:
        case TokenId::KwdCatch:
        case TokenId::KwdTryCatch:
        case TokenId::KwdAssume:
            SWAG_CHECK(doTryCatchAssume(parent, result));
            break;
        case TokenId::KwdThrow:
            SWAG_CHECK(doThrow(parent, result));
            break;
        case TokenId::KwdDiscard:
            SWAG_CHECK(doDiscard(parent, result));
            break;
        case TokenId::IntrinsicInit:
            SWAG_CHECK(doInit(parent, result));
            break;
        case TokenId::IntrinsicDrop:
        case TokenId::IntrinsicPostCopy:
        case TokenId::IntrinsicPostMove:
            SWAG_CHECK(doDropCopyMove(parent, result));
            break;

        case TokenId::CompilerUp:
        case TokenId::CompilerSelf:
        case TokenId::Identifier:
        case TokenId::SymLeftParen:
        case TokenId::KwdDeRef:
        case TokenId::KwdMoveRef:
            SWAG_CHECK(doAffectExpression(parent, result, withNode));
            break;

        case TokenId::IntrinsicGetContext:
        case TokenId::IntrinsicGetProcessInfos:
        case TokenId::IntrinsicDbgAlloc:
        case TokenId::IntrinsicSysAlloc:
            SWAG_CHECK(doAffectExpression(parent, result));
            break;

        case TokenId::IntrinsicPrint:
        case TokenId::IntrinsicAssert:
        case TokenId::IntrinsicBcBreakpoint:
        case TokenId::IntrinsicPanic:
        case TokenId::IntrinsicCompilerError:
        case TokenId::IntrinsicCompilerWarning:
        case TokenId::IntrinsicFree:
        case TokenId::IntrinsicMemCpy:
        case TokenId::IntrinsicMemMove:
        case TokenId::IntrinsicMemSet:
        case TokenId::IntrinsicSetContext:
        case TokenId::IntrinsicAtomicAdd:
        case TokenId::IntrinsicAtomicAnd:
        case TokenId::IntrinsicAtomicOr:
        case TokenId::IntrinsicAtomicXor:
        case TokenId::IntrinsicAtomicXchg:
        case TokenId::IntrinsicAtomicCmpXchg:
        case TokenId::InternalSetErr:
        case TokenId::IntrinsicCVaStart:
        case TokenId::IntrinsicCVaEnd:
        case TokenId::IntrinsicCVaArg:
            SWAG_CHECK(doIdentifierRef(parent, result));
            break;
    }

    return true;
}

bool Parser::doEmbeddedInstruction(AstNode* parent, AstNode** result)
{
    switch (tokenParse.token.id)
    {
        case TokenId::KwdReturn:
        case TokenId::KwdIf:
        case TokenId::KwdWhile:
        case TokenId::KwdFor:
        case TokenId::KwdSwitch:
        case TokenId::KwdLoop:
        case TokenId::KwdVisit:
        case TokenId::KwdTry:
        case TokenId::KwdCatch:
        case TokenId::KwdTryCatch:
        case TokenId::KwdAssume:
        case TokenId::KwdThrow:
        case TokenId::KwdDiscard:
        case TokenId::KwdDeRef:

        case TokenId::CompilerUp:
        case TokenId::SymLeftParen:
        case TokenId::CompilerSelf:

        case TokenId::IntrinsicAtomicAdd:
        case TokenId::IntrinsicAtomicAnd:
        case TokenId::IntrinsicAtomicCmpXchg:
        case TokenId::IntrinsicAtomicOr:
        case TokenId::IntrinsicAtomicXchg:
        case TokenId::IntrinsicAtomicXor:

        case TokenId::IntrinsicAssert:
        case TokenId::IntrinsicBcBreakpoint:
        case TokenId::IntrinsicCVaArg:
        case TokenId::IntrinsicCVaEnd:
        case TokenId::IntrinsicCVaStart:
        case TokenId::IntrinsicDrop:
        case TokenId::IntrinsicCompilerError:
        case TokenId::IntrinsicCompilerWarning:
        case TokenId::IntrinsicFree:
        case TokenId::IntrinsicGetContext:
        case TokenId::IntrinsicInit:
        case TokenId::IntrinsicMemCpy:
        case TokenId::IntrinsicMemMove:
        case TokenId::IntrinsicMemSet:
        case TokenId::IntrinsicPanic:
        case TokenId::IntrinsicPostCopy:
        case TokenId::IntrinsicPostMove:
        case TokenId::IntrinsicPrint:
        case TokenId::IntrinsicSetContext:
        case TokenId::InternalSetErr:
        case TokenId::IntrinsicGetProcessInfos:
        case TokenId::IntrinsicDbgAlloc:
        case TokenId::IntrinsicSysAlloc:
            SWAG_CHECK(doLeftInstruction(parent, result));
            break;

        case TokenId::Identifier:
            SWAG_CHECK(doLeftInstruction(parent, result));
            break;

        case TokenId::SymLeftCurly:
            SWAG_CHECK(doScopedCurlyStatement(parent, result, ScopeKind::EmptyStatement));
            break;
        case TokenId::SymSemiColon:
            SWAG_CHECK(eatToken());
            break;
        case TokenId::KwdUsing:
            SWAG_CHECK(doUsing(parent, result, false));
            break;
        case TokenId::KwdWith:
            SWAG_CHECK(doWith(parent, result));
            break;
        case TokenId::KwdVar:
        case TokenId::KwdLet:
        case TokenId::KwdConst:
            SWAG_CHECK(doVarDecl(parent, result));
            break;
        case TokenId::KwdDefer:
            SWAG_CHECK(doDefer(parent, result));
            break;
        case TokenId::KwdBreak:
            SWAG_CHECK(doBreak(parent, result));
            break;
        case TokenId::KwdFallThrough:
            SWAG_CHECK(doFallThrough(parent, result));
            break;
        case TokenId::KwdUnreachable:
            SWAG_CHECK(doUnreachable(parent, result));
            break;
        case TokenId::KwdContinue:
            SWAG_CHECK(doContinue(parent, result));
            break;
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, result));
            break;
        case TokenId::CompilerMacro:
            SWAG_CHECK(doCompilerMacro(parent, result));
            break;
        case TokenId::CompilerMixin:
            SWAG_CHECK(doCompilerMixin(parent, result));
            break;
        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIf(parent, result));
            break;
        case TokenId::CompilerPrint:
            SWAG_CHECK(doCompilerPrint(parent, result));
            break;
        case TokenId::CompilerError:
            SWAG_CHECK(doCompilerError(parent, result));
            break;
        case TokenId::CompilerWarning:
            SWAG_CHECK(doCompilerWarning(parent, result));
            break;
        case TokenId::CompilerRun:
            SWAG_CHECK(doCompilerRunEmbedded(parent, result));
            break;
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, result));
            break;
        case TokenId::SymAttrStart:
        {
            AstAttrUse* attrUse;
            SWAG_CHECK(doAttrUse(parent, reinterpret_cast<AstNode**>(&attrUse)));
            *result = attrUse;
            // We do not want a #[] to create a new scope when inside a function
            if (tokenParse.is(TokenId::SymLeftCurly))
                SWAG_CHECK(doCurlyStatement(attrUse, &attrUse->content));
            else
                SWAG_CHECK(doEmbeddedInstruction(attrUse, &attrUse->content));
            if (attrUse->content)
                attrUse->content->setOwnerAttrUse(attrUse);
            break;
        }

        case TokenId::KwdEnum:
            SWAG_CHECK(doEnum(parent, result));
            break;

        case TokenId::KwdMethod:
        case TokenId::KwdFunc:
        {
            AstNode* subFunc;
            SWAG_CHECK(doFuncDecl(parent, &subFunc));
            *result = subFunc;
            registerSubDecl(subFunc);
            break;
        }

        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::KwdInterface:
        {
            AstNode* subDecl;
            SWAG_CHECK(doStruct(parent, &subDecl));
            *result = subDecl;
            registerSubDecl(subDecl);
            break;
        }

        case TokenId::CompilerScope:
            SWAG_CHECK(doCompilerScopeBreakable(parent, result));
            break;
        case TokenId::KwdTypeAlias:
            SWAG_CHECK(doTypeAlias(parent, result));
            break;
        case TokenId::KwdNameAlias:
            SWAG_CHECK(doNameAlias(parent, result));
            break;

        case TokenId::KwdPublic:
        case TokenId::KwdInternal:
        case TokenId::KwdPrivate:
            return error(tokenParse.token, formErr(Err0483, tokenParse.token.c_str()));

        case TokenId::SymDot:
        {
            const auto withNode = parent->findParent(AstNodeKind::With);
            SWAG_VERIFY(withNode, error(tokenParse.token, toErr(Err0511)));
            auto tokenDot = tokenParse;
            eatToken();
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0367, tokenParse.token.c_str())));
            SWAG_CHECK(doLeftInstruction(parent, result, castAst<AstWith>(withNode, AstNodeKind::With)));
            FormatAst::inheritFormatBefore(this, *result, &tokenDot);
            return true;
        }

        case TokenId::NativeType:
        {
            Diagnostic err{sourceFile, tokenParse.token, toErr(Err0706)};
            eatToken();
            if (tokenParse.is(TokenId::Identifier))
                err.addNote(toNte(Nte0181));
            return context->report(err);
        }

        default:
            if (Tokenizer::isIntrinsicReturn(tokenParse.token.id))
                return error(tokenParse.token, formErr(Err0755, tokenParse.token.c_str()));
            return invalidTokenError(InvalidTokenError::EmbeddedInstruction);
    }

    return true;
}

bool Parser::doTopLevelAttrStart(AstNode* parent, AstNode** result)
{
    AstAttrUse* attrUse;
    SWAG_CHECK(doAttrUse(parent, reinterpret_cast<AstNode**>(&attrUse)));
    *result = attrUse;
    SWAG_CHECK(doTopLevelInstruction(attrUse, &attrUse->content));
    if (attrUse->content)
        attrUse->content->setOwnerAttrUse(attrUse);
    return true;
}

bool Parser::doTopLevelInstruction(AstNode* parent, AstNode** result)
{
    // #global is invalid if afterGlobal is true
    if (tokenParse.isNot(TokenId::CompilerGlobal) && tokenParse.isNot(TokenId::SymSemiColon))
        afterGlobal = true;
    // Do not generate AST if buildPass does not allow it
    if (sourceFile->buildPass < BuildPass::Syntax)
        return eatToken();

    *result = nullptr;
    switch (tokenParse.token.id)
    {
        case TokenId::SymLeftCurly:
            SWAG_CHECK(doGlobalCurlyStatement(parent, result));
            return true;
        case TokenId::SymSemiColon:
            SWAG_CHECK(eatToken());
            return true;
        case TokenId::KwdVar:
        case TokenId::KwdConst:
            SWAG_CHECK(doVarDecl(parent, result));
            return true;
        case TokenId::KwdTypeAlias:
            SWAG_CHECK(doTypeAlias(parent, result));
            return true;
        case TokenId::KwdNameAlias:
            SWAG_CHECK(doNameAlias(parent, result));
            return true;
        case TokenId::KwdPublic:
        case TokenId::KwdInternal:
            SWAG_CHECK(doPublicInternal(parent, result, false));
            return true;
        case TokenId::KwdPrivate:
            SWAG_CHECK(doPrivate(parent, result));
            return true;
        case TokenId::KwdNamespace:
            SWAG_CHECK(doNamespace(parent, result));
            return true;
        case TokenId::KwdEnum:
            SWAG_CHECK(doEnum(parent, result));
            return true;
        case TokenId::KwdImpl:
            SWAG_CHECK(doImpl(parent, result));
            return true;
        case TokenId::KwdStruct:
        case TokenId::KwdUnion:
        case TokenId::KwdInterface:
            SWAG_CHECK(doStruct(parent, result));
            return true;
        case TokenId::KwdAttr:
            SWAG_CHECK(doAttrDecl(parent, result));
            return true;
        case TokenId::KwdUsing:
            SWAG_CHECK(doUsing(parent, result, false));
            return true;
        case TokenId::SymAttrStart:
            SWAG_CHECK(doTopLevelAttrStart(parent, result));
            return true;
        case TokenId::CompilerAst:
            SWAG_CHECK(doCompilerAst(parent, result));
            return true;
        case TokenId::KwdFunc:
        case TokenId::KwdMethod:
        case TokenId::CompilerFuncTest:
        case TokenId::CompilerFuncInit:
        case TokenId::CompilerFuncDrop:
        case TokenId::CompilerFuncPreMain:
        case TokenId::CompilerFuncMain:
        case TokenId::CompilerFuncMessage:
            SWAG_CHECK(doFuncDecl(parent, result));
            return true;
        case TokenId::CompilerRun:
            SWAG_CHECK(doCompilerRunTopLevel(parent, result));
            return true;
        case TokenId::CompilerIf:
            SWAG_CHECK(doCompilerIf(parent, result));
            return true;
        case TokenId::CompilerAssert:
            SWAG_CHECK(doCompilerAssert(parent, result));
            return true;
        case TokenId::CompilerError:
            SWAG_CHECK(doCompilerError(parent, result));
            return true;
        case TokenId::CompilerWarning:
            SWAG_CHECK(doCompilerWarning(parent, result));
            return true;
        case TokenId::CompilerPrint:
            SWAG_CHECK(doCompilerPrint(parent, result));
            return true;
        case TokenId::CompilerDependencies:
            SWAG_CHECK(doCompilerDependencies(parent));
            return true;
        case TokenId::CompilerImport:
            SWAG_CHECK(doCompilerImport(parent));
            return true;
        case TokenId::CompilerLoad:
            SWAG_CHECK(doCompilerLoad(parent));
            return true;
        case TokenId::CompilerPlaceHolder:
            SWAG_CHECK(doCompilerPlaceHolder(parent));
            return true;
            break;
        case TokenId::CompilerGlobal:
            SWAG_CHECK(doCompilerGlobal(parent, result));
            return true;
        case TokenId::CompilerForeignLib:
            SWAG_CHECK(doCompilerForeignLib(parent, result));
            return true;
    }

    return invalidTokenError(InvalidTokenError::TopLevelInstruction);
}
