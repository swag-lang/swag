#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

bool Parser::checkIsValidVarName(AstNode* node, VarDeclFlags varDeclFlags) const
{
    if (!checkIsValidUserName(node))
        return false;

    if (node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return error(identifier->genericParameters, formErr(Err0638, identifier->token.cstr()));
        if (identifier->callParameters)
            return error(identifier->callParameters, formErr(Err0189, identifier->token.cstr(), "a variable name"));
    }

    if (node->token.text == g_LangSpec->name_me)
        return error(node->token, toErr(Err0269));

    // Special field name starts with 'item' followed by a number
    if (varDeclFlags.has(VAR_DECL_FLAG_FOR_STRUCT) && !node->hasAstFlag(AST_GENERATED))
    {
        // User cannot name its variables itemX
        if (isItemName(node->token.text))
            return context->report({node, node->token, formErr(Err0510, node->token.cstr())});
    }

    if (node->token.text[0] != '#')
        return true;

    if (node->token.text.length() >= 4)
    {
        // #uniq must be of the form #uniqNUM
        if (node->token.text.find(g_LangSpec->name_sharp_uniq) == 0)
        {
            if (node->token.is(g_LangSpec->name_sharp_uniq))
                return error(node->token, toErr(Err0420));

            const char* pz  = node->token.text.buffer + g_LangSpec->name_sharp_uniq.length();
            const auto  end = node->token.text.buffer + node->token.text.count;
            uint32_t    num = 0;
            while (pz != end)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, formErr(Err0121, node->token.cstr() + 4));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 10)
                return error(node->token, formErr(Err0495, num));
            if (node->ownerFct)
                node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_HAS_UNIQ);

            return true;
        }

        // #alias must be of the form #aliasNUM
        if (node->token.text.find(g_LangSpec->name_sharp_alias) == 0)
        {
            if (node->token.is(g_LangSpec->name_sharp_alias))
                return error(node->token, toErr(Err0414));

            const char* pz  = node->token.text.buffer + g_LangSpec->name_sharp_alias.length();
            const auto  end = node->token.text.buffer + node->token.text.count;
            uint32_t    num = 0;
            while (pz != end)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, formErr(Err0118, node->token.cstr() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 32)
                return error(node->token, formErr(Err0494, num));
            if (node->ownerFct)
                node->ownerFct->aliasMask |= 1 << num;

            return true;
        }
    }

    return error(node->token, formErr(Err0748, node->token.cstr()));
}

