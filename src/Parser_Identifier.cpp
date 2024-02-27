#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Parser.h"
#include "Scoped.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "SourceFile.h"

bool Parser::testIsSingleIdentifier(const AstNode* node)
{
    if (node->isNot(AstNodeKind::IdentifierRef) ||
        node->childCount() > 1 ||
        node->lastChild()->isNot(AstNodeKind::Identifier))
    {
        return false;
    }

    return true;
}

bool Parser::testIsValidUserName(const AstNode* node) const
{
    if (node->parent && node->parent->hasAstFlag(AST_GENERATED))
        return true;
    if (node->hasAstFlag(AST_GENERATED))
        return true;

    // An identifier that starts with '__' is reserved for internal usage !
    if (!sourceFile->hasFlag(FILE_IS_GENERATED) &&
        !sourceFile->hasFlag(FILE_IS_BOOTSTRAP_FILE) &&
        !sourceFile->hasFlag(FILE_IS_RUNTIME_FILE))
    {
        if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
            return false;
    }

    return true;
}

bool Parser::checkIsValidUserName(AstNode* node, const Token* loc) const
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (!testIsValidUserName(node))
        return error(loc ? *loc : node->token, formErr(Err0617, node->token.c_str()));

    // Pour toi frangouille, ajouté le jour de ton départ
    if (node->token.text.compareNoCase("jyb37"))
    {
        const Diagnostic err{node, "I'm sorry, but 'JYB' is my little brother's nickname, and '37' is the age when he passed away at 1:29 AM on 11-26-2023. Please, please use another identifier. I love you, bro."};
        context->report(err);
        return false;
    }

    return true;
}

bool Parser::checkIsSingleIdentifier(AstNode* node, const char* msg) const
{
    if (!testIsSingleIdentifier(node))
        return error(node, formErr(Err0246, msg));
    return true;
}

bool Parser::checkIsIdentifier(const TokenParse& myToken, const char* msg) const
{
    if (myToken.token.is(TokenId::Identifier))
        return true;
    return invalidIdentifierError(myToken, msg);
}

