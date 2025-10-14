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

bool Parser::isItemName(const Utf8& name)
{
    if (name.length() <= 4)
        return false;
    if (name[0] != 'i' || name[1] != 't' || name[2] != 'e' || name[3] != 'm')
        return false;

    for (uint32_t idx = 4; idx < name.length(); idx++)
    {
        if (!isdigit(name[idx]))
            return false;
    }

    return true;
}

bool Parser::isGeneratedName(const Utf8& name)
{
    return name.length() >= 2 && name[0] == '_' && name[1] == '_';
}

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

    // An identifier that starts with '__' is reserved for internal usage!
    if (!sourceFile->acceptsInternalStuff() && isGeneratedName(node->token.text))
        return false;

    return true;
}

bool Parser::checkIsValidUserName(AstNode* node, const Token* loc) const
{
    // An identifier that starts with '__' is reserved for internal usage!
    if (node->token.text == "_")
        return error(loc ? *loc : node->token, toErr(Err0512));
    if (!testIsValidUserName(node))
        return error(loc ? *loc : node->token, formErr(Err0513, node->token.cstr()));

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
        return error(node, formErr(Err0172, msg));

    return error(node, formErr(Err0198, msg));
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
                return error(tokenParse, toErr(Err0611));
            if (tokenParse.literalType != LiteralType::TypeUntypedInt && tokenParse.literalType != LiteralType::TypeUnsigned8)
                return error(tokenParse, formErr(Err0124, tokenParse.cstr()));
            if (tokenParse.literalValue.u64 > 255)
                return error(tokenParse, formErr(Err0496, tokenParse.literalValue.u64));
            if (tokenParse.literalValue.u8 == 0)
                return error(tokenParse, formErr(Err0123, tokenParse.cstr()));

            scopeUpValue = tokenParse;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
        }
    }

    if (tokenParse.is(TokenId::LiteralNumber))
    {
        Diagnostic err{sourceFile, tokenParse, toErr(Err0200)};
        if (tokenParse.literalType == LiteralType::TypeUntypedInt)
        {
            if (parent && parent->is(AstNodeKind::IdentifierRef) && parent->lastChild() && parent->lastChild()->is(AstNodeKind::Identifier))
            {
                err.addNote(form("hint: use square brackets [[%s[%s]]] to access an element by index", parent->lastChild()->token.cstr(), tokenParse.cstr()));
                if (tokenParse.literalValue.u64 < 32)
                    err.addNote(form("hint: use [[%s.item%s]] to access a struct or a tuple element", parent->lastChild()->token.cstr(), tokenParse.cstr()));
            }
        }

        return context->report(err);
    }

    if (tokenParse.isNot(TokenId::Identifier) &&
        tokenParse.isNot(TokenId::NativeType) &&
        tokenParse.isNot(TokenId::SymQuestion) &&
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

    if (isIntrinsic)
    {
        if (g_LangSpec->intrinsicConstants.contains(identifier->token.text))
        {
            if (tokenParse.is(TokenId::SymLeftParen))
            {
                const Diagnostic err{identifier, tokenParse.token, formErr(Err0786, identifier->token.cstr(), identifier->token.cstr())};
                return context->report(err);
            }

            ParserPushFreezeFormat ff(this);
            identifier->callParameters = Ast::newFuncCallParams(this, identifier);
            identifier->callParameters->addAstFlag(AST_GENERATED);
            return true;
        }

        if (tokenParse.isNot(TokenId::SymLeftParen))
        {
            const Diagnostic err{identifier, formErr(Err0439, "intrinsic", identifier->token.cstr())};
            return context->report(err);
        }
    }

    // Generic arguments
    if (!identifierFlags.has(IDENTIFIER_NO_GEN_PARAMS))
    {
        if (Tokenizer::isJustAfterPrevious(tokenParse))
        {
            if (tokenParse.is(TokenId::SymQuote))
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doGenericFuncCallArguments(identifier, &identifier->genericParameters));
                identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);
            }
        }
    }

    if (!identifierFlags.has(IDENTIFIER_NO_CALL_PARAMS))
    {
        if (Tokenizer::isJustAfterPrevious(tokenParse))
        {
            // Function call parameters
            if (tokenParse.is(TokenId::SymLeftParen))
            {
                if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
                {
                    Diagnostic err{identifier, tokenParse.token, toErr(Err0237)};
                    return context->report(err);
                }

                SWAG_CHECK(eatToken());
                SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }

            // Struct initialization values
            else if (tokenParse.is(TokenId::SymLeftCurly))
            {
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightCurly));
                identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
            }
        }
    }

    // Array index
    if (!identifierFlags.has(IDENTIFIER_NO_PARAMS))
    {
        if (Tokenizer::isJustAfterPrevious(tokenParse))
        {
            if (tokenParse.is(TokenId::SymLeftSquare))
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

                if (Tokenizer::isJustAfterPrevious(tokenParse))
                {
                    if (tokenParse.is(TokenId::SymLeftParen))
                    {
                        // @SilentCall
                        SWAG_CHECK(eatToken());
                        identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
                        identifier->inheritTokenLocation(tokenParse.token);
                        identifier->token.text = "";
                        identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
                        identifier->semanticFct = Semantic::resolveIdentifier;
                        SWAG_CHECK(doFuncCallArguments(identifier, &identifier->callParameters, TokenId::SymRightParen));
                    }
                }
            }
        }
    }

    return true;
}

