#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Scoped.h"
#include "Semantic.h"

bool Parser::checkIsValidVarName(AstNode* node)
{
    if (!checkIsValidUserName(node))
        return false;

    if (node->kind == AstNodeKind::Identifier)
    {
        const auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return error(identifier->genericParameters, Fmt(Err(Err0410), identifier->token.ctext()));
        if (identifier->callParameters)
            return error(identifier->callParameters, Fmt(Err(Err0411), identifier->token.ctext()));
    }

    if (node->token.text[0] != '#')
        return true;

    if (node->token.text.length() >= 6)
    {
        // #alias must be of the form #aliasNUM
        if (node->token.text.find(g_LangSpec->name_atmixin) == 0)
        {
            if (node->token.text == g_LangSpec->name_atmixin)
                return error(node->token, Err(Err0525));

            const char* pz    = node->token.text.buffer + 6;
            const auto  endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, Fmt(Err(Err0142), node->token.ctext() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 10)
                return error(node->token, Fmt(Err(Err0601), num));
            if (node->ownerFct)
                node->ownerFct->addSpecFlags(AstFuncDecl::SPECFLAG_SPEC_MIXIN);

            return true;
        }

        // #alias must be of the form #aliasNUM
        if (node->token.text.find(g_LangSpec->name_atalias) == 0)
        {
            if (node->token.text == g_LangSpec->name_atalias)
                return error(node->token, Err(Err0515));

            const char* pz    = node->token.text.buffer + 6;
            const auto  endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, Fmt(Err(Err0138), node->token.ctext() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 32)
                return error(node->token, Fmt(Err(Err0600), num));
            if (node->ownerFct)
                node->ownerFct->aliasMask |= 1 << num;

            return true;
        }
    }

    return error(node->token, Fmt(Err(Err0407), node->token.ctext()));
}