bool Parser::doIdentifier(AstNode* parent, IdentifierFlags identifierFlags)
{
    TokenParse scopeUpValue;

    // #up to change the scope
    if (tokenParse.is(TokenId::CompilerUp))
    {
        auto upToken = tokenParse;
        SWAG_CHECK(eatToken());
        auto startLoc                  = tokenParse.token.startLocation;
        tokenParse.token.startLocation = upToken.token.startLocation;

        if (tokenParse.is(TokenId::SymQuestion))
            return invalidIdentifierError(tokenParse);

        scopeUpValue.token.id            = TokenId::CompilerUp;
        scopeUpValue.literalType         = LiteralType::TypeUntypedInt;
        scopeUpValue.literalValue.u64    = 1;
        scopeUpValue.token.startLocation = upToken.token.startLocation;
        scopeUpValue.token.endLocation   = upToken.token.endLocation;

        if (tokenParse.is(TokenId::SymLeftParen))
        {
            SWAG_CHECK(eatToken());

            if (tokenParse.isNot(TokenId::LiteralNumber))
                return error(tokenParse.token, formErr(Err0145, tokenParse.token.c_str()));
            if (tokenParse.literalType != LiteralType::TypeUntypedInt && tokenParse.literalType != LiteralType::TypeUnsigned8)
                return error(tokenParse.token, formErr(Err0147, tokenParse.token.c_str()));
            if (tokenParse.literalValue.u64 > 255)
                return error(tokenParse.token, formErr(Err0602, tokenParse.literalValue.u64));
            if (tokenParse.literalValue.u8 == 0)
                return error(tokenParse.token, formErr(Err0146, tokenParse.token.c_str()));

            scopeUpValue = tokenParse;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
        }
    }

    if (tokenParse.isNot(TokenId::Identifier) &&
        tokenParse.isNot(TokenId::NativeType) &&
        tokenParse.isNot(TokenId::SymQuestion) &&
        tokenParse.isNot(TokenId::CompilerSelf) &&
        !Tokenizer::isIntrinsicReturn(tokenParse.token.id) &&
        !Tokenizer::isIntrinsicNoReturn(tokenParse.token.id))
    {
        return invalidIdentifierError(tokenParse);
    }

    auto identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
    identifier->inheritTokenLocation(tokenParse.token);
    identifier->semanticFct = Semantic::resolveIdentifier;

    if (scopeUpValue.token.isNot(TokenId::Invalid))
    {
        identifier->allocateIdentifierExtension();
        identifier->identifierExtension->scopeUpMode  = IdentifierScopeUpMode::Count;
        identifier->identifierExtension->scopeUpValue = scopeUpValue;
    }

    if (identifier->hasAstFlag(AST_IN_FUNC_DECL_PARAMS))
        identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_NO_INLINE);

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == g_LangSpec->name_Self)
    {
        SWAG_VERIFY(parent->ownerStructScope, context->report({identifier, toErr(Err0454)}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
    }

    // Generic arguments
    if (!tokenParse.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_GEN_PARAMS))
    {
        if (tokenParse.is(TokenId::SymQuote))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);
        }
    }

    // Function call parameters
    if (!tokenParse.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS))
    {
        if (tokenParse.is(TokenId::SymLeftParen))
        {
            if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
            {
                Diagnostic err{identifier, tokenParse.token, toErr(Err0377)};
                return context->report(err);
            }

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!tokenParse.flags.has(TOKEN_PARSE_LAST_BLANK) && tokenParse.is(TokenId::SymLeftCurly))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
        }
    }

    // Array index
    if (tokenParse.is(TokenId::SymLeftSquare) && !identifierFlags.has(IDENTIFIER_NO_PARAMS))
    {
        if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
            return context->report({identifier, tokenParse.token, toErr(Err0482)});

        SpecFlags serial = 0;
        while (true)
        {
            Ast::removeFromParent(identifier);
            SWAG_CHECK(doArrayPointerIndex(reinterpret_cast<AstNode**>(&identifier)));
            Ast::addChildBack(parent, identifier);
            if (identifier->isNot(AstNodeKind::ArrayPointerIndex))
                break;

            identifier->addSpecFlag(serial);
            if (tokenParse.isNot(TokenId::SymLeftSquare))
                break;
            if (serial.has(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
                serial.remove(AstArrayPointerIndex::SPEC_FLAG_SERIAL);
            else
                serial.add(AstArrayPointerIndex::SPEC_FLAG_SERIAL);
        }

        if (!tokenParse.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS) && tokenParse.is(TokenId::SymLeftParen))
        {
            // :SilentCall
            SWAG_CHECK(eatToken());
            identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
            identifier->inheritTokenLocation(tokenParse.token);
            identifier->token.text = "";
            identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
            identifier->semanticFct = Semantic::resolveIdentifier;
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
    }

    return true;
}

bool Parser::doIdentifierRef(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags)
{
    const auto identifierRef = Ast::newIdentifierRef(this, parent);
    *result                  = identifierRef;

    switch (tokenParse.token.id)
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
            return invalidIdentifierError(tokenParse);

        default:
            if (Tokenizer::isKeyword(tokenParse.token.id))
                return invalidIdentifierError(tokenParse);
            SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
            break;
    }

    while (tokenParse.is(TokenId::SymDot) && !tokenParse.flags.has(TOKEN_PARSE_LAST_EOL))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
    }

    return true;
}

bool Parser::doDiscard(AstNode* parent, AstNode** result)
{
    const auto discardToken = tokenParse;
    SWAG_CHECK(eatToken());

    AstNode* idRef;
    switch (tokenParse.token.id)
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
            if (Tokenizer::isIntrinsicReturn(tokenParse.token.id))
            {
                Diagnostic err{sourceFile, tokenParse.token, formErr(Err0748, tokenParse.token.c_str())};
                err.addNote(sourceFile, discardToken.token, toNte(Nte0149));
                err.addNote(toNte(Nte0012));
                return context->report(err);
            }

            return error(tokenParse.token, formErr(Err0159, tokenParse.token.c_str()));
    }

    *result = idRef;

    // For export
    idRef->addAstFlag(AST_DISCARD);

    // Mark the identifier with AST_DISCARD
    while (idRef && idRef->isNot(AstNodeKind::IdentifierRef))
        idRef = idRef->firstChild();
    SWAG_ASSERT(idRef);

    // This is where AST_DISCARD will be really used
    for (const auto c : idRef->children)
    {
        if (c->isNot(AstNodeKind::Identifier))
            break;
        c->addAstFlag(AST_DISCARD);
    }

    return true;
}