bool Parser::doTopLevelIdentifierRef(AstNode* parent)
{
    AstNode* identifierRef;
    SWAG_CHECK(doIdentifierRef(parent, &identifierRef, IDENTIFIER_TOP_LEVEL));

    const auto lastChild = identifierRef->lastChild();
    if (lastChild->isNot(AstNodeKind::Identifier))
    {
        const Diagnostic err{lastChild, lastChild->token, formErr(Err0645, lastChild->token.cstr())};
        return context->report(err);
    }

    const auto identifier = castAst<AstIdentifier>(lastChild, AstNodeKind::Identifier);
    identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_TOP_LEVEL);

    if (!identifier->callParameters)
    {
        const Diagnostic err{lastChild, lastChild->token, formErr(Err0645, lastChild->token.cstr())};
        return context->report(err);
    }

    identifier->semanticFct = Semantic::resolveTopLevelIdentifier;
    identifier->byteCodeFct = nullptr;

    return true;
}

bool Parser::doDottedIdentifierRef(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags)
{
    if (tokenParse.is(TokenId::SymDot))
    {
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::Identifier))
            return error(tokenParse, formErr(Err0199, ".").cstr());
        AstNode* idref;
        SWAG_CHECK(doIdentifierRef(parent, &idref, identifierFlags));
        *result = idref;
        SWAG_ASSERT(idref->is(AstNodeKind::IdentifierRef));
        idref->addSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE);
    }
    else
    {
        SWAG_CHECK(doIdentifierRef(parent, result, identifierFlags));
    }

    return true;
}

