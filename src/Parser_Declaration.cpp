#include "pch.h"
#include "Scoped.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "LanguageSpec.h"
#include "TypeManager.h"

bool Parser::doWith(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    auto node = Ast::newNode<AstWith>(this, AstNodeKind::With, sourceFile, parent);
    *result   = node;

    AstNode* id = nullptr;
    if (token.id == TokenId::KwdVar || token.id == TokenId::KwdLet)
    {
        SWAG_CHECK(doVarDecl(node, &id));
        if (id->kind != AstNodeKind::VarDecl)
        {
            Diagnostic diag{id->sourceFile, id->childs.front()->token.startLocation, id->childs.back()->token.endLocation, Err(Syn0157)};
            auto       note = Diagnostic::help(Hlp(Hlp0039));
            return context->report(diag, note);
        }

        SWAG_ASSERT(id->extSemantic()->semanticAfterFct == SemanticJob::resolveVarDeclAfter);
        id->extSemantic()->semanticAfterFct = SemanticJob::resolveWithVarDeclAfter;
        node->id.push_back(id->token.text);
    }
    else
    {
        SWAG_CHECK(doAffectExpression(node, &id));

        if (id->kind == AstNodeKind::StatementNoScope)
        {
            Diagnostic diag{node->sourceFile, id->childs.front()->token.startLocation, id->childs.back()->token.endLocation, Err(Syn0157)};
            auto       note = Diagnostic::help(Hlp(Hlp0039));
            return context->report(diag, note);
        }

        if (id->kind != AstNodeKind::IdentifierRef &&
            id->kind != AstNodeKind::VarDecl &&
            id->kind != AstNodeKind::AffectOp)
            return error(node->token, Err(Syn0148));

        id->allocateExtension(ExtensionKind::Semantic);
        if (id->kind == AstNodeKind::IdentifierRef)
        {
            SWAG_ASSERT(!id->extSemantic()->semanticAfterFct);
            id->extSemantic()->semanticAfterFct = SemanticJob::resolveWith;
            for (size_t i = 0; i < id->childs.size(); i++)
                node->id.push_back(id->childs[i]->token.text);
        }
        else if (id->kind == AstNodeKind::VarDecl)
        {
            SWAG_ASSERT(id->extSemantic()->semanticAfterFct == SemanticJob::resolveVarDeclAfter);
            id->extSemantic()->semanticAfterFct = SemanticJob::resolveWithVarDeclAfter;
            node->id.push_back(id->token.text);
        }
        else if (id->kind == AstNodeKind::AffectOp)
        {
            id = id->childs.front();
            if (id->extSemantic()->semanticAfterFct == SemanticJob::resolveAfterKnownType)
                id->extSemantic()->semanticAfterFct = SemanticJob::resolveWithAfterKnownType;
            else
                id->extSemantic()->semanticAfterFct = SemanticJob::resolveWith;
            for (size_t i = 0; i < id->childs.size(); i++)
                node->id.push_back(id->childs[i]->token.text);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    SWAG_CHECK(doEmbeddedStatement(node, &dummyResult));
    return true;
}

bool Parser::doCheckPublicInternalPrivate(Token& tokenAttr)
{
    // Check following instruction
    switch (token.id)
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
        break;
    case TokenId::SymAttrStart:
        return error(token, Fmt(Err(Syn0150), token.ctext(), tokenAttr.ctext()), nullptr, Fmt(Hnt(Hnt0043), tokenAttr.ctext()));
    default:
        return error(token, Fmt(Err(Syn0174), token.ctext(), tokenAttr.ctext()));
    }
    return true;
}

bool Parser::doPublicInternal(AstNode* parent, AstNode** result, bool forGlobal)
{
    SWAG_ASSERT(token.id == TokenId::KwdPublic || token.id == TokenId::KwdInternal);
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, Fmt(Err(Syn0032), token.ctext())));
    SWAG_VERIFY(!sourceFile->forceExport, error(token, Fmt(Err(Syn0018), token.ctext())));

    uint32_t attr      = 0;
    Scope*   newScope  = currentScope;
    auto     tokenAttr = token;

    if (token.id == TokenId::KwdPublic)
        attr = ATTRIBUTE_PUBLIC;
    else
        attr = ATTRIBUTE_INTERNAL;

    if (forGlobal)
        sourceFile->globalAttr |= attr;
    if (newScope->flags & SCOPE_FILE)
        newScope = newScope->parentScope;

    tokenizer.propagateComment = true;
    SWAG_CHECK(eatToken());

    SWAG_ASSERT(newScope);
    Scoped scoped(this, newScope);
    auto   attrUse = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    *result        = attrUse;
    attrUse->flags |= AST_GENERATED;
    attrUse->attributeFlags = attr;

    AstNode* topStmt = nullptr;

    if (!forGlobal)
    {
        SWAG_CHECK(doCheckPublicInternalPrivate(tokenAttr));
        SWAG_CHECK(doTopLevelInstruction(attrUse, &topStmt));
    }
    else
    {
        attrUse->flags |= AST_GLOBAL_NODE;
        topStmt = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, attrUse);
        topStmt->flags |= AST_GLOBAL_NODE;
        while (token.id != TokenId::EndOfFile)
            SWAG_CHECK(doTopLevelInstruction(topStmt, &dummyResult));
    }

    attrUse->content = topStmt;
    attrUse->content->setOwnerAttrUse(attrUse);

    // Add original scope
    if (topStmt)
        topStmt->addAlternativeScope(currentScope);

    return true;
}

