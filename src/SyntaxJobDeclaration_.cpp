#include "pch.h"
#include "Diagnostic.h"
#include "Scoped.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Workspace.h"
#include "Module.h"
#include "ByteCodeGenJob.h"

bool SyntaxJob::doUsing(AstNode* parent, AstNode** result)
{
    SWAG_CHECK(tokenizer.getToken(token));
    while (true)
    {
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
                case AstNodeKind::AttrUse:
                    if (((AstAttrUse*) child)->isGlobal)
                        break;
                    return error(node->token, Msg0386);
                case AstNodeKind::CompilerImport:
                case AstNodeKind::CompilerAssert:
                case AstNodeKind::CompilerForeignLib:
                case AstNodeKind::Using:
                case AstNodeKind::IdentifierRef:
                case AstNodeKind::Namespace:
                    break;

                default:
                    return error(node->token, Msg0386);
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

bool SyntaxJob::doNamespace(AstNode* parent, AstNode** result)
{
    SWAG_VERIFY(currentScope->isGlobal(), error(token, Msg0388));
    SWAG_CHECK(doNamespace(parent, result, false));
    return true;
}

bool SyntaxJob::doNamespace(AstNode* parent, AstNode** result, bool forGlobal)
{
    SWAG_CHECK(tokenizer.getToken(token));

    AstNode* namespaceNode;
    Scope*   oldScope = currentScope;
    Scope*   newScope = nullptr;
    bool     first    = true;

    // There'is only one swag namespace, defined in the bootstrap. So if we redeclared it
    // in runtime, use the one from the bootstrap
    if (sourceFile->isRuntimeFile && token.text == "swag")
        currentScope = g_Workspace.bootstrapModule->files[0]->astRoot->ownerScope;

    while (true)
    {
        namespaceNode              = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, parent);
        namespaceNode->semanticFct = SemanticJob::resolveNamespace;
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
            return error(token, Msg0389);
        case TokenId::SymSemiColon:
            return error(token, Msg0390);
        default:
            return error(token, format(Msg0391, token.text.c_str()));
        }

        // Be sure this is not the swag namespace, except for a runtime file
        if (!sourceFile->isBootstrapFile && !sourceFile->isRuntimeFile)
            SWAG_VERIFY(token.text != "swag", error(token, Msg0392));
        //SWAG_VERIFY(token.text != sourceFile->module->name, error(token, format(Msg0393, token.text.c_str())));

        // Add/Get namespace
        {
            scoped_lock lk(currentScope->symTable.mutex);
            auto        symbol = currentScope->symTable.findNoLock(namespaceNode->token.text);
            if (!symbol)
            {
                auto typeInfo           = allocType<TypeInfoNamespace>();
                typeInfo->name          = namespaceNode->token.text;
                newScope                = Ast::newScope(namespaceNode, namespaceNode->token.text, ScopeKind::Namespace, currentScope);
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                currentScope->symTable.addSymbolTypeInfoNoLock(&context, namespaceNode, typeInfo, SymbolKind::Namespace);
            }
            else if (symbol->kind != SymbolKind::Namespace)
            {
                auto       firstOverload = &symbol->defaultOverload;
                Utf8       msg           = format(Msg0886, symbol->name.c_str());
                Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
                Utf8       note = Msg0884;
                Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
                return sourceFile->report(diag, &diagNote);
            }
            else
                newScope = CastTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
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
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly));

        // Content of namespace is toplevel
        Scoped scoped(this, newScope);
        while (token.id != TokenId::EndOfFile && token.id != TokenId::SymRightCurly)
        {
            SWAG_CHECK(doTopLevelInstruction(namespaceNode));
        }

        SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, Msg0880));
        SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    }

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

    SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, Msg0880));
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doCurlyStatement(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstNode>(this, AstNodeKind::Statement, sourceFile, parent);
    if (result)
        *result = node;

    bool isGlobal  = currentScope->isGlobalOrImpl();
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

    SWAG_CHECK(verifyError(openCurly, token.id != TokenId::EndOfFile, Msg0880));
    node->token.endLocation = token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymRightCurly));
    return true;
}