bool Parser::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet)
{
    bool acceptDeref = true;
    if (currentScope->kind == ScopeKind::Struct || currentScope->kind == ScopeKind::File)
        acceptDeref = false;

    // Multiple affectation
    if (leftNode->kind == AstNodeKind::MultiIdentifier)
    {
        auto parentNode = parent;
        if (acceptDeref || parent->kind == AstNodeKind::AttrUse)
        {
            parentNode = Ast::newNode<AstNode>(this, AstNodeKind::StatementNoScope, sourceFile, parent);
            *result    = parentNode;
        }

        // Declare first variable, and affect it
        const auto front = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (const auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            const auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            AstVarDecl* varNode  = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->tokenId     = identifier->tokenId;
            varNode->assignToken = assignToken;
            varNode->flags |= AST_R_VALUE;
            varNode->addSpecFlags(forLet ? (AstVarDecl::SPECFLAG_IS_LET | AstVarDecl::SPECFLAG_CONST_ASSIGN) : 0);

            if (!firstDone)
            {
                firstDone = true;
                Ast::addChildBack(varNode, type);
                Ast::addChildBack(varNode, assign);
                varNode->type       = type;
                varNode->assignment = assign;
            }

            // We are supposed to be constexpr, so we need to duplicate the assignment instead of generating an
            // affectation to the first variable. If there's no assignment, then just declare the variable
            // without the affectation to avoid a useless copy (and a postCopy in case of structs)
            else if (!acceptDeref ||
                     assign == nullptr ||
                     assign->kind == AstNodeKind::ExplicitNoInit ||
                     assign->kind == AstNodeKind::Literal)
            {
                varNode->type = Ast::clone(type, varNode);
                if (assign)
                    varNode->assignment = Ast::clone(assign, varNode);
            }
            else
            {
                varNode->assignment = Ast::newIdentifierRef(sourceFile, front->token.text, varNode, this);
            }

            Semantic::setVarDeclResolve(varNode);

            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
    {
        SWAG_VERIFY(acceptDeref, error(leftNode, Fmt(Err(Err0511), Naming::aKindName(currentScope->kind).c_str())));

        const auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::StatementNoScope, sourceFile, parent);
        *result               = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        const auto  tmpVarName  = Fmt("__5tmp_%d", g_UniqueID.fetch_add(1));
        AstVarDecl* orgVarNode  = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
        orgVarNode->kind        = kind;
        orgVarNode->assignToken = assignToken;
        orgVarNode->addSpecFlags(forLet ? (AstVarDecl::SPECFLAG_IS_LET | AstVarDecl::SPECFLAG_CONST_ASSIGN) : 0);

        // This will avoid to initialize the tuple before the affectation
        orgVarNode->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
        orgVarNode->flags |= AST_R_VALUE;

        Ast::addChildBack(orgVarNode, type);
        orgVarNode->type = type;
        Ast::addChildBack(orgVarNode, assign);
        orgVarNode->assignment = assign;
        if (orgVarNode->assignment)
            orgVarNode->assignment->flags |= AST_NO_LEFT_DROP;
        Semantic::setVarDeclResolve(orgVarNode);

        // Must be done after 'setVarDeclResolve', because 'semanticAfterFct' is already affected
        orgVarNode->token.startLocation = leftNode->childs.front()->token.startLocation;
        orgVarNode->token.endLocation   = leftNode->childs.back()->token.endLocation;
        orgVarNode->allocateExtension(ExtensionKind::Semantic);
        orgVarNode->extSemantic()->semanticAfterFct = Semantic::resolveTupleUnpackBeforeVar;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, orgVarNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        orgVarNode->publicName = "(";

        int idx = 0;
        for (size_t i = 0; i < leftNode->childs.size(); i++)
        {
            const auto child = leftNode->childs[i];

            // Ignore field if '?', otherwise check that this is a valid variable name
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            if (child->childs.front()->token.text == '?')
            {
                Ast::removeFromParent(child);
                Ast::addChildBack(parentNode, child);
                child->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE;
                i--;
                idx++;
                continue;
            }

            SWAG_CHECK(checkIsValidUserName(child));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            if (idx)
                orgVarNode->publicName += ", ";
            orgVarNode->publicName += identifier->token.text;

            const auto varNode   = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->assignToken = assignToken;
            varNode->flags |= AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS;
            varNode->addSpecFlags(forLet ? (AstVarDecl::SPECFLAG_IS_LET | AstVarDecl::SPECFLAG_CONST_ASSIGN) : 0);
            varNode->addSpecFlags(AstVarDecl::SPECFLAG_TUPLE_AFFECT);
            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
            identifier          = Ast::newMultiIdentifierRef(sourceFile, Fmt("%s.item%u", tmpVarName.c_str(), idx++), varNode, this);
            varNode->assignment = identifier;
            Semantic::setVarDeclResolve(varNode);
            varNode->assignment->flags |= AST_TUPLE_UNPACK;
        }

        orgVarNode->publicName += ")";
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode, "as a variable name"));
        const auto identifier = leftNode->childs.back();
        SWAG_CHECK(checkIsValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->token.text, parent, this);
        *result             = varNode;
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode);
        varNode->assignToken = assignToken;
        varNode->addSpecFlags(forLet ? (AstVarDecl::SPECFLAG_IS_LET | AstVarDecl::SPECFLAG_CONST_ASSIGN) : 0);

        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        Semantic::setVarDeclResolve(varNode);
        varNode->flags |= AST_R_VALUE;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
    }

    return true;
}

bool Parser::doVarDecl(AstNode* parent, AstNode** result)
{
    // First variable
    AstNodeKind kind;
    bool        isLet = false;
    if (token.id == TokenId::KwdConst)
    {
        kind = AstNodeKind::ConstDecl;
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::SymLeftParen)
            SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0249), token.ctext())));
    }
    else
    {
        isLet = token.id == TokenId::KwdLet;
        kind  = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::SymLeftParen)
            SWAG_CHECK(checkIsIdentifier(token, Fmt(Err(Err0409), isLet ? "let" : "var", token.ctext())));
    }

    SWAG_CHECK(doVarDecl(parent, result, kind, false, isLet));
    return true;
}