bool Parser::doPrivate(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, Fmt(Err(Syn0032), token.ctext())));

    auto privName = token;
    auto attrUse  = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    *result       = attrUse;
    attrUse->flags |= AST_GENERATED;
    attrUse->attributeFlags = ATTRIBUTE_PRIVATE;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(doCheckPublicInternalPrivate(privName));
    privName.id   = TokenId::Identifier;
    privName.text = Fmt("__privns_%d", g_UniqueID.fetch_add(1));
    AstNode* namespc;
    SWAG_CHECK(doNamespaceOnName(attrUse, &namespc, false, true, &privName));

    attrUse->content = namespc;
    attrUse->content->setOwnerAttrUse(attrUse);

    return true;
}

bool Parser::doUsing(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    switch (token.id)
    {
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent, result, false, true));
        return true;

    case TokenId::KwdVar:
    {
        AstNode* varNode;
        SWAG_CHECK(doVarDecl(parent, &varNode));

        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Using, sourceFile, parent);
        *result           = node;
        node->semanticFct = SemanticJob::resolveUsing;
        Ast::newIdentifierRef(sourceFile, varNode->token.text, node, this);
        return true;
    }
    default:
        break;
    }

    // We must ensure that no job can be run before the using
    if (!parent->ownerFct)
    {
        for (auto child : parent->childs)
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
                break;
            case AstNodeKind::AttrUse:
                if (child->specFlags & AstAttrUse::SPECFLAG_GLOBAL)
                    break;
            default:
            {
                Diagnostic diag{sourceFile, token, Err(Syn0036)};
                auto       note = Diagnostic::note(child, child->token, Nte(Nte0024));
                return context->report(diag, note);
            }
            }
        }
    }

    while (true)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Using, sourceFile, parent);
        *result           = node;
        node->semanticFct = SemanticJob::resolveUsing;

        SWAG_CHECK(doIdentifierRef(node, &dummyResult, IDENTIFIER_NO_PARAMS));

        if (token.id != TokenId::SymComma)
        {
            SWAG_CHECK(eatSemiCol("'using' declaration"));
            break;
        }

        SWAG_CHECK(eatToken());
    }

    return true;
}

bool Parser::doNamespace(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, Err(Syn0040)));
    SWAG_CHECK(doNamespace(parent, result, false, false));
    return true;
}

bool Parser::doNamespace(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing)
{
    SWAG_CHECK(eatToken());
    return doNamespaceOnName(parent, result, forGlobal, forUsing);
}

