#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "ErrorIds.h"

bool Parser::testIsSingleIdentifier(AstNode* node)
{
    if (node->kind != AstNodeKind::IdentifierRef ||
        node->childs.size() > 1 ||
        node->childs.back()->kind != AstNodeKind::Identifier)
    {
        return false;
    }

    return true;
}

bool Parser::testIsValidUserName(AstNode* node)
{
    if (node->parent && (node->parent->flags & AST_GENERATED))
        return true;
    if (node->flags & AST_GENERATED)
        return true;

    // An identifier that starts with '__' is reserved for internal usage !
    if (!sourceFile->isGenerated && !sourceFile->isBootstrapFile && !sourceFile->isRuntimeFile)
    {
        if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
            return false;
    }

    return true;
}

bool Parser::checkIsValidUserName(AstNode* node, Token* loc)
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (!testIsValidUserName(node))
        return error(loc ? *loc : node->token, Fmt(Err(Err1122), node->token.ctext()));

    return true;
}

bool Parser::checkIsSingleIdentifier(AstNode* node, const char* msg)
{
    if (!testIsSingleIdentifier(node))
        return error(node, Fmt(Err(Err1062), msg));

    return true;
}

bool Parser::checkIsIdentifier(TokenParse& tokenParse, const char* msg)
{
    if (tokenParse.id == TokenId::Identifier)
        return true;
    return invalidIdentifierError(tokenParse, msg);
}

bool Parser::doIdentifier(AstNode* parent, uint32_t identifierFlags)
{
    TokenParse scopeUpValue;

    // #up to change the scope
    if (token.id == TokenId::CompilerUp)
    {
        auto upToken = token;
        SWAG_CHECK(eatToken());
        auto startLoc       = token.startLocation;
        token.startLocation = upToken.startLocation;

        if (token.id == TokenId::SymQuestion)
            return invalidIdentifierError(token);

        scopeUpValue.id               = TokenId::CompilerUp;
        scopeUpValue.literalType      = LiteralType::TT_UNTYPED_INT;
        scopeUpValue.literalValue.u64 = 1;
        scopeUpValue.startLocation    = upToken.startLocation;
        scopeUpValue.endLocation      = upToken.endLocation;

        if (token.id == TokenId::SymLeftParen)
        {
            SWAG_CHECK(eatToken());

            if (token.id != TokenId::LiteralNumber)
                return error(token, Fmt(Err(Err1211), token.ctext()));

            if (token.literalType != LiteralType::TT_UNTYPED_INT && token.literalType != LiteralType::TT_U8)
                return error(token, Fmt(Err(Err1210), token.ctext()));
            if (token.literalValue.u64 > 255)
                return error(token, Fmt(Err(Err1209), token.literalValue.u64));
            if (token.literalValue.u8 == 0)
                return error(token, Fmt(Err(Err1082), token.ctext()));

            scopeUpValue = token;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
        }
    }

    if (token.id != TokenId::Identifier &&
        token.id != TokenId::NativeType &&
        token.id != TokenId::SymQuestion &&
        token.id != TokenId::CompilerSelf &&
        !Tokenizer::isIntrinsicReturn(token.id) &&
        !Tokenizer::isIntrinsicNoReturn(token.id))
    {
        return invalidIdentifierError(token);
    }

    auto identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
    identifier->inheritTokenLocation(token);
    identifier->semanticFct = SemanticJob::resolveIdentifier;

    if (scopeUpValue.id != TokenId::Invalid)
    {
        identifier->allocateIdentifierExtension();
        identifier->identifierExtension->scopeUpMode  = IdentifierScopeUpMode::Count;
        identifier->identifierExtension->scopeUpValue = scopeUpValue;
    }

    if (identifier->flags & AST_IN_FUNC_DECL_PARAMS)
        identifier->specFlags |= AstIdentifier::SPECFLAG_NO_INLINE;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == g_LangSpec->name_Self)
    {
        SWAG_VERIFY(parent->ownerStructScope, context->report({identifier, Err(Err1135)}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
    }

    if (!(token.flags & TOKENPARSE_LAST_EOL) && !(identifierFlags & IDENTIFIER_NO_GEN_PARAMS))
    {
        // Generic arguments
        if (token.id == TokenId::SymQuote)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->flags |= AST_NO_BYTECODE;
        }
    }

    // Function call parameters
    if (!(token.flags & TOKENPARSE_LAST_EOL) && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS))
    {
        if (token.id == TokenId::SymLeftParen)
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
            {
                Diagnostic diag{identifier, token, Err(Err1128)};
                return context->report(diag);
            }

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!(token.flags & TOKENPARSE_LAST_BLANK) && token.id == TokenId::SymLeftCurly)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->specFlags |= AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT;
        }
    }

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (!(identifierFlags & IDENTIFIER_NO_PARAMS))
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
                return context->report({identifier, token, Err(Err1120)});
            Ast::removeFromParent(identifier);
            SWAG_CHECK(doArrayPointerIndex((AstNode**) &identifier));
            Ast::addChildBack(parent, identifier);

            if (!(token.flags & TOKENPARSE_LAST_EOL) && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS) && token.id == TokenId::SymLeftParen)
            {
                // :SilentCall
                SWAG_CHECK(eatToken());
                identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
                identifier->inheritTokenLocation(token);
                identifier->token.text = "";
                identifier->specFlags |= AstIdentifier::SPECFLAG_SILENT_CALL;
                identifier->semanticFct = SemanticJob::resolveIdentifier;
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }
        }
    }

    return true;
}

