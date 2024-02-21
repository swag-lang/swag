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

    if (node->kind == AstNodeKind::Identifier)
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return error(identifier->genericParameters, FMT(Err(Err0410), identifier->token.c_str()));
        if (identifier->callParameters)
            return error(identifier->callParameters, FMT(Err(Err0411), identifier->token.c_str()));
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
                    return error(node->token, FMT(Err(Err0142), node->token.c_str() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 10)
                return error(node->token, FMT(Err(Err0601), num));
            if (node->ownerFct)
                node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_SPEC_MIXIN);

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
                    return error(node->token, FMT(Err(Err0138), node->token.c_str() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 32)
                return error(node->token, FMT(Err(Err0600), num));
            if (node->ownerFct)
                node->ownerFct->aliasMask |= 1 << num;

            return true;
        }
    }

    return error(node->token, FMT(Err(Err0407), node->token.c_str()));
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
            parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
            *result    = parentNode;
        }

        // Declare first variable, and affect it
        const auto front = castAst<AstIdentifierRef>(leftNode->children.front(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (const auto child : leftNode->children)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            const auto identifier = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            AstVarDecl* varNode  = Ast::newVarDecl(identifier->token.text, this, parentNode, sourceFile);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->token.id    = identifier->token.id;
            varNode->assignToken = assignToken;
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
                     assign->kind == AstNodeKind::ExplicitNoInit ||
                     assign->kind == AstNodeKind::Literal)
            {
                varNode->type = Ast::clone(type, varNode);
                if (assign)
                    varNode->assignment = Ast::clone(assign, varNode);
            }
            else
            {
                varNode->assignment = Ast::newIdentifierRef(front->token.text, this, varNode, varNode->token.sourceFile);
            }

            Semantic::setVarDeclResolve(varNode);

            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
    {
        SWAG_VERIFY(acceptDeref, error(leftNode, FMT(Err(Err0511), Naming::aKindName(currentScope->kind).c_str())));

        const auto parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
        *result               = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        const auto  tmpVarName  = FMT("__5tmp_%d", g_UniqueID.fetch_add(1));
        AstVarDecl* orgVarNode  = Ast::newVarDecl(tmpVarName, this, parentNode, sourceFile);
        orgVarNode->kind        = kind;
        orgVarNode->assignToken = assignToken;
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
        orgVarNode->token.startLocation = leftNode->children.front()->token.startLocation;
        orgVarNode->token.endLocation   = leftNode->children.back()->token.endLocation;
        orgVarNode->allocateExtension(ExtensionKind::Semantic);
        orgVarNode->extSemantic()->semanticAfterFct = Semantic::resolveTupleUnpackBeforeVar;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, orgVarNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        orgVarNode->publicName = "(";

        int idx = 0;
        for (size_t i = 0; i < leftNode->children.size(); i++)
        {
            const auto child = leftNode->children[i];

            // Ignore field if '?', otherwise check that this is a valid variable name
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            if (child->children.front()->token.text == '?')
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

            const auto varNode   = Ast::newVarDecl(identifier->token.text, this, parentNode, sourceFile);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->assignToken = assignToken;
            varNode->addAstFlag(AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS);
            varNode->addSpecFlag(forLet ? AstVarDecl::SPEC_FLAG_IS_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);
            varNode->addSpecFlag(AstVarDecl::SPEC_FLAG_TUPLE_AFFECT);
            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
            identifier          = Ast::newMultiIdentifierRef(FMT("%s.item%u", tmpVarName.c_str(), idx++), this, varNode, sourceFile);
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
        const auto identifier = leftNode->children.back();
        SWAG_CHECK(checkIsValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(identifier->token.text, this, parent, sourceFile);
        *result             = varNode;
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode->token);
        varNode->assignToken = assignToken;
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
    if (token.id == TokenId::KwdConst)
    {
        kind = AstNodeKind::ConstDecl;
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::SymLeftParen)
            SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0249), token.c_str())));
    }
    else
    {
        isLet = token.id == TokenId::KwdLet;
        kind  = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        if (token.id != TokenId::SymLeftParen)
            SWAG_CHECK(checkIsIdentifier(token, FMT(Err(Err0409), isLet ? "let" : "var", token.c_str())));
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
                msg = FMT(Err(Err0546), token.c_str());
            else
                msg = FMT(Err(Err0584), token.c_str());

            const Diagnostic err{sourceFile, token, msg};
            if (token.id == TokenId::SymEqualEqual)
                return context->report(err, Diagnostic::note(Nte(Nte0010)));
            return context->report(err);
        }

        // Type
        AstNode* type = nullptr;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_IN_VAR_DECL, &type));
            Ast::removeFromParent(type);

            // Ambiguous {
            if (token.id == TokenId::SymLeftCurly &&
                token.flags.has(TOKEN_PARSE_LAST_BLANK) &&
                !token.flags.has(TOKEN_PARSE_LAST_EOL) &&
                type->kind == AstNodeKind::TypeExpression)
            {
                const auto typeExpr = castAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
                if (typeExpr->identifier)
                {
                    const Diagnostic err{sourceFile, token, FMT(Err(Err0021), typeExpr->identifier->token.c_str())};
                    const auto       note  = Diagnostic::note(FMT(Nte(Nte0183), typeExpr->identifier->token.c_str(), typeExpr->identifier->token.c_str()));
                    const auto       note1 = Diagnostic::note(Nte(Nte0179));
                    return context->report(err, note, note1);
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
            auto typeExpression = castAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            while (typeExpression->typeFlags.has(TYPEFLAG_IS_SUB_TYPE))
                typeExpression = castAst<AstTypeExpression>(typeExpression->children.back(), AstNodeKind::TypeExpression);

            if (typeExpression->identifier && typeExpression->identifier->kind == AstNodeKind::IdentifierRef)
            {
                const auto back = typeExpression->identifier->children.back();
                if (back->kind == AstNodeKind::Identifier)
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

    if (!tokenizer.comment.empty() && *result && !token.flags.has(TOKEN_PARSE_EOL_BEFORE_COMMENT))
    {
        (*result)->allocateExtension(ExtensionKind::Misc);

        Vector<Utf8> tkn;
        Utf8::tokenize(tokenizer.comment, '\n', tkn);
        (*result)->extMisc()->docComment = tkn[0];

        tokenizer.comment.clear();
        for (int i = 1; i < static_cast<int>(tkn.size()); i++)
            tokenizer.comment += tkn[i];
    }

    return true;
}