bool Parser::doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind, bool forStruct, bool forLet)
{
    AstNode* leftNode;
    while (true)
    {
        SWAG_CHECK(doLeftExpressionVar(parent, &leftNode, IDENTIFIER_NO_PARAMS));
        Ast::removeFromParent(leftNode);

        if (token.id != TokenId::SymColon && token.id != TokenId::SymEqual)
        {
            Utf8 msg;
            if (kind == AstNodeKind::ConstDecl)
                msg = Fmt(Err(Err0546), token.ctext());
            else
                msg = Fmt(Err(Err0584), token.ctext());

            const Diagnostic diag{sourceFile, token, msg};
            if (token.id == TokenId::SymEqualEqual)
                return context->report(diag, Diagnostic::note(Nte(Nte0010)));
            return context->report(diag);
        }

        // Type
        AstNode* type = nullptr;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_IN_VAR_DECL, &type));
            Ast::removeFromParent(type);

            // Ambigous {
            if (token.id == TokenId::SymLeftCurly &&
                (token.flags & TOKENPARSE_LAST_BLANK) &&
                !(token.flags & TOKENPARSE_LAST_EOL) &&
                type->kind == AstNodeKind::TypeExpression)
            {
                const auto typeExpr = CastAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
                if (typeExpr->identifier)
                {
                    const Diagnostic diag{sourceFile, token, Fmt(Err(Err0021), typeExpr->identifier->token.ctext())};
                    const auto       note  = Diagnostic::note(Fmt(Nte(Nte0183), typeExpr->identifier->token.ctext(), typeExpr->identifier->token.ctext()));
                    const auto       note1 = Diagnostic::note(Nte(Nte0179));
                    return context->report(diag, note, note1);
                }
            }
        }

        // Value
        AstNode* assign      = nullptr;
        auto     assignToken = token;
        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doInitializationExpression(assignToken, parent, type ? EXPR_FLAG_IN_VAR_DECL_WITH_TYPE : EXPR_FLAG_IN_VAR_DECL, &assign));
            Ast::removeFromParent(assign);
        }

        // Treat all
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, assignToken, kind, result, forLet));
        leftNode->release();

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpression = CastAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            while (typeExpression->typeFlags & TYPEFLAG_IS_SUB_TYPE)
                typeExpression = CastAst<AstTypeExpression>(typeExpression->childs.back(), AstNodeKind::TypeExpression);

            if (typeExpression->identifier && typeExpression->identifier->kind == AstNodeKind::IdentifierRef)
            {
                const auto back = typeExpression->identifier->childs.back();
                if (back->kind == AstNodeKind::Identifier)
                {
                    const auto identifier = CastAst<AstIdentifier>(back, AstNodeKind::Identifier);
                    if (identifier->callParameters)
                    {
                        typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
                        type->flags &= ~AST_NO_BYTECODE_CHILDS;
                        typeExpression->addSpecFlags(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);
                        type->addSpecFlags(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);
                    }
                }
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    if (!forStruct || token.id != TokenId::SymRightCurly)
    {
        if (!parent || parent->kind != AstNodeKind::If)
        {
            SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Err(Err0677)));
            SWAG_CHECK(eatSemiCol("variable declaration"));
        }
    }

    if (!tokenizer.comment.empty() && *result && !(token.flags & TOKENPARSE_EOL_BEFORE_COMMENT))
    {
        (*result)->allocateExtension(ExtensionKind::Misc);

        Vector<Utf8> tkn;
        Utf8::tokenize(tokenizer.comment, '\n', tkn);
        (*result)->extMisc()->docComment = tkn[0];

        tokenizer.comment.clear();
        for (int i = 1; i < (int) tkn.size(); i++)
            tokenizer.comment += tkn[i];
    }

    return true;
}
