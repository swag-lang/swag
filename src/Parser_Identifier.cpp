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

bool Parser::testIsSingleIdentifier(AstNode* node)
{
    if (node->kind != AstNodeKind::IdentifierRef ||
        node->children.size() > 1 ||
        node->children.back()->kind != AstNodeKind::Identifier)
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
        return error(loc ? *loc : node->token, FMT(Err(Err0617), node->token.c_str()));

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
        return error(node, FMT(Err(Err0246), msg));
    return true;
}

bool Parser::checkIsIdentifier(const TokenParse& tokenParse, const char* msg) const
{
    if (tokenParse.id == TokenId::Identifier)
        return true;
    return invalidIdentifierError(tokenParse, msg);
}

bool Parser::doIdentifier(AstNode* parent, IdentifierFlags identifierFlags)
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
        scopeUpValue.literalType      = LiteralType::TypeUntypedInt;
        scopeUpValue.literalValue.u64 = 1;
        scopeUpValue.startLocation    = upToken.startLocation;
        scopeUpValue.endLocation      = upToken.endLocation;

        if (token.id == TokenId::SymLeftParen)
        {
            SWAG_CHECK(eatToken());

            if (token.id != TokenId::LiteralNumber)
                return error(token, FMT(Err(Err0145), token.c_str()));
            if (token.literalType != LiteralType::TypeUntypedInt && token.literalType != LiteralType::TypeUnsigned8)
                return error(token, FMT(Err(Err0147), token.c_str()));
            if (token.literalValue.u64 > 255)
                return error(token, FMT(Err(Err0602), token.literalValue.u64));
            if (token.literalValue.u8 == 0)
                return error(token, FMT(Err(Err0146), token.c_str()));

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

    auto identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
    identifier->inheritTokenLocation(token);
    identifier->semanticFct = Semantic::resolveIdentifier;

    if (scopeUpValue.id != TokenId::Invalid)
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
        SWAG_VERIFY(parent->ownerStructScope, context->report({identifier, Err(Err0454)}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
    }

    if (!token.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_GEN_PARAMS))
    {
        // Generic arguments
        if (token.id == TokenId::SymQuote)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doGenericFuncCallParameters(identifier, &identifier->genericParameters));
            identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);
        }
    }

    // Function call parameters
    if (!token.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS))
    {
        if (token.id == TokenId::SymLeftParen)
        {
            if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
            {
                Diagnostic err{identifier, token, Err(Err0377)};
                return context->report(err);
            }

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!token.flags.has(TOKEN_PARSE_LAST_BLANK) && token.id == TokenId::SymLeftCurly)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
        }
    }

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (!identifierFlags.has(IDENTIFIER_NO_PARAMS))
        {
            if (identifierFlags.has(IDENTIFIER_TYPE_DECL))
                return context->report({identifier, token, Err(Err0482)});

            SpecFlags serial = 0;
            while (true)
            {
                Ast::removeFromParent(identifier);
                SWAG_CHECK(doArrayPointerIndex(reinterpret_cast<AstNode**>(&identifier)));
                Ast::addChildBack(parent, identifier);
                if (identifier->kind != AstNodeKind::ArrayPointerIndex)
                    break;

                identifier->addSpecFlag(serial);
                if (token.id != TokenId::SymLeftSquare)
                    break;
                if (serial.has(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
                    serial.remove(AstArrayPointerIndex::SPEC_FLAG_SERIAL);
                else
                    serial.add(AstArrayPointerIndex::SPEC_FLAG_SERIAL);
            }

            if (!token.flags.has(TOKEN_PARSE_LAST_EOL) && !identifierFlags.has(IDENTIFIER_NO_FCT_PARAMS) && token.id == TokenId::SymLeftParen)
            {
                // :SilentCall
                SWAG_CHECK(eatToken());
                identifier = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, parent);
                identifier->inheritTokenLocation(token);
                identifier->token.text = "";
                identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
                identifier->semanticFct = Semantic::resolveIdentifier;
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }
        }
    }

    return true;
}