bool Parser::doNamespaceOnName(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing, Token* privName)
{
    AstNode* namespaceNode;
    Scope*   oldScope = currentScope;
    Scope*   newScope = nullptr;
    bool     first    = true;

    // There is only one swag namespace, defined in the bootstrap. So if we redeclared it
    // in runtime, use the one from the bootstrap
    if (sourceFile->isRuntimeFile && token.text == g_LangSpec->name_Swag)
        currentScope = g_Workspace->bootstrapModule->files[0]->astRoot->ownerScope;

    bool scopeFilePriv = privName != nullptr;
    while (true)
    {
        namespaceNode = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, parent);
        if (privName)
            namespaceNode->token.text = privName->text;

        if (first)
            *result = namespaceNode;
        if (forGlobal)
            namespaceNode->flags |= AST_GLOBAL_NODE;
        first = false;

        switch (token.id)
        {
        case TokenId::Identifier:
            break;
        case TokenId::SymLeftCurly:
            if (!privName)
                return error(token, Err(Syn0094));
            break;
        case TokenId::SymSemiColon:
            return error(token, Err(Syn0093));
        default:
            if (!privName)
                return error(token, Fmt(Err(Syn0041), token.ctext()));
            break;
        }

        // Be sure this is not the swag namespace, except for a runtime file
        if (!sourceFile->isBootstrapFile && !sourceFile->isRuntimeFile)
            SWAG_VERIFY(!namespaceNode->token.text.compareNoCase(g_LangSpec->name_Swag), error(token, Fmt(Err(Syn0118), token.ctext())));

        // Add/Get namespace
        {
            ScopedLock lk(currentScope->symTable.mutex);
            auto       symbol = currentScope->symTable.findNoLock(namespaceNode->token.text);
            if (!symbol)
            {
                auto typeInfo  = makeType<TypeInfoNamespace>();
                typeInfo->name = namespaceNode->token.text;
                newScope       = Ast::newScope(namespaceNode, namespaceNode->token.text, ScopeKind::Namespace, currentScope);
                if (scopeFilePriv)
                    newScope->flags |= SCOPE_FILE_PRIV;
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                AddSymbolTypeInfo toAdd;
                toAdd.node       = namespaceNode;
                toAdd.typeInfo   = typeInfo;
                toAdd.kind       = SymbolKind::Namespace;
                toAdd.symbolName = currentScope->symTable.registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, &namespaceNode->token.text);
                currentScope->symTable.addSymbolTypeInfoNoLock(context, toAdd);
            }
            else if (symbol->kind != SymbolKind::Namespace)
            {
                Diagnostic diag{sourceFile, token.startLocation, token.endLocation, Fmt(Err(Err0305), symbol->name.c_str())};
                auto       note = Diagnostic::note(symbol->nodes.front(), symbol->nodes.front()->token, Nte(Nte0036));
                return context->report(diag, note);
            }
            else
                newScope = CastTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
        }

        if (privName)
            privName = nullptr;
        else
            SWAG_CHECK(eatToken());

        if (token.id != TokenId::SymDot)
        {
            if (forUsing)
            {
                while (parent->kind != AstNodeKind::File)
                    parent = parent->parent;
                parent->allocateExtension(ExtensionKind::Misc);
                parent->addAlternativeScope(newScope);
            }

            break;
        }

        SWAG_CHECK(eatToken(TokenId::SymDot));
        parent       = namespaceNode;
        currentScope = newScope;
    }

    currentScope   = oldScope;
    auto openCurly = token;

    if (forGlobal)
    {
        SWAG_CHECK(eatSemiCol("'#global namespace'"));
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
        }
    }
    else
    {
        auto startLoc = token.startLocation;

        // Content of namespace is toplevel
        if (token.id == TokenId::SymLeftCurly)
        {
            eatToken();

            Scoped scoped(this, newScope);
            while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
            {
                SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
            }

            SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the namespace body"));
        }
        else
        {
            Scoped scoped(this, newScope);
            SWAG_CHECK(doTopLevelInstruction(namespaceNode, &dummyResult));
        }
    }

    return true;
}

bool Parser::doGlobalCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    *result   = node;

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        SWAG_CHECK(doTopLevelInstruction(node, &dummyResult));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    *result   = node;

    bool isGlobal = currentScope->isGlobalOrImpl();
    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    if (isGlobal)
    {
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(node, &dummyResult));
        }
    }
    else
    {
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doEmbeddedInstruction(node, &dummyResult));
        }
    }

    node->token.endLocation = token.startLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind)
{
    auto   newScope = Ast::newScope(parent, "", scopeKind, currentScope);
    Scoped scoped(this, newScope);

    AstNode* statement;
    SWAG_CHECK(doCurlyStatement(parent, &statement));
    *result = statement;

    newScope->owner = statement;
    statement->flags |= AST_NEED_SCOPE;
    statement->byteCodeFct = ByteCodeGenJob::emitDebugNop;
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = SemanticJob::resolveScopedStmtAfter;

    return true;
}

bool Parser::doEmbeddedStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doScopedCurlyStatement(parent, result);

    // Empty statement
    if (token.id == TokenId::SymSemiColon)
        return error(token, Err(Syn0187), Hlp(Hlp0053));

    // One single line, but we need a scope too
    auto     newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped   scoped(this, newScope);
    AstNode* statement = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    *result            = statement;
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extSemantic()->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    statement->extSemantic()->semanticAfterFct  = SemanticJob::resolveScopedStmtAfter;
    statement->flags |= AST_NEED_SCOPE;
    newScope->owner = statement;
    SWAG_CHECK(doEmbeddedInstruction(statement, &dummyResult));
    return true;
}

