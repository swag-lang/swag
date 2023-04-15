#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"

bool Parser::checkIsValidVarName(AstNode* node)
{
    if (!checkIsValidUserName(node))
        return false;

    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->genericParameters)
            return error(identifier->genericParameters, Fmt(Err(Syn0161), identifier->token.ctext()), nullptr, Hnt(Hnt0026));
        if (identifier->callParameters)
            return error(identifier->callParameters, Fmt(Err(Err0433), identifier->token.ctext()), nullptr, Hnt(Hnt0026));
    }

    if (node->token.text[0] != '@')
        return true;

    if (node->token.text.length() >= 6)
    {
        // @alias must be of the form @aliasNUM
        if (node->token.text.find(g_LangSpec->name_atmixin) == 0)
        {
            if (node->token.text == g_LangSpec->name_atmixin)
                return error(node->token, Err(Syn0119));

            const char* pz    = node->token.text.buffer + 6;
            auto        endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, Fmt(Err(Syn0169), node->token.ctext(), node->token.ctext() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 10)
                return error(node->token, Fmt(Err(Syn0043), num));
            if (node->ownerFct)
                node->ownerFct->specFlags |= AstFuncDecl::SPECFLAG_SPEC_MIXIN;

            return true;
        }

        // @alias must be of the form @aliasNUM
        if (node->token.text.find(g_LangSpec->name_atalias) == 0)
        {
            if (node->token.text == g_LangSpec->name_atalias)
                return error(node->token, Err(Syn0023));

            const char* pz    = node->token.text.buffer + 6;
            auto        endpz = node->token.text.buffer + node->token.text.count;
            int         num   = 0;
            while (pz != endpz)
            {
                if (!SWAG_IS_DIGIT(*pz))
                    return error(node->token, Fmt(Err(Syn0168), node->token.ctext(), node->token.ctext() + 6));
                num *= 10;
                num += *pz - '0';
                pz++;
            }

            if (num >= 32)
                return error(node->token, Fmt(Err(Syn0043), num));
            if (node->ownerFct)
                node->ownerFct->aliasMask |= 1 << num;

            return true;
        }
    }

    return error(node->token, Fmt(Err(Syn0111), node->token.ctext()));
}

bool Parser::doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result)
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
        auto front = CastAst<AstIdentifierRef>(leftNode->childs.front(), AstNodeKind::IdentifierRef);

        // Then declare all other variables, and assign them to the first one
        bool firstDone = false;
        for (auto child : leftNode->childs)
        {
            SWAG_CHECK(checkIsSingleIdentifier(child, "as a variable name"));
            auto identifier = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
            identifier->computeName();
            SWAG_CHECK(checkIsValidVarName(identifier));

            AstVarDecl* varNode  = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->tokenId     = identifier->tokenId;
            varNode->assignToken = assignToken;
            varNode->flags |= AST_R_VALUE;

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

            SemanticJob::setVarDeclResolve(varNode);

            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
        }
    }

    // Tuple dereference
    else if (leftNode->kind == AstNodeKind::MultiIdentifierTuple)
    {
        SWAG_VERIFY(acceptDeref, error(leftNode, Fmt(Err(Syn0177), Naming::aKindName(currentScope->kind).c_str())));

        auto parentNode = Ast::newNode<AstNode>(this, AstNodeKind::StatementNoScope, sourceFile, parent);
        *result         = parentNode;

        // Generate an expression of the form "var __tmp_0 = assignment"
        auto        tmpVarName  = Fmt("__5tmp_%d", g_UniqueID.fetch_add(1));
        AstVarDecl* orgVarNode  = Ast::newVarDecl(sourceFile, tmpVarName, parentNode, this);
        orgVarNode->kind        = kind;
        orgVarNode->assignToken = assignToken;

        // This will avoid to initialize the tuple before the affectation
        orgVarNode->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
        orgVarNode->flags |= AST_R_VALUE;

        Ast::addChildBack(orgVarNode, type);
        orgVarNode->type = type;
        Ast::addChildBack(orgVarNode, assign);
        orgVarNode->assignment = assign;
        orgVarNode->assignment->flags |= AST_NO_LEFT_DROP;
        SemanticJob::setVarDeclResolve(orgVarNode);

        // Must be done after 'setVarDeclResolve', because 'semanticAfterFct' is already affected
        orgVarNode->token.startLocation = leftNode->childs.front()->token.startLocation;
        orgVarNode->token.endLocation   = leftNode->childs.back()->token.endLocation;
        orgVarNode->allocateExtension(ExtensionKind::Semantic);
        orgVarNode->extSemantic()->semanticAfterFct = SemanticJob::resolveTupleUnpackBeforeVar;

        if (currentScope->isGlobalOrImpl())
            SWAG_CHECK(currentScope->symTable.registerSymbolName(context, orgVarNode, SymbolKind::Variable));

        // And reference that variable, in the form value = __tmp_0.item?
        orgVarNode->publicName = "(";

        int idx = 0;
        for (size_t i = 0; i < leftNode->childs.size(); i++)
        {
            auto child = leftNode->childs[i];

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

            auto varNode         = Ast::newVarDecl(sourceFile, identifier->token.text, parentNode, this);
            varNode->kind        = kind;
            varNode->token       = identifier->token;
            varNode->assignToken = assignToken;
            varNode->flags |= AST_R_VALUE | AST_GENERATED | AST_HAS_FULL_STRUCT_PARAMETERS;
            if (currentScope->isGlobalOrImpl())
                SWAG_CHECK(currentScope->symTable.registerSymbolName(context, varNode, SymbolKind::Variable));
            identifier          = Ast::newMultiIdentifierRef(sourceFile, Fmt("%s.item%d", tmpVarName.c_str(), idx++), varNode, this);
            varNode->assignment = identifier;
            SemanticJob::setVarDeclResolve(varNode);
            varNode->assignment->flags |= AST_TUPLE_UNPACK;
        }

        orgVarNode->publicName += ")";
    }

    // Single declaration/affectation
    else
    {
        SWAG_CHECK(checkIsSingleIdentifier(leftNode, "as a variable name"));
        auto identifier = leftNode->childs.back();
        SWAG_CHECK(checkIsValidVarName(identifier));
        AstVarDecl* varNode = Ast::newVarDecl(sourceFile, identifier->token.text, parent, this);
        *result             = varNode;
        varNode->kind       = kind;
        varNode->inheritTokenLocation(leftNode);
        varNode->assignToken = assignToken;

        Ast::addChildBack(varNode, type);
        varNode->type = type;
        Ast::addChildBack(varNode, assign);
        varNode->assignment = assign;
        SemanticJob::setVarDeclResolve(varNode);
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
    if (token.id == TokenId::KwdConst)
    {
        kind = AstNodeKind::ConstDecl;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::SymLeftParen, error(token, Fmt(Err(Syn0184), token.ctext())));
    }
    else
    {
        kind = AstNodeKind::VarDecl;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::Identifier || token.id == TokenId::SymLeftParen, error(token, Fmt(Err(Syn0069), token.ctext())));
    }

    SWAG_CHECK(doVarDecl(parent, result, kind));
    return true;
}