bool Parser::doIdentifierRef(AstNode* parent, AstNode** result, uint32_t identifierFlags)
{
    auto identifierRef = Ast::newIdentifierRef(sourceFile, parent, this);
    *result            = identifierRef;

    if (identifierFlags & IDENTIFIER_GLOBAL)
        identifierRef->specFlags |= AstIdentifierRef::SPECFLAG_GLOBAL;

    switch (token.id)
    {
    case TokenId::CompilerLocation:
        SWAG_CHECK(doCompilerSpecialValue(identifierRef, &dummyResult));
        break;
    case TokenId::IntrinsicLocation:
        SWAG_CHECK(doIntrinsicLocation(identifierRef, &dummyResult));
        break;

    case TokenId::IntrinsicSpread:
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicAlignOf:
    case TokenId::IntrinsicOffsetOf:
    case TokenId::IntrinsicTypeOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicDeclType:
    case TokenId::IntrinsicCountOf:
    case TokenId::IntrinsicDataOf:
    case TokenId::IntrinsicStringOf:
    case TokenId::IntrinsicNameOf:
    case TokenId::IntrinsicRunes:
    case TokenId::IntrinsicMakeAny:
    case TokenId::IntrinsicMakeSlice:
    case TokenId::IntrinsicMakeString:
    case TokenId::IntrinsicMakeInterface:
    case TokenId::IntrinsicMakeCallback:
    case TokenId::IntrinsicIsConstExpr:
    case TokenId::IntrinsicCVaStart:
    case TokenId::IntrinsicCVaEnd:
    case TokenId::IntrinsicCVaArg:
        SWAG_CHECK(doIntrinsicProp(identifierRef, &dummyResult));
        break;

    case TokenId::NativeType:
        return invalidIdentifierError(token);

    default:
        if (Tokenizer::isKeyword(token.id))
            return invalidIdentifierError(token);

        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
        break;
    }

    while (token.id == TokenId::SymDot && !(token.flags & TOKENPARSE_LAST_EOL))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
    }

    return true;
}

