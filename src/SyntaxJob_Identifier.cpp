#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "Report.h"

void SyntaxJob::relaxIdentifier(Token& token)
{
    switch (token.id)
    {
    case TokenId::KwdAnd:
    case TokenId::KwdOr:
    case TokenId::KwdOrElse:
        token.id = TokenId::Identifier;
        return;
    }
}

bool SyntaxJob::testIsValidUserName(AstNode* node)
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

bool SyntaxJob::checkIsValidUserName(AstNode* node, Token* loc)
{
    // An identifier that starts with '__' is reserved for internal usage !
    if (!testIsValidUserName(node))
        return error(loc ? *loc : node->token, Fmt(Err(Syn0122), node->token.ctext()));

    return true;
}

bool SyntaxJob::testIsSingleIdentifier(AstNode* node)
{
    if (node->kind != AstNodeKind::IdentifierRef ||
        node->childs.size() > 1 ||
        node->childs.back()->kind != AstNodeKind::Identifier)
    {
        return false;
    }

    return true;
}

bool SyntaxJob::checkIsSingleIdentifier(AstNode* node, const char* msg)
{
    if (!testIsSingleIdentifier(node))
        return error(node, Fmt(Err(Syn0062), msg));
    return true;
}

bool SyntaxJob::doIdentifier(AstNode* parent, uint32_t identifierFlags)
{
    Token backTickValue;

    if (token.id == TokenId::SymBackTick)
    {
        auto backTickToken = token;
        SWAG_CHECK(eatToken());
        token.startLocation = backTickToken.startLocation;

        if (token.id == TokenId::SymQuestion)
            return error(token, Fmt(Err(Err0398), token.ctext()));

        backTickValue.id               = TokenId::SymBackTick;
        backTickValue.literalType      = LiteralType::TT_UNTYPED_INT;
        backTickValue.literalValue.u64 = 1;

        if (token.id == TokenId::SymLeftParen)
        {
            SWAG_CHECK(eatToken());

            if (token.id != TokenId::LiteralNumber)
                return error(token, Fmt(Err(Err0504), token.ctext()));

            if (token.literalType != LiteralType::TT_UNTYPED_INT && token.literalType != LiteralType::TT_U8)
                return error(token, Fmt(Err(Err0577), token.ctext()));
            if (token.literalValue.u64 > 255)
                return error(token, Fmt(Err(Err0575), token.ctext()));
            if (token.literalValue.u8 == 0)
                return error(token, Fmt(Err(Err0575), token.ctext()));

            backTickValue = token;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(eatToken(TokenId::SymRightParen));
        }
    }

    if (token.id == TokenId::SymQuestion && !(identifierFlags & IDENTIFIER_ACCEPT_QUESTION))
        return error(token, Fmt(Err(Err0398), token.ctext()));
    else if (token.id != TokenId::SymQuestion && Tokenizer::isSymbol(token.id))
        return error(token, Fmt(Err(Err0398), token.ctext()));
    else if (Tokenizer::isLiteral(token.id))
        return error(token, Fmt(Err(Syn0079), token.ctext()));
    else if (token.id == TokenId::EndOfFile)
        return error(token, Err(Syn0077));
    else
    {
        relaxIdentifier(token);
        if (token.id != TokenId::Identifier &&
            token.id != TokenId::NativeType &&
            token.id != TokenId::SymQuestion &&
            token.id != TokenId::CompilerSelf &&
            !Tokenizer::isIntrinsicReturn(token.id) &&
            !Tokenizer::isIntrinsicNoReturn(token.id))
        {
            return error(token, Fmt(Err(Syn0078), token.ctext()));
        }
    }

    auto identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
    identifier->inheritTokenLocation(token);
    identifier->semanticFct   = SemanticJob::resolveIdentifier;
    identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);

    if (backTickValue.id != TokenId::Invalid)
    {
        identifier->backTickMode  = IdentifierBackTypeMode::Count;
        identifier->backTickValue = backTickValue;
    }

    if (contextualNoInline)
        identifier->specFlags |= AST_SPEC_IDENTIFIER_NO_INLINE;
    SWAG_CHECK(eatToken());

    SWAG_CHECK(checkIsValidUserName(identifier));

    // Replace "Self" with the corresponding struct name
    if (identifier->token.text == g_LangSpec->name_Self)
    {
        SWAG_VERIFY(parent->ownerStructScope, Report::report({identifier, Err(Syn0135)}));
        if (currentSelfStructScope)
            identifier->token.text = currentSelfStructScope->name;
        else
            identifier->token.text = parent->ownerStructScope->name;
    }

    if (!token.lastTokenIsEOL && !(identifierFlags & IDENTIFIER_NO_GEN_PARAMS))
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
    if (!token.lastTokenIsEOL && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS))
    {
        if (token.id == TokenId::SymLeftParen)
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
            {
                Diagnostic diag{identifier, token, Fmt(Err(Syn0128), identifier->token.ctext())};
                Diagnostic note(Hlp(Hlp0035), DiagnosticLevel::Help);
                return Report::report(diag, &note);
            }

            SWAG_CHECK(eatToken(TokenId::SymLeftParen));
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
        }
        else if (!token.lastTokenIsBlank && token.id == TokenId::SymLeftCurly)
        {
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
            SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightCurly));
            identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;
        }
    }

    // Array index
    if (token.id == TokenId::SymLeftSquare)
    {
        if (!(identifierFlags & IDENTIFIER_NO_PARAMS))
        {
            if (identifierFlags & IDENTIFIER_TYPE_DECL)
                return Report::report({identifier, token, Err(Syn0120)});
            Ast::removeFromParent(identifier);
            SWAG_CHECK(doArrayPointerIndex((AstNode**) &identifier));
            Ast::addChildBack(parent, identifier);

            if (!token.lastTokenIsEOL && !(identifierFlags & IDENTIFIER_NO_FCT_PARAMS) && token.id == TokenId::SymLeftParen)
            {
                SWAG_CHECK(eatToken(TokenId::SymLeftParen));
                identifier = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, parent);
                identifier->inheritTokenLocation(token);
                identifier->token.text    = ""; // :SilentCall
                identifier->semanticFct   = SemanticJob::resolveIdentifier;
                identifier->identifierRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
                SWAG_CHECK(doFuncCallParameters(identifier, &identifier->callParameters, TokenId::SymRightParen));
            }
        }
    }

    return true;
}