bool Parser::doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard)
{
    AstNode* node = nullptr;
    if (tokenParse.is(TokenId::KwdTry))
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Try, this, parent);
        node->semanticFct = Semantic::resolveTry;
    }
    else if (tokenParse.is(TokenId::KwdCatch))
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Catch, this, parent);
        node->semanticFct = Semantic::resolveCatch;
    }
    else if (tokenParse.is(TokenId::KwdTryCatch))
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::TryCatch, this, parent);
        node->semanticFct = Semantic::resolveTryCatch;
    }
    else if (tokenParse.is(TokenId::KwdAssume))
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Assume, this, parent);
        node->semanticFct = Semantic::resolveAssume;
    }

    *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, formErr(Err0501, node->token.c_str())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, castAst<AstTryCatchAssume>(node));

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK);
        SWAG_VERIFY(!afterDiscard, error(tokenParse.token, toErr(Err0231)));
        SWAG_CHECK(doCurlyStatement(node, &dummyResult));

        if (node->semanticFct == Semantic::resolveTry)
        {
            node->semanticFct = Semantic::resolveTryBlock;
        }
        else if (node->semanticFct == Semantic::resolveTryCatch || node->semanticFct == Semantic::resolveAssume)
        {
            node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);
            node->semanticFct = nullptr;
        }
        else
            node->semanticFct = nullptr;
    }
    else
    {
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0116, node->token.c_str(), tokenParse.token.c_str())));
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Throw, this, parent);
    *result         = node;
    SWAG_VERIFY(node->ownerFct, error(node, toErr(Err0471)));
    node->semanticFct = Semantic::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse.token, formErr(Err0500, tokenParse.token.c_str(), node->token.c_str())));

    if (tokenParse.is(TokenId::IntrinsicGetErr))
    {
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_THROW_GET_ERR);
    }
    else
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doTypeAlias(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstAlias>(AstNodeKind::TypeAlias, this, parent);
    node->kwdLoc      = tokenParse.token;
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0189, node->token.c_str(), tokenParse.token.c_str())));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased type"));

    AstNode* expr;
    SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &expr));
    SWAG_CHECK(eatSemiCol("[[typealias]] expression"));

    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::resolveTypeAliasBefore;
    node->semanticFct                      = Semantic::resolveTypeAlias;
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::TypeAlias));
    return true;
}

bool Parser::doNameAlias(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstAlias>(AstNodeKind::NameAlias, this, parent);
    node->kwdLoc      = tokenParse.token;
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0189, node->token.c_str(), tokenParse.token.c_str())));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased name"));
    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0188, tokenParse.token.c_str())));

    AstNode* expr;
    SWAG_CHECK(doIdentifierRef(node, &expr, IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_ARRAY));
    SWAG_CHECK(eatSemiCol("[[namealias]] expression"));
    expr->lastChild()->addSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS);

    node->semanticFct = Semantic::resolveNameAlias;
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::NameAlias));
    return true;
}

bool Parser::errorTopLevelIdentifier()
{
    const auto tokenIdentifier = tokenParse.token;
    eatToken();

    Diagnostic err{sourceFile, tokenIdentifier, formErr(Err0689, tokenIdentifier.c_str())};

    if (tokenParse.is(TokenId::Identifier))
    {
        if (tokenIdentifier.text == "function" || tokenIdentifier.text == "fn" || tokenIdentifier.text == "def")
            err.addNote(toNte(Nte0040));
    }
    else if (tokenParse.is(TokenId::SymEqual) || tokenParse.is(TokenId::SymColon))
    {
        err.addNote(toNte(Nte0053));
    }

    if (!err.hasNotes())
    {
        const Utf8 appendMsg = SemanticError::findClosestMatchesMsg(tokenIdentifier.text, {}, IdentifierSearchFor::TopLevelInstruction);
        if (!appendMsg.empty())
            err.addNote(appendMsg);
    }

    return context->report(err);
}