bool Parser::doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    switch (kind)
    {
    case AstNodeKind::Statement:
        return doStatement(parent, result);
    case AstNodeKind::EnumDecl:
        return doEnumContent(parent, &dummyResult);
    case AstNodeKind::StructDecl:
        return doStructBody(parent, SyntaxStructType::Struct, &dummyResult);
    case AstNodeKind::InterfaceDecl:
        return doStructBody(parent, SyntaxStructType::Interface, &dummyResult);

    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
}

bool Parser::doStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doCurlyStatement(parent, result);

    bool isGlobal = currentScope->isGlobalOrImpl();
    if (isGlobal)
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        *result   = node;
        return doTopLevelInstruction(node, &dummyResult);
    }

    return doEmbeddedInstruction(parent, result);
}

void Parser::registerSubDecl(AstNode* subDecl)
{
    SWAG_ASSERT(subDecl->ownerFct);
    subDecl->ownerFct->subDecls.push_back(subDecl);
    subDecl->flags |= AST_NO_SEMANTIC | AST_SUB_DECL | AST_INTERNAL;

    AstAttrUse* newAttrUse = nullptr;

    // If parent is an attribute, then we will move it with the subdecl
    if (subDecl->parent->kind == AstNodeKind::AttrUse)
        subDecl = subDecl->parent;

    // Else if we are in an attruse block, we need to duplicate each of them, in order
    // for the subdecl to have its own attruse
    else if (subDecl->parent->kind == AstNodeKind::Statement)
    {
        auto testParent  = subDecl->parent;
        int  idxToInsert = 0;
        while (testParent)
        {
            while (testParent && testParent->kind == AstNodeKind::Statement)
                testParent = testParent->parent;
            if (!testParent)
                break;
            if (testParent->kind != AstNodeKind::AttrUse)
                break;
            if (!newAttrUse)
                newAttrUse = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, subDecl->sourceFile, subDecl->parent);

            // Clone all attributes
            CloneContext cloneContext;
            for (size_t i = 0; i < testParent->childs.size() - 1; i++) // Do not clone content
            {
                cloneContext.parent = newAttrUse;
                auto child          = testParent->childs[i]->clone(cloneContext);

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

    auto newParent = subDecl->parent;
    Ast::removeFromParent(subDecl);

    // :SubDeclParent
    while (newParent != sourceFile->astRoot && !(newParent->flags & AST_GLOBAL_NODE) && (newParent->kind != AstNodeKind::Namespace))
        newParent = newParent->parent;

    // Force the parent to be the new attribute, if defined
    if (newAttrUse)
    {
        // The last child must have the semanticAfterFct set
        // :AttrUseLastChild
        auto back = newAttrUse->childs.back();
        back->allocateExtension(ExtensionKind::Semantic);
        SWAG_ASSERT(!back->extSemantic()->semanticAfterFct);
        back->extSemantic()->semanticAfterFct = SemanticJob::resolveAttrUse;

        Ast::removeFromParent(newAttrUse);
        Ast::addChildBack(newParent, newAttrUse);
        newAttrUse->content = subDecl;
        subDecl->setOwnerAttrUse(newAttrUse);
        newParent = newAttrUse;
    }

    Ast::addChildBack(newParent, subDecl);
}

bool Parser::doCompilerScopeBreakable(AstNode* parent, AstNode** result)
{
    auto labelNode         = Ast::newNode<AstScopeBreakable>(this, AstNodeKind::ScopeBreakable, sourceFile, parent);
    *result                = labelNode;
    labelNode->semanticFct = SemanticJob::resolveScopeBreakable;

    SWAG_CHECK(eatToken());
    if (token.id != TokenId::SymLeftCurly)
    {
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0164), token.ctext())));
        labelNode->inheritTokenName(token);
        labelNode->inheritTokenLocation(token);
        SWAG_CHECK(eatToken());
    }

    ScopedBreakable scoped(this, labelNode);
    SWAG_CHECK(doEmbeddedInstruction(labelNode, &labelNode->block));
    return true;
}

bool Parser::doLeftInstruction(AstNode* parent, AstNode** result, AstWith* withNode)
{
    switch (token.id)
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
    default:
        break;
    }

    return true;
}

