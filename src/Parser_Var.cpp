#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Scoped.h"
#include "Semantic.h"

bool Parser::checkIsValidVarName(AstNode* node) const
{
    if (!checkIsValidUserName(node))
        return false;

    if (node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return error(identifier->genericParameters, formErr(Err0410, identifier->token.c_str()));
        if (identifier->callParameters)
            return error(identifier->callParameters, formErr(Err0411, identifier->token.c_str()));
    }

    if (node->token.text[0] != '#')
        return true;

    if (node->token.text.length() >= 6)
    {
        // #alias must be of the form #aliasNUM
        if (node->token.text.find(g_LangSpec->name_atmixin) == 0)
        {
            if (node->token.text == g_LangSpec->name_atmixin)
                return error(node->token, toErr(Err0525));

            const char* pz    = node->token.text.buffer + 6;
            const auto  endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, formErr(Err0142, node->token.c_str() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 10)
                return error(node->token, formErr(Err0601, num));
            if (node->ownerFct)
                node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_SPEC_MIXIN);

            return true;
        }

        // #alias must be of the form #aliasNUM
        if (node->token.text.find(g_LangSpec->name_atalias) == 0)
        {
            if (node->token.text == g_LangSpec->name_atalias)
                return error(node->token, toErr(Err0515));

            const char* pz    = node->token.text.buffer + 6;
            const auto  endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, formErr(Err0138, node->token.c_str() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 32)
                return error(node->token, formErr(Err0600, num));
            if (node->ownerFct)
                node->ownerFct->aliasMask |= 1 << num;

            return true;
        }
    }

    return error(node->token, formErr(Err0407, node->token.c_str()));
}

