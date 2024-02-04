#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "SourceFile.h"

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

bool Parser::testIsValidUserName(const AstNode* node) const
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
        return error(loc ? *loc : node->token, Fmt(Err(Err0617), node->token.ctext()));

    // Pour toi frangouille, ajouté le jour de ton départ
    if (node->token.text.compareNoCase("jyb37"))
    {
        const Diagnostic diag{
            node,
            "I'm sorry, but 'JYB' is my little brother's nickname, and '37' is the age when he passed away at 1:29 AM on 11-26-2023. Please, please use another identifier. I love you, bro."};
        context->report(diag);
        return false;
    }

    return true;
}

bool Parser::checkIsSingleIdentifier(AstNode* node, const char* msg)
{
    if (!testIsSingleIdentifier(node))
        return error(node, Fmt(Err(Err0246), msg));

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
                return error(token, Fmt(Err(Err0145), token.ctext()));

            if (token.literalType != LiteralType::TT_UNTYPED_INT && token.literalType != LiteralType::TT_U8)
                return error(token, Fmt(Err(Err0147), token.ctext()));
            if (token.literalValue.u64 > 255)
                return error(token, Fmt(Err(Err0602), token.literalValue.u64));
            if (token.literalValue.u8 == 0)
                return error(token, Fmt(Err(Err0146), token.ctext()));

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
    identifier->semanticFct = Semantic::resolveIdentifier;

    if (scopeUpValue.id != TokenId::Invalid)
    {
        identifier->allocateIdentifierExtension();
        identifier->identifierExtension->scopeUpMode  = IdentifierScopeUpMode::Count;
        identifier->identifierExtension->scopeUpValue = scopeUpValue;
    }

    if (identifier->flags & AST_IN_FUNC_DECL_PARAMS)
        identifier->addSpecFlags(AstIdentifier::SPECFLAG_NO_INLINE);

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == g_LangSpec->name_Self)
    {
        SWAG_VERIFY(parent->ownerStructScope, context->report({identifier, Err(Err0454)}));
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
                Diagnostic diag{identifier, token, Err(Err0377)};
                return context->report(diag);
            }

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!(token.flags & TOKENPARSE_LAST_BLANK) && token.id == TokenId::SymLeftCurly)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->addSpecFlags(AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT);
        }
    }

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (!(identifierFlags & IDENTIFIER_NO_PARAMS))
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
                return context->report({identifier, token, Err(Err0482)});

            uint16_t serial = 0;
            while (true)
            {
                Ast::removeFromParent(identifier);
                SWAG_CHECK(doArrayPointerIndex((AstNode**) &identifier));
                Ast::addChildBack(parent, identifier);
                if (identifier->kind != AstNodeKind::ArrayPointerIndex)
                    break;

                identifier->addSpecFlags(serial);
                if (token.id != TokenId::SymLeftSquare)
                    break;
                serial ^= AstArrayPointerIndex::SPECFLAG_SERIAL;
            }

            if (!(token.flags & TOKENPARSE_LAST_EOL) && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS) && token.id == TokenId::SymLeftParen)
            {
                // :SilentCall
                SWAG_CHECK(eatToken());
                identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
                identifier->inheritTokenLocation(token);
                identifier->token.text = "";
                identifier->addSpecFlags(AstIdentifier::SPECFLAG_SILENT_CALL);
                identifier->semanticFct = Semantic::resolveIdentifier;
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }
        }
    }

    return true;
}