bool SyntaxJob::doIdentifierRef(AstNode* parent, AstNode** result, uint32_t identifierFlags)
{
    auto identifierRef = Ast::newIdentifierRef(sourceFile, parent, this);
    if (result)
        *result = identifierRef;

    switch (token.id)
    {
    case TokenId::CompilerLocation:
        SWAG_CHECK(doCompilerLocation(identifierRef));
        break;
    case TokenId::IntrinsicLocation:
        SWAG_CHECK(doIntrinsicLocation(identifierRef));
        break;

    case TokenId::IntrinsicSpread:
    case TokenId::IntrinsicSizeOf:
    case TokenId::IntrinsicAlignOf:
    case TokenId::IntrinsicOffsetOf:
    case TokenId::IntrinsicTypeOf:
    case TokenId::IntrinsicKindOf:
    case TokenId::IntrinsicMakeType:
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
        SWAG_CHECK(doIntrinsicProp(identifierRef));
        break;

    default:
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
        break;
    }

    while (token.id == TokenId::SymDot && !token.lastTokenIsEOL)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doIdentifier(identifierRef, identifierFlags));
    }

    return true;
}

bool SyntaxJob::doDiscard(AstNode* parent, AstNode** result)
{
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
            return error(token, Fmt(Err(Syn0127), token.ctext()), nullptr, Hnt(Hnt0008));
        return error(token, Fmt(Err(Syn0173), token.ctext()));
    }

    if (result)
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

bool SyntaxJob::doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard)
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

    if (result)
        *result = node;
    SWAG_VERIFY(node->ownerFct, error(node, Fmt(Err(Syn0021), node->token.ctext())));
    SWAG_CHECK(eatToken());

    ScopedTryCatchAssume sc(this, (AstTryCatchAssume*) node);

    if (token.id == TokenId::SymLeftCurly)
    {
        node->specFlags |= AST_SPEC_TCA_BLOCK;
        SWAG_VERIFY(!afterDiscard, error(token, Err(Err0189)));
        SWAG_CHECK(doCurlyStatement(node));

        if (node->semanticFct == SemanticJob::resolveTry)
        {
            node->semanticFct = SemanticJob::resolveTryBlock;
        }
        else if (node->semanticFct == SemanticJob::resolveTryCatch)
        {
            node->ownerFct->needRegisterGetContext = true;
            node->semanticFct                      = nullptr;
        }
        else
            node->semanticFct = nullptr;
    }
    else
    {
        SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Syn0147), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err0401), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Syn0140), node->token.ctext())));
        SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Syn0146), node->token.ctext())));
        SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0019), node->token.ctext())));
        SWAG_CHECK(doIdentifierRef(node));
    }

    return true;
}

bool SyntaxJob::doThrow(AstNode* parent, AstNode** result)
{
    auto node = Ast::newNode<AstTryCatchAssume>(this, AstNodeKind::Throw, sourceFile, parent);
    SWAG_VERIFY(node->ownerFct, error(node, Err(Syn0028)));
    node->semanticFct = SemanticJob::resolveThrow;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id != TokenId::KwdTry, error(token, Fmt(Err(Syn0147), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdCatch, error(token, Fmt(Err(Err0401), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdAssume, error(token, Fmt(Err(Syn0140), node->token.ctext())));
    SWAG_VERIFY(token.id != TokenId::KwdThrow, error(token, Fmt(Err(Syn0146), node->token.ctext())));
    SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
    return true;
}