bool Parser::doIdentifierRef(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags)
{
    const auto identifierRef = Ast::newIdentifierRef(this, parent);
    *result                  = identifierRef;

    switch (tokenParse.token.id)
    {
        case TokenId::CompilerCurLocation:
            SWAG_CHECK(doCompilerSpecialValue(identifierRef, &dummyResult));
            break;
        case TokenId::CompilerLocation:
            SWAG_CHECK(doCompilerLocation(identifierRef, &dummyResult));
            break;

        case TokenId::CompilerSizeOf:
        case TokenId::CompilerAlignOf:
        case TokenId::CompilerOffsetOf:
        case TokenId::CompilerTypeOf:
        case TokenId::CompilerDeclType:
        case TokenId::CompilerStringOf:
        case TokenId::CompilerNameOf:
        case TokenId::CompilerRunes:
        case TokenId::CompilerIsConstExpr:
            SWAG_CHECK(doIntrinsicProp(identifierRef, &dummyResult));
            break;
        case TokenId::IntrinsicKindOf:
        case TokenId::IntrinsicCountOf:
        case TokenId::IntrinsicDataOf:
        case TokenId::IntrinsicMakeAny:
        case TokenId::IntrinsicMakeSlice:
        case TokenId::IntrinsicMakeString:
        case TokenId::IntrinsicMakeInterface:
        case TokenId::IntrinsicMakeCallback:
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

    while (tokenParse.is(TokenId::SymDot) && !Tokenizer::isStartOfNewStatement(tokenParse))
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
        case TokenId::SymDot:
            SWAG_CHECK(doDottedIdentifierRef(parent, &idRef));
            break;
        case TokenId::Identifier:
            SWAG_CHECK(doIdentifierRef(parent, &idRef));
            break;
        case TokenId::KwdTry:
        case TokenId::KwdCatch:
        case TokenId::KwdTryCatch:
        case TokenId::KwdAssume:
            SWAG_CHECK(doTryCatchAssume(parent, &idRef, true));
            break;
        case TokenId::CompilerInject:
            SWAG_CHECK(doCompilerInject(parent, &idRef));
            break;
        default:
            if (Tokenizer::isIntrinsicReturn(tokenParse.token.id))
            {
                Diagnostic err{sourceFile, tokenParse, formErr(Err0712, tokenParse.cstr())};
                err.addNote(sourceFile, discardToken.token, "this [[discard]] should be removed");
                err.addNote("[[discard]] cannot be used with an intrinsic, as an intrinsic result should always be utilized");
                return context->report(err);
            }

            return error(tokenParse, toErr(Err0613));
    }

    *result = idRef;

    // For export
    idRef->addAstFlag(AST_DISCARD);
    FormatAst::inheritFormatBefore(this, idRef, &discardToken);

    Ast::setDiscard(idRef);
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
    SWAG_VERIFY(node->ownerFct, error(node, formErr(Err0395, node->token.cstr())));
    SWAG_CHECK(eatToken());

    ParserPushTryCatchAssume sc(this, castAst<AstTryCatchAssume>(node));

    if (tokenParse.is(TokenId::SymLeftCurly))
    {
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK);
        SWAG_VERIFY(!afterDiscard, error(tokenParse, toErr(Err0381)));
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
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
        SWAG_CHECK(doDottedIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Throw, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(node->ownerFct, error(node, formErr(Err0395, node->token.cstr())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdTry), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdCatch), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdAssume), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));
    SWAG_VERIFY(tokenParse.isNot(TokenId::KwdThrow), error(tokenParse, formErr(Err0394, tokenParse.cstr(), node->token.cstr())));

    if (tokenParse.is(TokenId::IntrinsicGetErr))
    {
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_THROW_GET_ERR);
    }
    else
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doAlias(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstAlias>(AstNodeKind::TypeAlias, this, parent);
    node->kwdLoc    = tokenParse.token;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0619)));
    node->inheritTokenName(tokenParse.token);
    node->inheritTokenLocation(tokenParse.token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased value"));

    AstNode* expr;
    if (tokenParse.is(TokenId::CompilerDeclType))
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_ALIAS, &expr));
    else
        SWAG_CHECK(doSinglePrimaryExpression(node, EXPR_FLAG_ALIAS, &expr));

    if (expr->isNot(AstNodeKind::IdentifierRef) &&
        expr->isNot(AstNodeKind::TypeLambda) &&
        expr->isNot(AstNodeKind::TypeClosure) &&
        expr->isNot(AstNodeKind::TypeExpression))
    {
        const Diagnostic err{expr, toErr(Err0618)};
        return context->report(err);
    }

    SWAG_CHECK(eatSemiCol("[[alias]] declaration"));

    expr->allocateExtension(ExtensionKind::Semantic);
    expr->extSemantic()->semanticAfterFct = Semantic::resolveAliasAfterValue;

    node->semanticFct = Semantic::resolveAlias;
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::Alias));
    return true;
}
