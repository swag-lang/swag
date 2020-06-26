#include "pch.h"
#include "Diagnostic.h"
#include "Scoped.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"

bool SyntaxJob::doUsing(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(tokenizer.getToken(token));
    while (true)
    {
        auto node         = Ast::newNode<AstNode>(this, AstNodeKind::Using, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveUsing;
        if (result)
            *result = node;

        SWAG_CHECK(doIdentifierRef(node));

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
                    break;

                default:
                    return error(node->token, "global 'using' must be defined at the top of the file");
                }
            }
        }

        if (token.id == TokenId::SymComma)
        {
            SWAG_CHECK(eatToken());
            continue;
        }

        SWAG_CHECK(eatSemiCol("after 'using' declaration"));
        break;
    }

    return true;
}

bool SyntaxJob::doNamespace(AstNode* parent)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, "a namespace definition must appear either at file scope or immediately within another namespace definition"));
    SWAG_CHECK(tokenizer.getToken(token));

    AstNode* namespaceNode;
    Scope*   oldScope = currentScope;
    Scope*   newScope = nullptr;

    // A namespace registers its scope in the module scope, except if the file scope has a name
    if (currentScope->kind == ScopeKind::File && currentScope->name.empty())
        currentScope = currentScope->parentScope;

    while (true)
    {
        namespaceNode              = Ast::newNode<AstNode>(this, AstNodeKind::Namespace, sourceFile, parent);
        namespaceNode->semanticFct = SemanticJob::resolveNamespace;

        switch (token.id)
        {
        case TokenId::Identifier:
            break;
        case TokenId::SymLeftCurly:
            return syntaxError(token, "missing namespace name before '{'");
        default:
            return syntaxError(token, format("invalid namespace name '%s'", token.text.c_str()));
        }

        // Be sure this is not the swag namespace, except for a runtime file
        if (!sourceFile->swagFile)
            SWAG_VERIFY(token.text != "swag", syntaxError(token, "the 'swag' namespace is reserved by the compiler"));

        // Add/Get namespace
        namespaceNode->inheritTokenName(token);
        {
            scoped_lock lk(currentScope->symTable.mutex);
            auto        symbol = currentScope->symTable.findNoLock(namespaceNode->name);
            if (!symbol)
            {
                auto typeInfo           = g_Allocator.alloc<TypeInfoNamespace>();
                typeInfo->name          = namespaceNode->name;
                newScope                = Ast::newScope(namespaceNode, namespaceNode->name, ScopeKind::Namespace, currentScope);
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                currentScope->symTable.addSymbolTypeInfoNoLock(&context, namespaceNode, typeInfo, SymbolKind::Namespace);
                currentScope->addPublicNamespace(namespaceNode);
            }
            else if (symbol->kind != SymbolKind::Namespace)
            {
                auto       firstOverload = &symbol->defaultOverload;
                Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
                Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
                Utf8       note = "this is the other definition";
                Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
                return sourceFile->report(diag, &diagNote);
            }
            else
                newScope = static_cast<TypeInfoNamespace*>(symbol->overloads[0]->typeInfo)->scope;
        }

        SWAG_CHECK(tokenizer.getToken(token));
        if (token.id != TokenId::SymDot)
            break;
        SWAG_CHECK(eatToken(TokenId::SymDot));
        parent       = namespaceNode;
        currentScope = newScope;
    }

    currentScope   = oldScope;
    auto openCurly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    // Content of namespace is toplevel
    {
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode));
        }
    }

    SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, "no corresponding '}' has been found"));
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doGlobalCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = node;

    auto openCurly = token;
    SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

    while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        SWAG_CHECK(doTopLevelInstruction(node));

    SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, "no corresponding '}' has been found"));
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = node;

    bool isGlobal  = currentScope->isGlobal() || currentScope->kind == ScopeKind::Struct;
    auto openCurly = token;
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

    SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, "no corresponding '}' has been found"));
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doScopedCurlyStatement(AstNode* parent, AstNode** result)
{
    auto     newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    AstNode* statement;

    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(doCurlyStatement(parent, &statement));
        statement->flags |= AST_NEED_SCOPE;
        statement->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    }

    if (result)
        *result = statement;
    return true;
}

bool SyntaxJob::doEmbeddedStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doScopedCurlyStatement(parent, result);

    // Empty statement
    if (token.id == TokenId::SymSemiColon)
    {
        auto node = Ast::newNode(sourceFile, AstNodeKind::Statement, parent, this);
        if (result)
            *result = node;
        SWAG_CHECK(eatToken());
        return true;
    }

    // One single line, but we need a scope too
    auto     newScope = Ast::newScope(parent, "", ScopeKind::Statement, currentScope);
    Scoped   scoped(this, newScope);
    AstNode* statement = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = statement;
    statement->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    statement->flags |= AST_NEED_SCOPE;
    SWAG_CHECK(doEmbeddedInstruction(statement));
    return true;
}

bool SyntaxJob::doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    switch (kind)
    {
    case AstNodeKind::Statement:
        return doStatement(parent, result);
    case AstNodeKind::EnumDecl:
        return doEnumContent(parent);
    case AstNodeKind::StructDecl:
        return doStructContent(parent);

    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
}

bool SyntaxJob::doStatement(AstNode* parent, AstNode** result)
{
    if (token.id == TokenId::SymLeftCurly)
        return doCurlyStatement(parent, result);

    bool isGlobal = currentScope->isGlobal();
    if (isGlobal)
    {
        auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
        if (result)
            *result = node;
        return doTopLevelInstruction(node);
    }

    return doEmbeddedInstruction(parent, result);
}