bool Parser::doDiscard(AstNode* parent, AstNode** result)
{
    auto discardToken = token;
    SWAG_CHECK(eatToken());

    AstNode* idRef;
    switch (token.id)
    {
    case TokenId::Identifier:
        SWAG_CHECK(doIdentifierRef(parent, &idRef));
        break;
    case TokenId::KwdTry:
    case TokenId::KwdCatch:
    case TokenId::KwdTryCatch:
    case TokenId::KwdAssume:
        SWAG_CHECK(doTryCatchAssume(parent, &idRef, true));
        break;
    default:
        if (Tokenizer::isIntrinsicReturn(token.id))
        {
            Diagnostic diag{sourceFile, token, Fmt(Err(Err1179), token.ctext())};
            auto       note = Diagnostic::note(sourceFile, discardToken, Nte(Nte1019));
            return context->report(diag, note);
        }

        return error(token, Fmt(Err(Err1173), token.ctext()));
    }

    *result = idRef;

    // For export
    idRef->flags |= AST_DISCARD;

    // Mark the identifier with AST_DISCARD
    while (idRef && idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->childs.front();
    SWAG_ASSERT(idRef);

    // This is where AST_DISCARD will be really used
    for (auto c : idRef->childs)
    {
        if (c->kind != AstNodeKind::Identifier)
            break;
        c->flags |= AST_DISCARD;
    }

    return true;
}

bool Parser::doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard)
{
    AstNode* node = nullptr;
    if (token.id == TokenId::KwdTry)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Try, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveTry;
    }
    else if (token.id == TokenId::KwdCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Catch, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveCatch;
    }
    else if (token.id == TokenId::KwdTryCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::TryCatch, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveTryCatch;
    }
    else if (token.id == TokenId::KwdAssume)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Assume, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveAssume;
    }

    *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, Fmt(Err(Err1021), node->token.ctext())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);

    if (token.id == TokenId::SymLeftCurly)
    {
        node->specFlags |= AstTryCatchAssume::SPECFLAG_BLOCK;
        SWAG_VERIFY(!afterDiscard, error(token, Err(Err1207)));
        SWAG_CHECK(doCurlyStatement(node, &dummyResult));

        if (node->semanticFct == SemanticJob::resolveTry)
        {
            node->semanticFct = SemanticJob::resolveTryBlock;
        }
        else if (node->semanticFct == SemanticJob::resolveTryCatch)
        {
            node->ownerFct->specFlags |= AstFuncDecl::SPECFLAG_REG_GET_CONTEXT;
            node->semanticFct = nullptr;
        }
        else
            node->semanticFct = nullptr;
    }
    else
    {
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1019), node->token.ctext(), token.ctext())));
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Throw, sourceFile, parent);
    *result   = node;
    SWAG_VERIFY(node->ownerFct, error(node, Err(Err1028)));
    node->semanticFct = SemanticJob::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Err1147), token.ctext(), node->token.ctext())));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doTypeAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstAlias>(this, AstNodeKind::TypeAlias, sourceFile, parent);
    node->kwdLoc      = token;
    node->semanticFct = SemanticJob::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1071), node->token.ctext(), token.ctext())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased type"));

    AstNode* expr;
    SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &expr));
    SWAG_CHECK(eatSemiCol("'typealias' expression"));

    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
    node->semanticFct                      = SemanticJob::resolveTypeAlias;
    node->resolvedSymbolName               = currentScope->symTable.registerSymbolName(context, node, SymbolKind::TypeAlias);
    return true;
}

bool Parser::doNameAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstAlias>(this, AstNodeKind::NameAlias, sourceFile, parent);
    node->kwdLoc      = token;
    node->semanticFct = SemanticJob::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1071), node->token.ctext(), token.ctext())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased name"));
    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err1218), token.ctext())));

    AstNode* expr;
    SWAG_CHECK(doIdentifierRef(node, &expr, IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_ARRAY));
    SWAG_CHECK(eatSemiCol("'namealias' expression"));
    expr->childs.back()->flags |= AST_NAME_ALIAS;

    node->semanticFct        = SemanticJob::resolveNameAlias;
    node->resolvedSymbolName = currentScope->symTable.registerSymbolName(context, node, SymbolKind::NameAlias);
    return true;
}

bool Parser::doTopLevelIdentifier(AstNode* parent, AstNode** result)
{
    auto tokenIdentifier = token;
    eatToken();

    Diagnostic                diag{sourceFile, tokenIdentifier, Fmt(Err(Err0087), tokenIdentifier.ctext())};
    Vector<const Diagnostic*> notes;

    if (token.id == TokenId::Identifier)
    {
        if (tokenIdentifier.text == "function" || tokenIdentifier.text == "fn")
            notes.push_back(Diagnostic::note(Nte(Nte1026)));
    }

    if (token.id == TokenId::SymEqual || token.id == TokenId::SymColon)
    {
        notes.push_back(Diagnostic::note(Nte(Nte1120)));
    }

    Utf8 appendMsg = SemanticJob::findClosestMatchesMsg(tokenIdentifier.text, {});
    if (!appendMsg.empty())
        notes.push_back(Diagnostic::note(appendMsg));
    return context->report(diag, notes);
}