bool Parser::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, bool forLet)
{
    bool acceptDeref = true;
    if (currentScope->is(ScopeKind::Struct) || currentScope->is(ScopeKind::File))
        acceptDeref = false;

    // Multiple affectation
    if (leftNode->is(AstNodeKind::MultiIdentifier))
    {
        auto parentNode = parent;
        if (acceptDeref || parent->is(AstNodeKind::AttrUse))
        {
            parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
            *result    = parentNode;
        }

        // Declare first variable, and affect it
        const auto front = castAst<AstIdentifierRef>(leftNode->firstChild(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (const auto child : leftNode->children)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            const auto identifier = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            AstVarDecl* varNode  = Ast::newVarDecl(identifier->token.text, this, parentNode);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->token.id    = identifier->token.id;
            varNode->assignToken = assignToken.token;
            varNode->addAstFlag(AST_R_VALUE);
            varNode->addSpecFlag(forLet ? AstVarDecl::SPEC_FLAG_IS_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

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
                     assign->is(AstNodeKind::ExplicitNoInit) ||
                     assign->is(AstNodeKind::Literal))
            {
                varNode->type = Ast::clone(type, varNode);
                if (assign)
                    varNode->assignment = Ast::clone(assign, varNode);
            }
            else
            {
                varNode->assignment = Ast::newIdentifierRef(front->token.text, this, varNode);
            }

            Semantic::setVarDeclResolve(varNode);

            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->is(AstNodeKind::MultiIdentifierTuple))
    {
        SWAG_VERIFY(acceptDeref, error(leftNode, formErr(Err0511, Naming::aKindName(currentScope->kind).c_str())));

        const auto parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
        *result               = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        const auto  tmpVarName  = form("__5tmp_%d", g_UniqueID.fetch_add(1));
        AstVarDecl* orgVarNode  = Ast::newVarDecl(tmpVarName, this, parentNode);
        orgVarNode->kind        = kind;
        orgVarNode->assignToken = assignToken.token;
        orgVarNode->addSpecFlag(forLet ? AstVarDecl::SPEC_FLAG_IS_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

        // This will avoid to initialize the tuple before the affectation
        orgVarNode->addAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        orgVarNode->addAstFlag(AST_R_VALUE);

        Ast::addChildBack(orgVarNode, type);
        orgVarNode->type = type;
        Ast::addChildBack(orgVarNode, assign);
        orgVarNode->assignment = assign;
        if (orgVarNode->assignment)
            orgVarNode->assignment->addAstFlag(AST_NO_LEFT_DROP);
        Semantic::setVarDeclResolve(orgVarNode);

        // Must be done after 'setVarDeclResolve', because 'semanticAfterFct' is already affected
        orgVarNode->token.startLocation = leftNode->firstChild()->token.startLocation;
        orgVarNode->token.endLocation   = leftNode->lastChild()->token.endLocation;
        orgVarNode->allocateExtension(ExtensionKind::Semantic);
        orgVarNode->extSemantic()->semanticAfterFct = Semantic::resolveTupleUnpackBeforeVar;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, orgVarNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        orgVarNode->publicName = "(";

        int idx = 0;
        for (uint32_t i = 0; i < leftNode->childCount(); i++)
        {
            const auto child = leftNode->children[i];

            // Ignore field if '?', otherwise check that this is a valid variable name
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            if (child->firstChild()->token.text == '?')
            {
                Ast::removeFromParent(child);
                Ast::addChildBack(parentNode, child);
                child->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
                i--;
                idx++;
                continue;
            }

            SWAG_CHECK(checkIsValidUserName(child));
            auto identifier = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            if (idx)
                orgVarNode->publicName += ", ";
            orgVarNode->publicName += identifier->token.text;

            const auto varNode   = Ast::newVarDecl(identifier->token.text, this, parentNode);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->assignToken = assignToken.token;
            varNode->addAstFlag(AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS);
            varNode->addSpecFlag(forLet ? AstVarDecl::SPEC_FLAG_IS_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);
            varNode->addSpecFlag(AstVarDecl::SPEC_FLAG_TUPLE_AFFECT);
            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
            identifier          = Ast::newMultiIdentifierRef(form("%s.item%u", tmpVarName.c_str(), idx++), this, varNode);
            varNode->assignment = identifier;
            Semantic::setVarDeclResolve(varNode);
            varNode->assignment->addAstFlag(AST_TUPLE_UNPACK);
        }

        orgVarNode->publicName += ")";
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode, "as a variable name"));
        const auto identifier = leftNode->lastChild();
        SWAG_CHECK(checkIsValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(identifier->token.text, this, parent);
        *result             = varNode;
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode->token);
        varNode->assignToken = assignToken.token;
        varNode->addSpecFlag(forLet ? AstVarDecl::SPEC_FLAG_IS_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        Semantic::setVarDeclResolve(varNode);
        varNode->addAstFlag(AST_R_VALUE);

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
    if (tokenParse.is(TokenId::KwdConst))
    {
        kind = AstNodeKind::ConstDecl;
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::SymLeftParen))
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0249, tokenParse.token.c_str())));
    }
    else
    {
        isLet = tokenParse.is(TokenId::KwdLet);
        kind  = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::SymLeftParen))
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0409, isLet ? "let" : "var", tokenParse.token.c_str())));
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

        if (tokenParse.isNot(TokenId::SymColon) && tokenParse.isNot(TokenId::SymEqual))
        {
            Utf8 msg;
            if (kind == AstNodeKind::ConstDecl)
                msg = formErr(Err0546, tokenParse.token.c_str());
            else
                msg = formErr(Err0584, tokenParse.token.c_str());

            Diagnostic err{sourceFile, tokenParse.token, msg};
            if (tokenParse.is(TokenId::SymEqualEqual))
                err.addNote(toNte(Nte0010));
            return context->report(err);
        }

        // Type
        AstNode* type = nullptr;
        if (tokenParse.is(TokenId::SymColon))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_IN_VAR_DECL, &type));
            Ast::removeFromParent(type);

            // Ambiguous {
            if (tokenParse.is(TokenId::SymLeftCurly) &&
                tokenParse.flags.has(TOKEN_PARSE_LAST_BLANK) &&
                !tokenParse.flags.has(TOKEN_PARSE_LAST_EOL) &&
                type->is(AstNodeKind::TypeExpression))
            {
                const auto typeExpr = castAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
                if (typeExpr->identifier)
                {
                    Diagnostic err{sourceFile, tokenParse.token, formErr(Err0021, typeExpr->identifier->token.c_str())};
                    err.addNote(formNte(Nte0183, typeExpr->identifier->token.c_str(), typeExpr->identifier->token.c_str()));
                    err.addNote(toNte(Nte0179));
                    return context->report(err);
                }
            }
        }

        // Value
        AstNode* assign      = nullptr;
        auto     assignToken = tokenParse;
        if (tokenParse.is(TokenId::SymEqual))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doInitializationExpression(assignToken, parent, type ? EXPR_FLAG_IN_VAR_DECL_WITH_TYPE : EXPR_FLAG_IN_VAR_DECL, &assign));
            Ast::removeFromParent(assign);
        }

        // Treat all
        SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, assignToken, kind, result, forLet));
        leftNode->release();

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->is(AstNodeKind::TypeExpression))
        {
            auto typeExpression = castAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            while (typeExpression->typeFlags.has(TYPEFLAG_IS_SUB_TYPE))
                typeExpression = castAst<AstTypeExpression>(typeExpression->lastChild(), AstNodeKind::TypeExpression);

            if (typeExpression->identifier && typeExpression->identifier->is(AstNodeKind::IdentifierRef))
            {
                const auto back = typeExpression->identifier->lastChild();
                if (back->is(AstNodeKind::Identifier))
                {
                    const auto identifier = castAst<AstIdentifier>(back, AstNodeKind::Identifier);
                    if (identifier->callParameters)
                    {
                        typeExpression->removeAstFlag(AST_NO_BYTECODE_CHILDREN);
                        type->removeAstFlag(AST_NO_BYTECODE_CHILDREN);
                        typeExpression->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);
                        type->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);
                    }
                }
            }
        }

        if (tokenParse.isNot(TokenId::SymComma))
            break;
        SWAG_CHECK(eatToken());
    }

    if (!forStruct || tokenParse.isNot(TokenId::SymRightCurly))
    {
        if (!parent || parent->isNot(AstNodeKind::If))
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymEqualEqual), error(tokenParse.token, toErr(Err0677)));
            SWAG_CHECK(eatSemiCol("variable declaration"));
        }
    }

    if (!tokenizer.comment.empty() && *result && !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE_COMMENT))
    {
        (*result)->allocateExtension(ExtensionKind::Misc);

        Vector<Utf8> tkn;
        Utf8::tokenize(tokenizer.comment, '\n', tkn);
        (*result)->extMisc()->docComment = tkn[0];

        tokenizer.comment.clear();
        for (uint32_t i = 1; i < tkn.size(); i++)
            tokenizer.comment += tkn[i];
    }

    return true;
}