bool SyntaxJob::doEmbeddedInstruction(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent));
        break;
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doScopedCurlyStatement(parent, result));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdReturn:
        SWAG_CHECK(doReturn(parent, result));
        SWAG_CHECK(eatSemiCol("after return expression"));
        break;
    case TokenId::KwdUsing:
        SWAG_CHECK(doUsing(parent, result));
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
    case TokenId::KwdLet:
    case TokenId::KwdVar:
    case TokenId::KwdConst:
        SWAG_CHECK(doVarDecl(parent, result));
        break;
    case TokenId::KwdDefer:
        SWAG_CHECK(doDefer(parent, result));
        break;
    case TokenId::SymBackTick:
    case TokenId::Identifier:
    case TokenId::Intrinsic:
    case TokenId::SymLeftParen:
        SWAG_CHECK(doAffectExpression(parent, result));
        break;
    case TokenId::KwdInit:
        SWAG_CHECK(doInit(parent, result));
        break;
    case TokenId::KwdDrop:
        SWAG_CHECK(doDrop(parent, result));
        break;
    case TokenId::KwdBreak:
        SWAG_CHECK(doBreak(parent, result));
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
    case TokenId::CompilerRun:
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doCompilerRunStatement(parent, result));
        break;
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result, CompilerAstKind::EmbeddedInstruction));
        break;
    case TokenId::SymAttrStart:
        SWAG_CHECK(doAttrUse(parent));
        break;
    case TokenId::KwdFunc:
        moveAttributes(parent, sourceFile->astRoot);
        SWAG_CHECK(doFuncDecl(sourceFile->astRoot, result));
        break;
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdInterface:
        moveAttributes(parent, sourceFile->astRoot);
        SWAG_CHECK(doStruct(sourceFile->astRoot, result));
        break;
    case TokenId::KwdLabel:
        SWAG_CHECK(doLabel(parent, result));
        break;
    default:
        return invalidTokenError(InvalidTokenError::EmbeddedInstruction);
    }

    return true;
}

bool SyntaxJob::doLabel(AstNode* parent, AstNode** result)
{
    auto labelNode = Ast::newNode<AstLabelBreakable>(this, AstNodeKind::LabelBreakable, sourceFile, parent);
    if (result)
        *result = labelNode;
    labelNode->semanticFct = SemanticJob::resolveLabel;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, syntaxError(labelNode->token, "missing label identifier before '{'"));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid label identifier '%s'", token.text.c_str())));
    labelNode->inheritTokenName(token);
    labelNode->inheritTokenLocation(token);

    ScopedBreakable scoped(this, labelNode);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doEmbeddedInstruction(labelNode, &labelNode->block));
    return true;
}

void SyntaxJob::moveAttributes(AstNode* from, AstNode* to)
{
    VectorNative<AstNode*> attrs;
    for (int i = (int) from->childs.size() - 1; i >= 0; i--)
    {
        if (from->childs[i]->kind != AstNodeKind::AttrUse)
            break;
        attrs.push_back(from->childs[i]);
    }

    for (auto p : attrs)
    {
        Ast::removeFromParent(p);
        Ast::addChildBack(to, p);
    }
}

bool SyntaxJob::doTopLevelInstruction(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doGlobalCurlyStatement(parent));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdLet:
    case TokenId::KwdVar:
    case TokenId::KwdConst:
        SWAG_CHECK(doVarDecl(parent));
        break;
    case TokenId::KwdTypeAlias:
        SWAG_CHECK(doTypeAlias(parent));
        break;
    case TokenId::KwdPublic:
    case TokenId::KwdPrivate:
    case TokenId::KwdReadOnly:
        SWAG_CHECK(doGlobalAttributeExpose(parent));
        break;
    case TokenId::KwdNamespace:
        SWAG_CHECK(doNamespace(parent));
        break;
    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent));
        break;
    case TokenId::KwdImpl:
        SWAG_CHECK(doImpl(parent));
        break;
    case TokenId::KwdStruct:
    case TokenId::KwdUnion:
    case TokenId::KwdInterface:
        SWAG_CHECK(doStruct(parent));
        break;
    case TokenId::KwdAttr:
        SWAG_CHECK(doAttrDecl(parent));
        break;
    case TokenId::KwdUsing:
        SWAG_CHECK(doUsing(parent));
        break;
    case TokenId::SymAttrStart:
        SWAG_CHECK(doAttrUse(parent));
        break;
    case TokenId::DocComment:
        SWAG_CHECK(doDocComment(parent));
        break;
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result, CompilerAstKind::TopLevelInstruction));
        break;
    case TokenId::KwdFunc:
    case TokenId::CompilerFuncTest:
    case TokenId::CompilerFuncInit:
    case TokenId::CompilerFuncDrop:
    case TokenId::CompilerFuncMain:
        SWAG_CHECK(doFuncDecl(parent, result));
        break;
    case TokenId::CompilerRun:
        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymLeftCurly)
            SWAG_CHECK(doFuncDecl(parent, nullptr, TokenId::CompilerRun));
        else
            SWAG_CHECK(doCompilerRunExpression(parent));
        break;
    case TokenId::CompilerForeignLib:
        SWAG_CHECK(doCompilerForeignLib(parent));
        break;
    case TokenId::CompilerIf:
        SWAG_CHECK(doCompilerIf(parent));
        break;
    case TokenId::CompilerUnitTest:
        SWAG_CHECK(doCompilerUnitTest());
        break;
    case TokenId::CompilerModule:
        SWAG_CHECK(doCompilerModule());
        break;
    case TokenId::CompilerAssert:
        SWAG_CHECK(doCompilerAssert(parent));
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent));
        break;
    case TokenId::CompilerImport:
        SWAG_CHECK(doCompilerImport(parent));
        break;
    default:
        return invalidTokenError(InvalidTokenError::TopLevelInstruction);
    }

    return true;
}