bool Parser::doEmbeddedInstruction(AstNode* parent, AstNode** result)
{
    relaxIdentifier(token);
    switch (token.id)
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
    case TokenId::Identifier:
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

    case TokenId::SymLeftCurly:
        SWAG_CHECK(doScopedCurlyStatement(parent, result, ScopeKind::EmptyStatement));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(eatToken());
        break;
    case TokenId::KwdUsing:
        SWAG_CHECK(doUsing(parent, result));
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
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        *result = attrUse;
        // We do not want a #[] to create a new scope when inside a function
        if (token.id == TokenId::SymLeftCurly)
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
        return error(token, Fmt(Err(Syn0121), token.ctext()));

    case TokenId::SymDot:
    {
        auto withNode = parent->findParent(AstNodeKind::With);
        SWAG_VERIFY(withNode, error(token, Err(Syn0180)));
        eatToken();
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Err(Syn0022)));
        return doLeftInstruction(parent, result, CastAst<AstWith>(withNode, AstNodeKind::With));
    }

    default:
        if (Tokenizer::isIntrinsicReturn(token.id))
            return error(token, Fmt(Err(Syn0179), token.ctext()), nullptr, Hnt(Hnt0008));
        return invalidTokenError(InvalidTokenError::EmbeddedInstruction);
    }

    return true;
}

bool Parser::doTopLevelInstruction(AstNode* parent, AstNode** result)
{
    // #global is invalid if afterGlobal is true
    if (token.id != TokenId::CompilerGlobal && token.id != TokenId::SymSemiColon)
        afterGlobal = true;
    // Do not generate AST if buildPass does not allow it
    if (sourceFile->buildPass < BuildPass::Syntax)
        return eatToken();

    switch (token.id)
    {
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doGlobalCurlyStatement(parent, result));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(eatToken());
        break;
    case TokenId::KwdVar:
    case TokenId::KwdConst:
        SWAG_CHECK(doVarDecl(parent, result));
        break;
    case TokenId::KwdTypeAlias:
        SWAG_CHECK(doTypeAlias(parent, result));
        break;
    case TokenId::KwdNameAlias:
        SWAG_CHECK(doNameAlias(parent, result));
        break;
    case TokenId::KwdPublic:
    case TokenId::KwdInternal:
        SWAG_CHECK(doPublicInternal(parent, result, false));
        break;
    case TokenId::KwdPrivate:
        SWAG_CHECK(doPrivate(parent, result));
        break;
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent, result));
        break;
    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent, result));
        break;
    case TokenId::KwdImpl:
        SWAG_CHECK(doImpl(parent, result));
        break;
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdInterface:
        SWAG_CHECK(doStruct(parent, result));
        break;
    case TokenId::KwdAttr:
        SWAG_CHECK(doAttrDecl(parent, result));
        break;
    case TokenId::KwdUsing:
        SWAG_CHECK(doUsing(parent, result));
        break;
    case TokenId::SymAttrStart:
    {
        AstAttrUse* attrUse;
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        *result = attrUse;
        SWAG_CHECK(doTopLevelInstruction(attrUse, &attrUse->content));
        if (attrUse->content)
            attrUse->content->setOwnerAttrUse(attrUse);
        break;
    }
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result));
        break;
    case TokenId::KwdFunc:
    case TokenId::KwdMethod:
    case TokenId::CompilerFuncTest:
    case TokenId::CompilerFuncInit:
    case TokenId::CompilerFuncDrop:
    case TokenId::CompilerFuncPreMain:
    case TokenId::CompilerFuncMain:
    case TokenId::CompilerFuncCompiler:
        SWAG_CHECK(doFuncDecl(parent, result));
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(doCompilerRunTopLevel(parent, result));
        break;
    case TokenId::CompilerIf:
        SWAG_CHECK(doCompilerIf(parent, result));
        break;
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent, result));
        break;
    case TokenId::CompilerError:
        SWAG_CHECK(doCompilerError(parent, result));
        break;
    case TokenId::CompilerWarning:
        SWAG_CHECK(doCompilerWarning(parent, result));
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent, result));
        break;
    case TokenId::CompilerDependencies:
        SWAG_CHECK(doCompilerDependencies(parent));
        break;
    case TokenId::CompilerImport:
        SWAG_CHECK(doCompilerImport(parent));
        break;
    case TokenId::CompilerLoad:
        SWAG_CHECK(doCompilerLoad(parent));
        break;
    case TokenId::CompilerPlaceHolder:
        SWAG_CHECK(doCompilerPlaceHolder(parent));
        break;
    case TokenId::CompilerGlobal:
        SWAG_CHECK(doCompilerGlobal(parent, result));
        break;
    case TokenId::CompilerForeignLib:
        SWAG_CHECK(doCompilerForeignLib(parent, result));
        break;
    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, result, IDENTIFIER_GLOBAL));
        break;

    default:
        return invalidTokenError(InvalidTokenError::TopLevelInstruction);
    }

    return true;
}
