#include "pch.h"
#include "Scoped.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool Parser::doWith(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());
    auto node = Ast::newNode<AstWith>(this, AstNodeKind::With, sourceFile, parent);
    if (result)
        *result = node;
    AstNode* id = nullptr;

    if (token.id == TokenId::KwdVar)
    {
        SWAG_CHECK(doVarDecl(node, &id));
        if (id->kind != AstNodeKind::VarDecl)
        {
            Diagnostic diag{id->sourceFile, id->childs.front()->token.startLocation, id->childs.back()->token.endLocation, Err(Syn0157)};
            Diagnostic note{Hlp(Hlp0039), DiagnosticLevel::Help};
            return context->report(diag, &note);
        }

        SWAG_ASSERT(id->extension->semantic->semanticAfterFct == SemanticJob::resolveVarDeclAfter);
        id->extension->semantic->semanticAfterFct = SemanticJob::resolveWithVarDeclAfter;
        node->id.push_back(id->token.text);
    }
    else
    {
        SWAG_CHECK(doAffectExpression(node, &id));

        if (id->kind == AstNodeKind::StatementNoScope)
        {
            Diagnostic diag{node->sourceFile, id->childs.front()->token.startLocation, id->childs.back()->token.endLocation, Err(Syn0157)};
            Diagnostic note{Hlp(Hlp0039), DiagnosticLevel::Help};
            return context->report(diag, &note);
        }

        if (id->kind != AstNodeKind::IdentifierRef &&
            id->kind != AstNodeKind::VarDecl &&
            id->kind != AstNodeKind::AffectOp)
            return error(node->token, Err(Syn0148));

        id->allocateExtension(ExtensionKind::Semantic);
        if (id->kind == AstNodeKind::IdentifierRef)
        {
            SWAG_ASSERT(!id->extension->semantic->semanticAfterFct);
            id->extension->semantic->semanticAfterFct = SemanticJob::resolveWith;
            for (int i = 0; i < id->childs.size(); i++)
                node->id.push_back(id->childs[i]->token.text);
        }
        else if (id->kind == AstNodeKind::VarDecl)
        {
            SWAG_ASSERT(id->extension->semantic->semanticAfterFct == SemanticJob::resolveVarDeclAfter);
            id->extension->semantic->semanticAfterFct = SemanticJob::resolveWithVarDeclAfter;
            node->id.push_back(id->token.text);
        }
        else if (id->kind == AstNodeKind::AffectOp)
        {
            id = id->childs.front();
            SWAG_ASSERT(id->extension->semantic->semanticAfterFct == SemanticJob::resolveAfterAffectLeft);
            id->extension->semantic->semanticAfterFct = SemanticJob::resolveWithAfterAffectLeft;
            for (int i = 0; i < id->childs.size(); i++)
                node->id.push_back(id->childs[i]->token.text);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    SWAG_CHECK(doEmbeddedStatement(node));
    return true;
}

bool Parser::doCompilerScopeFile(AstNode* parent, AstNode** result)
{
    Token privName = token;
    SWAG_CHECK(eatToken());
    privName.id   = TokenId::Identifier;
    privName.text = Fmt("__privns_%d", g_UniqueID.fetch_add(1));
    SWAG_CHECK(doNamespaceOnName(parent, result, false, true, &privName));
    return true;
}

bool Parser::doUsing(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(eatToken());

    if (token.id == TokenId::KwdNamespace)
    {
        SWAG_CHECK(doNamespace(parent, result, false, true));
        return true;
    }

    while (true)
    {
        // using var
        if (token.id == TokenId::KwdVar)
        {
            AstNode* varNode;
            SWAG_CHECK(doVarDecl(parent, &varNode));

            auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Using, sourceFile, parent);
            node->semanticFct = SemanticJob::resolveUsing;
            if (result)
                *result = node;
            Ast::newIdentifierRef(sourceFile, varNode->token.text, node, this);
            return true;
        }

        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Using, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveUsing;
        if (result)
            *result = node;

        SWAG_CHECK(doIdentifierRef(node, nullptr, IDENTIFIER_NO_PARAMS));

        // We must ensure that no job can be run before the using
        if (!node->ownerFct)
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
                    if (((AstAttrUse*) child)->specFlags & AST_SPEC_ATTRUSE_GLOBAL)
                        break;
                default:
                {
                    Diagnostic diag{node, Err(Syn0036)};
                    Diagnostic note{child, child->token, Nte(Nte0024), DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
                }
            }
        }

        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            continue;
        }

        SWAG_CHECK(eatSemiCol("'using' declaration"));
        break;
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

        if (first && result)
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
            return error(token, Fmt(Err(Syn0041), token.ctext()));
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
                auto typeInfo           = allocType<TypeInfoNamespace>();
                typeInfo->name          = namespaceNode->token.text;
                newScope                = Ast::newScope(namespaceNode, namespaceNode->token.text, ScopeKind::Namespace, currentScope);
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                currentScope->symTable.addSymbolTypeInfoNoLock(context, namespaceNode, typeInfo, SymbolKind::Namespace);
            }
            else if (symbol->kind != SymbolKind::Namespace)
            {
                Diagnostic diag{sourceFile, token.startLocation, token.endLocation, Fmt(Err(Err0305), symbol->name.c_str())};
                Diagnostic note{symbol->nodes.front(), symbol->nodes.front()->token, Nte(Nte0036), DiagnosticLevel::Note};
                return context->report(diag, &note);
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
                parent->allocateExtension(ExtensionKind::AltScopes);
                parent->addAlternativeScope(newScope, scopeFilePriv ? ALTSCOPE_SCOPEFILE : true);
            }

            break;
        }

        SWAG_CHECK(eatToken(TokenId::SymDot));
        parent       = namespaceNode;
        currentScope = newScope;
    }

    currentScope   = oldScope;
    auto openCurly = token;

    if (namespaceNode && forUsing)
        namespaceNode->specFlags |= AST_SPEC_NAMESPACE_USING;

    if (forGlobal)
    {
        SWAG_CHECK(eatSemiCol("'#global namespace'"));
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode));
        }
    }
    else
    {
        auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the namespace body"));

        // Content of namespace is toplevel
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode));
        }

        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the namespace body"));
    }

    return true;
}