bool Parser::doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind)
{
    AstNode* leftNode;
    while (true)
    {
        SWAG_CHECK(doLeftExpressionVar(parent, &leftNode, IDENTIFIER_NO_PARAMS));
        Ast::removeFromParent(leftNode);

        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Err(Syn0149)));
        SWAG_VERIFY(token.id != TokenId::SymSemiColon, error(token, Fmt(Err(Syn0070), token.ctext())));
        SWAG_VERIFY(token.id != TokenId::SymColonEqual, error(token, Fmt(Err(Syn0052), token.ctext()), Hnt(Hnt0116)));
        SWAG_VERIFY(token.id == TokenId::SymColon || token.id == TokenId::SymEqual, error(token, Fmt(Err(Syn0052), token.ctext())));

        AstNode* type = nullptr;
        if (token.id == TokenId::SymColon)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doTypeExpression(parent, EXPR_FLAG_IN_VAR_DECL, &type));
            Ast::removeFromParent(type);
        }

        AstNode* assign = nullptr;
        SWAG_VERIFY(token.id != TokenId::SymEqualEqual, error(token, Err(Syn0149)));
        auto assignToken = token;
        if (token.id == TokenId::SymEqual)
        {
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doInitializationExpression(assignToken, parent, &assign));
            Ast::removeFromParent(assign);
        }

        // Be sure we will be able to have a type
        if (!type && !assign)
            return error(leftNode->token, Err(Syn0131));

        SWAG_CHECK(doVarDeclExpression(parent, leftNode, type, assign, assignToken, kind, result));
        leftNode->release();

        // If we have a type, and that type has parameters (struct construction), then we need to evaluate and push the parameters
        if (type && type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpression = CastAst<AstTypeExpression>(type, AstNodeKind::TypeExpression);
            if (typeExpression->identifier && typeExpression->identifier->kind == AstNodeKind::IdentifierRef)
            {
                auto back = typeExpression->identifier->childs.back();
                if (back->kind == AstNodeKind::Identifier)
                {
                    auto identifier = CastAst<AstIdentifier>(back, AstNodeKind::Identifier);
                    if (identifier->callParameters)
                    {
                        typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
                        typeExpression->specFlags |= AstType::SPECFLAG_HAS_STRUCT_PARAMETERS;
                    }
                }
            }
        }

        if (token.id != TokenId::SymComma)
            break;
        SWAG_CHECK(eatToken());
    }

    SWAG_CHECK(eatSemiCol("end of the variable declaration"));
    return true;
}