bool Parser::doIdentifierRef(AstNode* parent, AstNode** result, IdentifierFlags identifierFlags)
{
    const auto identifierRef = Ast::newIdentifierRef(this, parent);
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

    while (token.id == TokenId::SymDot && !token.flags.has(TOKEN_PARSE_LAST_EOL))
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
                const Diagnostic err{sourceFile, token, FMT(Err(Err0748), token.c_str())};
                const auto       note  = Diagnostic::note(sourceFile, discardToken, Nte(Nte0149));
                const auto       note1 = Diagnostic::note(Nte(Nte0012));
                return context->report(err, note, note1);
            }

            return error(token, FMT(Err(Err0159), token.c_str()));
    }

    *result = idRef;

    // For export
    idRef->addAstFlag(AST_DISCARD);

    // Mark the identifier with AST_DISCARD
    while (idRef && idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->children.front();
    SWAG_ASSERT(idRef);

    // This is where AST_DISCARD will be really used
    for (const auto c : idRef->children)
    {
        if (c->kind != AstNodeKind::Identifier)
            break;
        c->addAstFlag(AST_DISCARD);
    }

    return true;
}

bool Parser::doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard)
{
    AstNode* node = nullptr;
    if (token.id == TokenId::KwdTry)
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Try, this, parent);
        node->semanticFct = Semantic::resolveTry;
    }
    else if (token.id == TokenId::KwdCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Catch, this, parent);
        node->semanticFct = Semantic::resolveCatch;
    }
    else if (token.id == TokenId::KwdTryCatch)
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::TryCatch, this, parent);
        node->semanticFct = Semantic::resolveTryCatch;
    }
    else if (token.id == TokenId::KwdAssume)
    {
        node              = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Assume, this, parent);
        node->semanticFct = Semantic::resolveAssume;
    }

    *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, FMT(Err(Err0501), node->token.c_str())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, castAst<AstTryCatchAssume>(node));

    if (token.id == TokenId::SymLeftCurly)
    {
        node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK);
        SWAG_VERIFY(!afterDiscard, error(token, Err(Err0231)));
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
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
        SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0116), node->token.c_str(), token.c_str())));
        SWAG_CHECK(doIdentifierRef(node, &dummyResult));
    }

    return true;
}

bool Parser::doThrow(AstNode* parent, AstNode** result)
{
    const auto node = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Throw, this, parent);
    *result         = node;
    SWAG_VERIFY(node->ownerFct, error(node, Err(Err0471)));
    node->semanticFct = Semantic::resolveThrow;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, FMT(Err(Err0500), token.c_str(), node->token.c_str())));

    if (token.id == TokenId::IntrinsicGetErr)
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
    node->kwdLoc      = static_cast<Token>(token);
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0189), node->token.c_str(), token.c_str())));
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
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::TypeAlias));
    return true;
}

bool Parser::doNameAlias(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstAlias>(AstNodeKind::NameAlias, this, parent);
    node->kwdLoc      = static_cast<Token>(token);
    node->semanticFct = Semantic::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0189), node->token.c_str(), token.c_str())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual, "to specify the aliased name"));
    SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0188), token.c_str())));

    AstNode* expr;
    SWAG_CHECK(doIdentifierRef(node, &expr, IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_ARRAY));
    SWAG_CHECK(eatSemiCol("[[namealias]] expression"));
    expr->children.back()->addSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS);

    node->semanticFct = Semantic::resolveNameAlias;
    node->setResolvedSymbolName(currentScope->symTable.registerSymbolName(context, node, SymbolKind::NameAlias));
    return true;
}

bool Parser::doTopLevelIdentifier(AstNode* parent, AstNode** result)
{
    const auto tokenIdentifier = token;
    eatToken();

    const Diagnostic          err{sourceFile, tokenIdentifier, FMT(Err(Err0689), tokenIdentifier.c_str())};
    Vector<const Diagnostic*> notes;

    if (token.id == TokenId::Identifier)
    {
        if (tokenIdentifier.text == "function" || tokenIdentifier.text == "fn" || tokenIdentifier.text == "def")
            notes.push_back(Diagnostic::note(Nte(Nte0040)));
    }
    else if (token.id == TokenId::SymEqual || token.id == TokenId::SymColon)
    {
        notes.push_back(Diagnostic::note(Nte(Nte0053)));
    }

    const Utf8 appendMsg = SemanticError::findClosestMatchesMsg(tokenIdentifier.text, {}, IdentifierSearchFor::TopLevelInstruction);
    if (!appendMsg.empty())
        notes.push_back(Diagnostic::note(appendMsg));
    return context->report(err, notes);
}