bool SyntaxJob::doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind)
{
    auto     newScope = Ast::newScope(parent, "", scopeKind, currentScope);
    AstNode* statement;

    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(doCurlyStatement(parent, &statement));
        newScope->owner = statement;
        statement->flags |= AST_NEED_SCOPE;
        statement->allocateExtension();
        statement->byteCodeFct                  = ByteCodeGenJob::emitDebugNop;
        statement->extension->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
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
    statement->allocateExtension();
    statement->extension->semanticBeforeFct = SemanticJob::resolveScopedStmtBefore;
    statement->flags |= AST_NEED_SCOPE;
    newScope->owner = statement;
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
        return doStructBody(parent, SyntaxStructType::Struct);

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

void SyntaxJob::registerSubDecl(AstNode* subDecl)
{
    subDecl->ownerFct->subDecls.push_back(subDecl);
    subDecl->attributeFlags |= ATTRIBUTE_PRIVATE;
    subDecl->flags |= AST_NO_SEMANTIC | AST_SUB_DECL;

    // Move to the root
    if (subDecl->parent->kind == AstNodeKind::AttrUse)
        subDecl = subDecl->parent;
    auto newParent = subDecl->parent;
    Ast::removeFromParent(subDecl);

    while (newParent != sourceFile->astRoot && !(newParent->flags & AST_GLOBAL_NODE) && (newParent->kind != AstNodeKind::Namespace))
        newParent = newParent->parent;

    Ast::addChildBack(newParent, subDecl);
}

bool SyntaxJob::doEmbeddedInstruction(AstNode* parent, AstNode** result)
{
    switch (token.id)
    {
    case TokenId::SymLeftCurly:
        SWAG_CHECK(doScopedCurlyStatement(parent, result, ScopeKind::EmptyStatement));
        break;
    case TokenId::SymSemiColon:
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdReturn:
        SWAG_CHECK(doReturn(parent, result));
        SWAG_CHECK(eatSemiCol("return expression"));
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
    case TokenId::KwdVar:
    case TokenId::KwdConst:
        SWAG_CHECK(doVarDecl(parent, result));
        break;
    case TokenId::KwdDefer:
    case TokenId::KwdErrDefer:
        SWAG_CHECK(doDefer(parent, result));
        break;
    case TokenId::KwdTry:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryAssume(parent, result));
        break;
    case TokenId::KwdCatch:
        SWAG_CHECK(doCatch(parent, result));
        break;
    case TokenId::KwdThrow:
        SWAG_CHECK(doThrow(parent, result));
        break;
    case TokenId::KwdDiscard:
        SWAG_CHECK(doDiscard(parent, result));
        break;
    case TokenId::SymBackTick:
    case TokenId::CompilerScopeFct:
    case TokenId::Identifier:
    case TokenId::IntrinsicPrint:
    case TokenId::IntrinsicAssert:
    case TokenId::IntrinsicBcDbg:
    case TokenId::IntrinsicPanic:
    case TokenId::IntrinsicErrorMsg:
    case TokenId::IntrinsicFree:
    case TokenId::IntrinsicMemCpy:
    case TokenId::IntrinsicMemMove:
    case TokenId::IntrinsicMemSet:
    case TokenId::IntrinsicSetContext:
    case TokenId::IntrinsicGetContext:
    case TokenId::IntrinsicAtomicAdd:
    case TokenId::IntrinsicAtomicAnd:
    case TokenId::IntrinsicAtomicOr:
    case TokenId::IntrinsicAtomicXor:
    case TokenId::IntrinsicAtomicXchg:
    case TokenId::IntrinsicAtomicCmpXchg:
    case TokenId::IntrinsicSetErr:
    case TokenId::SymLeftParen:
    case TokenId::SymColon:
        SWAG_CHECK(doAffectExpression(parent, result));
        break;
    case TokenId::IntrinsicInit:
        SWAG_CHECK(doInit(parent, result));
        break;
    case TokenId::IntrinsicDrop:
    case TokenId::IntrinsicPostCopy:
    case TokenId::IntrinsicPostMove:
        SWAG_CHECK(doDropCopyMove(parent, result));
        break;
    case TokenId::IntrinsicReloc:
        SWAG_CHECK(doReloc(parent, result));
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
    case TokenId::CompilerSemError:
        SWAG_CHECK(doCompilerSemError(parent, result, true));
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
        SWAG_CHECK(doCompilerRunEmbedded(parent, result));
        break;
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result, CompilerAstKind::EmbeddedInstruction));
        break;
    case TokenId::SymAttrStart:
    {
        AstAttrUse* attrUse;
        SWAG_CHECK(doAttrUse(parent, (AstNode**) &attrUse));
        if (result)
            *result = attrUse;
        SWAG_CHECK(doEmbeddedInstruction(attrUse, &attrUse->content));
        if (attrUse->content)
            attrUse->content->setOwnerAttrUse(attrUse);
        break;
    }

    case TokenId::KwdEnum:
        SWAG_CHECK(doEnum(parent, result));
        break;

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
    case TokenId::KwdTypeSet:
    case TokenId::KwdInterface:
    {
        AstNode* subDecl;
        SWAG_CHECK(doStruct(parent, &subDecl));
        if (result)
            *result = subDecl;
        registerSubDecl(subDecl);
        break;
    }

    case TokenId::KwdLabel:
        SWAG_CHECK(doLabel(parent, result));
        break;
    case TokenId::KwdAlias:
        SWAG_CHECK(doAlias(parent, result));
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
    SWAG_VERIFY(token.id != TokenId::SymLeftCurly, error(labelNode->token, Msg0394));
    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, format(Msg0395, token.text.c_str())));
    labelNode->inheritTokenName(token);
    labelNode->inheritTokenLocation(token);

    ScopedBreakable scoped(this, labelNode);
    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_CHECK(doEmbeddedInstruction(labelNode, &labelNode->block));
    return true;
}