bool Parser::doIdentifierRef(AstNode* parent, AstNode** result, uint32_t identifierFlags)
{
    const auto identifierRef = Ast::newIdentifierRef(sourceFile, parent, this);
    *result                  = identifierRef;

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
    const auto discardToken = token;
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
            const Diagnostic diag{sourceFile, token, Fmt(Err(Err0748), token.ctext())};
            const auto       note  = Diagnostic::note(sourceFile, discardToken, Nte(Nte0149));
            const auto       note1 = Diagnostic::note(Nte(Nte0012));
            return context->report(diag, note, note1);
        }

        return error(token, Fmt(Err(Err0159), token.ctext()));
    }

    *result = idRef;

    // For export
    idRef->flags |= AST_DISCARD;

    // Mark the identifier with AST_DISCARD
    while (idRef && idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->childs.front();
    SWAG_ASSERT(idRef);

    // This is where AST_DISCARD will be really used
    for (const auto c : idRef->childs)
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
        node->semanticFct = Semantic::resolveTry;
    }
    else if (token.id == TokenId::KwdCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Catch, sourceFile, parent);
        node->semanticFct = Semantic::resolveCatch;
    }
    else if (token.id == TokenId::KwdTryCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::TryCatch, sourceFile, parent);
        node->semanticFct = Semantic::resolveTryCatch;
    }
    else if (token.id == TokenId::KwdAssume)
    {
        node              = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Assume, sourceFile, parent);
        node->semanticFct = Semantic::resolveAssume;
    }

    *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, Fmt(Err(Err0501), node->token.ctext())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);

    if (token.id == TokenId::SymLeftCurly)
    {
        node->addSpecFlags(AstTryCatchAssume::SPECFLAG_BLOCK);
        SWAG_VERIFY(!afterDiscard, error(token, Err(Err0231)));
        SWAG_CHECK(doCurlyStatement(node, &dummyResult));

        if (node->semanticFct == Semantic::resolveTry)
        {
            node->semanticFct = Semantic::resolveTryBlock;
        }
        else if (node->semanticFct == Semantic::resolveTryCatch || node->semanticFct == Semantic::resolveAssume)
        {
            node->ownerFct->addSpecFlags(AstFuncDecl::SPECFLAG_REG_GET_CONTEXT);
            node->semanticFct = nullptr;
        }
        else
            node->semanticFct = nullptr;
    }
    else
    {
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
        SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0116), node->token.ctext(), token.ctext())));
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Throw, sourceFile, parent);
    *result         = node;
    SWAG_VERIFY(node->ownerFct, error(node, Err(Err0471)));
    node->semanticFct = Semantic::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Err0500), token.ctext(), node->token.ctext())));

    if (token.id == TokenId::IntrinsicGetErr)
    {
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
        node->addSpecFlags(AstTryCatchAssume::SPECFLAG_THROW_GETERR);
    }
    else
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doTypeAlias(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstAlias>(this, AstNodeKind::TypeAlias, sourceFile, parent);
    node->kwdLoc      = token;
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0189), node->token.ctext(), token.ctext())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased type"));

    AstNode* expr;
    SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &expr));
    SWAG_CHECK(eatSemiCol("[[typealias]] expression"));

    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveTypeAliasBefore;
    node->semanticFct                      = Semantic::resolveTypeAlias;
    node->resolvedSymbolName               = currentScope->symTable.registerSymbolName(context, node, SymbolKind::TypeAlias);
    return true;
}

bool Parser::doNameAlias(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstAlias>(this, AstNodeKind::NameAlias, sourceFile, parent);
    node->kwdLoc      = token;
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0189), node->token.ctext(), token.ctext())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased name"));
    SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0188), token.ctext())));

    AstNode* expr;
    SWAG_CHECK(doIdentifierRef(node, &expr, IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_ARRAY));
    SWAG_CHECK(eatSemiCol("[[namealias]] expression"));
    expr->childs.back()->addSpecFlags(AstIdentifier::SPECFLAG_NAME_ALIAS);

    node->semanticFct        = Semantic::resolveNameAlias;
    node->resolvedSymbolName = currentScope->symTable.registerSymbolName(context, node, SymbolKind::NameAlias);
    return true;
}

bool Parser::doTopLevelIdentifier(AstNode* parent, AstNode** result)
{
    const auto tokenIdentifier = token;
    eatToken();

    const Diagnostic          diag{sourceFile, tokenIdentifier, Fmt(Err(Err0689), tokenIdentifier.ctext())};
    Vector<const Diagnostic*> notes;

    if (token.id == TokenId::Identifier)
    {
        if (tokenIdentifier.text == "function" || tokenIdentifier.text == "fn" || tokenIdentifier.text == "def")
            notes.push_back(Diagnostic::note(Nte(Nte0040)));
    }

    if (token.id == TokenId::SymEqual || token.id == TokenId::SymColon)
    {
        notes.push_back(Diagnostic::note(Nte(Nte0053)));
    }

    const Utf8 appendMsg = SemanticError::findClosestMatchesMsg(tokenIdentifier.text, {}, IdentifierSearchFor::TopLevelInstruction);
    if (!appendMsg.empty())
        notes.push_back(Diagnostic::note(appendMsg));
    return context->report(diag, notes);
}