bool Parser::doGlobalCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = node;

    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        SWAG_CHECK(doTopLevelInstruction(node));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = node;

    bool isGlobal = currentScope->isGlobalOrImpl();
    auto startLoc = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
    {
        if (isGlobal)
        {
            SWAG_CHECK(doTopLevelInstruction(node));
        }
        else
        {
            SWAG_CHECK(doEmbeddedInstruction(node));
        }
    }

    node->token.endLocation = token.startLocation;
    SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    return true;
}

bool Parser::doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind)
{
    auto     newScope = Ast::newScope(parent, "", scopeKind, currentScope);
    AstNode* statement;

    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(doCurlyStatement(parent, &statement));
        newScope->owner = statement;
        statement->flags |= AST_NEED_SCOPE;
        statement->byteCodeFct = ByteCodeGenJob::emitDebugNop;
        statement->allocateExtension(ExtensionKind::Semantic);
        statement->extension->semantic->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
        statement->extension->semantic->semanticAfterFct  = SemanticJob::resolveScopedStmtAfter;
    }

    if (result)
        *result = statement;
    return true;
}

bool Parser::doEmbeddedStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doScopedCurlyStatement(parent, result);

    // Empty statement
    SWAG_VERIFY(token.id != TokenId::SymSemiColon, error(token, Err(Syn0187)));

    // One single line, but we need a scope too
    auto     newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped   scoped(this, newScope);
    AstNode* statement = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = statement;
    statement->allocateExtension(ExtensionKind::Semantic);
    statement->extension->semantic->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    statement->extension->semantic->semanticAfterFct  = SemanticJob::resolveScopedStmtAfter;
    statement->flags |= AST_NEED_SCOPE;
    newScope->owner = statement;
    SWAG_CHECK(doEmbeddedInstruction(statement));
    return true;
}

bool Parser::doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    switch (kind)
    {
    case AstNodeKind::Statement:
        return doStatement(parent, result);
    case AstNodeKind::EnumDecl:
        return doEnumContent(parent);
    case AstNodeKind::StructDecl:
        return doStructBody(parent, SyntaxStructType::Struct);

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
        if (result)
            *result = node;
        return doTopLevelInstruction(node);
    }

    return doEmbeddedInstruction(parent, result);
}

void Parser::registerSubDecl(AstNode* subDecl)
{
    SWAG_ASSERT(subDecl->ownerFct);
    subDecl->ownerFct->subDecls.push_back(subDecl);
    subDecl->flags |= AST_NO_SEMANTIC | AST_SUB_DECL | AST_PRIVATE;

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
            for (int i = 0; i < testParent->childs.size() - 1; i++) // Do not clone content
            {
                cloneContext.parent = newAttrUse;
                auto child          = testParent->childs[i]->clone(cloneContext);

                // Only the last attribute of the block needs to have a semanticAfterFct, so
                // we rest it, and we will set it later for the last child
                // :AttrUseLastChild
                if (child->extension && child->extension->semantic)
                    child->extension->semantic->semanticAfterFct = nullptr;

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
        SWAG_ASSERT(!back->extension->semantic->semanticAfterFct);
        back->extension->semantic->semanticAfterFct = SemanticJob::resolveAttrUse;

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
    auto labelNode = Ast::newNode<AstScopeBreakable>(this, AstNodeKind::ScopeBreakable, sourceFile, parent);
    if (result)
        *result = labelNode;
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
    case TokenId::KwdContinue:
        SWAG_CHECK(doContinue(parent, result));
        break;
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent, result));
        break;
    case TokenId::CompilerInline:
        SWAG_CHECK(doCompilerInline(parent, result));
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
        if (result)
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
    case TokenId::KwdConstMethod:
    case TokenId::KwdFunc:
    {
        AstNode* subFunc;
        SWAG_CHECK(doFuncDecl(parent, &subFunc));
        if (result)
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
        if (result)
            *result = subDecl;
        registerSubDecl(subDecl);
        break;
    }

    case TokenId::CompilerScope:
        SWAG_CHECK(doCompilerScopeBreakable(parent, result));
        break;
    case TokenId::KwdAlias:
        SWAG_CHECK(doAlias(parent, result));
        break;

    case TokenId::KwdPublic:
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
    case TokenId::KwdAlias:
        SWAG_CHECK(doAlias(parent, result));
        break;
    case TokenId::KwdPublic:
    case TokenId::KwdPrivate:
        SWAG_CHECK(doGlobalAttributeExpose(parent, result, false));
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
    case TokenId::CompilerScopeFile:
        SWAG_CHECK(doCompilerScopeFile(parent, result));
        break;
    case TokenId::SymAttrStart:
    {
        AstAttrUse* attrUse;
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        if (result)
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
    case TokenId::KwdConstMethod:
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
    {
        AstNode* identifierRef = nullptr;
        SWAG_CHECK(doIdentifierRef(parent, &identifierRef));
        identifierRef->flags |= AST_GLOBAL_CALL;
        if (result)
            *result = identifierRef;
        break;
    }

    default:
        return invalidTokenError(InvalidTokenError::TopLevelInstruction);
    }

    return true;
}