bool SyntaxJob::doTopLevelInstruction(AstNode* parent, AstNode** result)
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
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdVar:
    case TokenId::KwdConst:
        SWAG_CHECK(doVarDecl(parent, result));
        break;
    case TokenId::KwdAlias:
        SWAG_CHECK(doAlias(parent, result));
        break;
    case TokenId::KwdPublic:
    case TokenId::KwdProtected:
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
    case TokenId::KwdTypeSet:
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
        if (result)
            *result = attrUse;
        SWAG_CHECK(doTopLevelInstruction(attrUse, &attrUse->content));
        if (attrUse->content)
            attrUse->content->setOwnerAttrUse(attrUse);
        break;
    }
    case TokenId::CompilerAst:
        SWAG_CHECK(doCompilerAst(parent, result, CompilerAstKind::TopLevelInstruction));
        break;
    case TokenId::KwdFunc:
    case TokenId::CompilerFuncTest:
    case TokenId::CompilerFuncInit:
    case TokenId::CompilerFuncDrop:
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
    case TokenId::CompilerSemError:
        SWAG_CHECK(doCompilerSemError(parent, result, false));
        break;
    case TokenId::CompilerPrint:
        SWAG_CHECK(doCompilerPrint(parent, result));
        break;
    case TokenId::CompilerImport:
        SWAG_CHECK(doCompilerImport(parent));
        break;
    case TokenId::CompilerPlaceHolder:
        SWAG_CHECK(doCompilerPlaceHolder(parent));
        break;
    case TokenId::CompilerGlobal:
        SWAG_CHECK(doCompilerGlobal(parent, result));
        break;
    case TokenId::Identifier:
    {
        AstNode* identifierRef = nullptr;
        SWAG_CHECK(doIdentifierRef(parent, &identifierRef));
        identifierRef->flags |= AST_GLOBAL_IDENTIFIER;
        if (result)
            *result = identifierRef;
        break;
    }

    default:
        return invalidTokenError(InvalidTokenError::TopLevelInstruction);
    }

    return true;
}
