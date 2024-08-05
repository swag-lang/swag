#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

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
    if (!sourceFile->acceptsInternalStuff() && isGeneratedName(node->token.text))
        return false;

    return true;
}

bool Parser::checkIsValidUserName(AstNode* node, const Token* loc) const
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (!testIsValidUserName(node))
        return error(loc ? *loc : node->token, formErr(Err0615, node->token.c_str()));

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
    if (testIsSingleIdentifier(node))
        return true;

    if (node->is(AstNodeKind::IdentifierRef))
        return error(node, formErr(Err0245, msg));

    return error(node, formErr(Err0305, msg));
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
                return error(tokenParse, toErr(Err0139));
            if (tokenParse.literalType != LiteralType::TypeUntypedInt && tokenParse.literalType != LiteralType::TypeUnsigned8)
                return error(tokenParse, formErr(Err0141, tokenParse.token.c_str()));
            if (tokenParse.literalValue.u64 > 255)
                return error(tokenParse, formErr(Err0600, tokenParse.literalValue.u64));
            if (tokenParse.literalValue.u8 == 0)
                return error(tokenParse, formErr(Err0140, tokenParse.token.c_str()));

            scopeUpValue = tokenParse;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
        }
    }

    if (tokenParse.is(TokenId::LiteralNumber))
    {
        Diagnostic err{sourceFile, tokenParse, toErr(Err0306)};
        if (tokenParse.literalType == LiteralType::TypeUntypedInt)
        {
            if (parent && parent->is(AstNodeKind::IdentifierRef) && parent->lastChild() && parent->lastChild()->is(AstNodeKind::Identifier))
            {
                err.addNote(formNte(Nte0086, parent->lastChild()->token.c_str(), tokenParse.token.c_str()));
                if (tokenParse.literalValue.u64 < 32)
                    err.addNote(formNte(Nte0085, parent->lastChild()->token.c_str(), tokenParse.token.c_str()));
            }
        }

        return context->report(err);
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

    bool isIntrinsic = tokenParse.token.text[0] == '@';

    SWAG_CHECK(eatToken());
    SWAG_CHECK(checkIsValidUserName(identifier));

    if (isIntrinsic && tokenParse.isNot(TokenId::SymLeftParen))
    {
        Diagnostic err{identifier, formErr(Err0567, identifier->token.c_str())};
        err.addNote(sourceFile, tokenParse.token, formNte(Nte0059, tokenParse.token.c_str()));
        return context->report(err);
    }

    // Replace "Self" with the corresponding struct name
    if (identifier->token.is(g_LangSpec->name_Self))
    {
        SWAG_VERIFY(parent->ownerStructScope, context->report({identifier, toErr(Err0449)}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
        identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_SELF);
    }

    // Generic arguments
    if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE) && !identifierFlags.has(IDENTIFIER_NO_GEN_PARAMS))
    {
        if (tokenParse.is(TokenId::SymQuote))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);
        }
    }

    // Function call parameters
    if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS))
    {
        if (tokenParse.is(TokenId::SymLeftParen))
        {
            if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
            {
                Diagnostic err{identifier, tokenParse.token, toErr(Err0368)};
                return context->report(err);
            }

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!tokenParse.flags.has(TOKEN_PARSE_BLANK_BEFORE) && tokenParse.is(TokenId::SymLeftCurly))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
        }
    }

    // Array index
    if (tokenParse.is(TokenId::SymLeftSquare) && !identifierFlags.has(IDENTIFIER_NO_PARAMS))
    {
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

        if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS) && tokenParse.is(TokenId::SymLeftParen))
        {
            // :SilentCall
            SWAG_CHECK(eatToken());
            identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
            identifier->inheritTokenLocation(tokenParse.token);
            identifier->token.text = "";
            identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
            identifier->semanticFct = Semantic::resolveIdentifier;
            SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightParen));
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

    while (tokenParse.is(TokenId::SymDot) && !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
    }

    return true;
}

bool Parser::doDiscard(AstNode* parent, AstNode** result)
{
    auto discardToken = tokenParse;
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
                Diagnostic err{sourceFile, tokenParse, formErr(Err0744, tokenParse.token.c_str())};
                err.addNote(sourceFile, discardToken.token, toNte(Nte0160));
                err.addNote(toNte(Nte0012));
                return context->report(err);
            }

            return error(tokenParse, toErr(Err0153));
    }

    *result = idRef;

    // For export
    idRef->addAstFlag(AST_DISCARD);
    FormatAst::inheritFormatBefore(this, idRef, &discardToken);

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
    SWAG_VERIFY(node->ownerFct, error(node, formErr(Err0492, node->token.c_str())));
    SWAG_CHECK(eatToken());

    ParserPushTryCatchAssume sc(this, castAst<AstTryCatchAssume>(node));

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK);
        SWAG_VERIFY(!afterDiscard, error(tokenParse, toErr(Err0228)));
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
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
        SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0110, node->token.c_str())));
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Throw, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse, formErr(Err0491, tokenParse.token.c_str(), node->token.c_str())));

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

    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0184, node->token.c_str())));
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

    SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0184, node->token.c_str())));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased name"));
    SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0183)));

    AstNode* expr;
    SWAG_CHECK(doIdentifierRef(node, &expr, IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_ARRAY));

    SWAG_CHECK(eatSemiCol("[[namealias]] expression"));
    expr->lastChild()->addSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS);

    node->semanticFct = Semantic::resolveNameAlias;
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::NameAlias));
    return true;
}