bool Parser::doVarDeclMultiIdentifier(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, VarDeclFlags varDeclFlags, bool acceptDeref)
{
    auto parentNode = parent;
    if (acceptDeref || parent->is(AstNodeKind::AttrUse))
    {
        parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
        *result    = parentNode;
    }

    // Declare the first variable and affect it
    const auto front = castAst<AstIdentifierRef>(leftNode->firstChild(), AstNodeKind::IdentifierRef);

    // Then declare all other variables and assign them to the first one
    AstVarDecl* orgVarNode = nullptr;
    for (const auto child : leftNode->children)
    {
        SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
        const auto identifier = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        identifier->computeName();
        SWAG_CHECK(checkIsValidVarName(identifier, varDeclFlags));

        AstVarDecl* varNode  = Ast::newVarDecl(identifier->token.text, this, parentNode);
        varNode->kind        = kind;
        varNode->token       = identifier->token;
        varNode->token.id    = identifier->token.id;
        varNode->assignToken = assignToken.token;
        varNode->addAstFlag(AST_R_VALUE);
        varNode->addSpecFlag(varDeclFlags.has(VAR_DECL_FLAG_IS_LET) ? AstVarDecl::SPEC_FLAG_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

        if (orgVarNode)
        {
            varNode->addAstFlag(AST_GENERATED);
            orgVarNode->multiNames.push_back(varNode->token.text);
        }

        if (!orgVarNode)
        {
            orgVarNode = varNode;
            orgVarNode->multiNames.push_back(varNode->token.text);
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

    return true;
}

bool Parser::doVarDeclMultiIdentifierTuple(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, VarDeclFlags varDeclFlags, bool acceptDeref)
{
    SWAG_VERIFY(acceptDeref, error(leftNode, formErr(Err0408, Naming::aKindName(currentScope->kind).cstr())));

    const auto parentNode = Ast::newNode<AstStatement>(AstNodeKind::StatementNoScope, this, parent);
    *result               = parentNode;

    // Generate an expression of the form "var __tmp_0 = assignment"
    const auto  tmpVarName  = form("__5tmp_%d", g_UniqueID.fetch_add(1));
    AstVarDecl* orgVarNode  = Ast::newVarDecl(tmpVarName, this, parentNode);
    orgVarNode->kind        = kind;
    orgVarNode->assignToken = assignToken.token;
    orgVarNode->addSpecFlag(varDeclFlags.has(VAR_DECL_FLAG_IS_LET) ? AstVarDecl::SPEC_FLAG_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

    // This will avoid having to initialize the tuple before the affectation
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
    orgVarNode->addAstFlag(AST_EXPR_IN_PARENTS);

    int idx = 0;
    for (uint32_t i = 0; i < leftNode->childCount(); i++)
    {
        const auto child = leftNode->children[i];

        // Ignore the field if '?', otherwise check that this is a valid variable name
        SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
        if (child->firstChild()->token.is("?"))
        {
            orgVarNode->multiNames.push_back("?");
            child->addAstFlag(AST_GENERATED);
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

        orgVarNode->multiNames.push_back(identifier->token.text);

        const auto varNode   = Ast::newVarDecl(identifier->token.text, this, parentNode);
        varNode->kind        = kind;
        varNode->token       = identifier->token;
        varNode->assignToken = assignToken.token;
        varNode->addAstFlag(AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS);
        varNode->addSpecFlag(varDeclFlags.has(VAR_DECL_FLAG_IS_LET) ? AstVarDecl::SPEC_FLAG_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);
        varNode->addSpecFlag(AstVarDecl::SPEC_FLAG_TUPLE_AFFECT);
        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
        identifier          = Ast::newMultiIdentifierRef(form("%s.item%u", tmpVarName.cstr(), idx++), this, varNode);
        varNode->assignment = identifier;
        Semantic::setVarDeclResolve(varNode);
        varNode->assignment->addAstFlag(AST_TUPLE_UNPACK);
    }

    return true;
}

bool Parser::doVarDeclSingleIdentifier(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, VarDeclFlags varDeclFlags)
{
    SWAG_CHECK(checkIsSingleIdentifier(leftNode, "as a variable name"));
    const auto identifier = leftNode->lastChild();
    SWAG_CHECK(checkIsValidVarName(identifier, varDeclFlags));

    AstVarDecl* varNode = Ast::newVarDecl(identifier->token.text, this, parent);
    *result             = varNode;
    varNode->kind       = kind;
    varNode->inheritTokenLocation(leftNode->token);
    varNode->assignToken = assignToken.token;
    varNode->addSpecFlag(varDeclFlags.has(VAR_DECL_FLAG_IS_LET) ? AstVarDecl::SPEC_FLAG_LET | AstVarDecl::SPEC_FLAG_CONST_ASSIGN : 0);

    Ast::addChildBack(varNode, type);
    varNode->type = type;
    Ast::addChildBack(varNode, assign);
    varNode->assignment = assign;
    Semantic::setVarDeclResolve(varNode);
    varNode->addAstFlag(AST_R_VALUE);

    if (currentScope->isGlobalOrImpl())
        SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
    return true;
}

bool Parser::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result, VarDeclFlags varDeclFlags)
{
    bool acceptDeref = true;
    if (currentScope->is(ScopeKind::Struct) || currentScope->is(ScopeKind::File))
        acceptDeref = false;

    // Multiple affectation
    if (leftNode->is(AstNodeKind::MultiIdentifier))
    {
        SWAG_CHECK(doVarDeclMultiIdentifier(parent, leftNode, type, assign, assignToken, kind, result, varDeclFlags, acceptDeref));
    }

    // Tuple dereference
    else if (leftNode->is(AstNodeKind::MultiIdentifierTuple))
    {
        SWAG_CHECK(doVarDeclMultiIdentifierTuple(parent, leftNode, type, assign, assignToken, kind, result, varDeclFlags, acceptDeref));
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(doVarDeclSingleIdentifier(parent, leftNode, type, assign, assignToken, kind, result, varDeclFlags));
    }

    return true;
}

bool Parser::doVarDecl(AstNode* parent, AstNode** result)
{
    TokenParse  savedTokenParse = tokenParse;
    AstNodeKind kind;
    bool        isLet = false;

    if (tokenParse.is(TokenId::KwdConst))
    {
        kind = AstNodeKind::ConstDecl;
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::SymLeftParen))
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdVar), error(tokenParse, formErr(Err0312, "var", "const", "var")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdLet), error(tokenParse, formErr(Err0312, "let", "const", "let")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdConst), error(tokenParse, formErr(Err0313, "const", "const", "const")));
            SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0174)));
        }
    }
    else if (tokenParse.is(TokenId::KwdLet))
    {
        isLet = true;
        kind  = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::SymLeftParen))
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdConst), error(tokenParse, formErr(Err0312, "const", "let", "const")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdVar), error(tokenParse, formErr(Err0312, "var", "let", "var")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdLet), error(tokenParse, formErr(Err0313, "let", "let", "let")));
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0273, "let")));
        }
    }
    else
    {
        kind = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        if (tokenParse.isNot(TokenId::SymLeftParen))
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdConst), error(tokenParse, formErr(Err0312, "const", "var", "const")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdLet), error(tokenParse, formErr(Err0312, "let", "var", "let")));
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdVar), error(tokenParse, formErr(Err0313, "var", "var", "var")));
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0273, "var")));
        }
    }

    const auto count = parent->children.size();
    SWAG_CHECK(doVarDecl(parent, result, kind, isLet ? VAR_DECL_FLAG_IS_LET : 0));
    if (*result)
        FormatAst::inheritFormatBefore(this, *result, &savedTokenParse);
    else
        FormatAst::inheritFormatBefore(this, parent->children[count], &savedTokenParse);
    return true;
}

bool Parser::doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind, VarDeclFlags varDeclFlags)
{
    AstNode* leftNode;
    bool     first = true;
    while (true)
    {
        SWAG_CHECK(doLeftExpressionVar(parent, &leftNode, IDENTIFIER_NO_PARAMS, varDeclFlags));
        Ast::removeFromParent(leftNode);

        if (tokenParse.isNot(TokenId::SymColon) && tokenParse.isNot(TokenId::SymEqual))
        {
            Utf8 msg;
            if (kind == AstNodeKind::ConstDecl)
                msg = toErr(Err0443);
            else
                msg = toErr(Err0479);

            Diagnostic err{sourceFile, tokenParse, msg};
            if (tokenParse.is(TokenId::SymEqualEqual))
                err.addNote("did you mean to assign a value with the symbol [['=']] instead?");

            if (leftNode->is(AstNodeKind::IdentifierRef))
            {
                if (kind == AstNodeKind::ConstDecl)
                    err.addNote(Diagnostic::hereIs(leftNode, form("this is the declaration of the constant [[%s]]", leftNode->token.cstr())));
                else
                    err.addNote(Diagnostic::hereIs(leftNode, form("this is the declaration of the variable [[%s]]", leftNode->token.cstr())));
            }
            else
            {
                err.addNote(Diagnostic::hereIs(leftNode, "this is the declaration"));
            }

            return context->report(err);
        }

        // Type
        AstNode* type = nullptr;
        if (tokenParse.is(TokenId::SymColon))
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_IN_VAR_DECL, &type));
            Ast::removeFromParent(type);
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
        {
            ParserPushFreezeFormat ff(this);
            AstNode*               varExpr = nullptr;
            SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, assignToken, kind, &varExpr, varDeclFlags));
            leftNode->release();
            if (!first && varExpr)
                varExpr->addSpecFlag(AstVarDecl::SPEC_FLAG_EXTRA_DECL);
            else
                *result = varExpr;
            first = false;
        }

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->is(AstNodeKind::TypeExpression))
        {
            auto typeExpression = castAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            while (typeExpression->typeFlags.has(TYPE_FLAG_IS_SUB_TYPE))
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

    if (!varDeclFlags.has(VAR_DECL_FLAG_FOR_STRUCT) || tokenParse.isNot(TokenId::SymRightCurly))
    {
        if (!parent || parent->isNot(AstNodeKind::If))
        {
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymEqualEqual), error(tokenParse, toErr(Err0627)));
            if (tokenParse.isNot(TokenId::SymRightCurly))
                SWAG_CHECK(eatSemiCol("variable declaration"));
        }
    }

    return true;
}
