#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool SyntaxJob::doAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstAlias>(this, AstNodeKind::Alias, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveUsing;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Utf8::format(Msg0333, token.text.c_str())));
    node->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual));

    AstNode* expr;
    SWAG_CHECK(doPrimaryExpression(node, EXPR_FLAG_NONE, &expr));
    SWAG_CHECK(eatSemiCol("'alias' expression"));

    // This is a type alias
    if (expr->kind == AstNodeKind::TypeExpression || expr->kind == AstNodeKind::TypeLambda)
    {
        node->allocateExtension();
        node->extension->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
        node->semanticFct                  = SemanticJob::resolveTypeAlias;
        node->resolvedSymbolName           = currentScope->symTable.registerSymbolName(&context, node, SymbolKind::TypeAlias);
    }
    else
    {
        node->semanticFct        = SemanticJob::resolveAlias;
        node->resolvedSymbolName = currentScope->symTable.registerSymbolName(&context, node, SymbolKind::Alias);
    }

    return true;
}

bool SyntaxJob::doTypeExpressionLambda(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveTypeLambda;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        auto params      = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters = params;
        while (true)
        {
            bool isConst = false;
            if (token.id == TokenId::KwdConst)
            {
                isConst = true;
                SWAG_CHECK(eatToken());
            }

            if (token.text == g_LangSpec->name_self)
            {
                SWAG_VERIFY(currentStructScope, error(token, Msg0334));
                SWAG_CHECK(eatToken());
                auto typeNode         = Ast::newTypeExpression(sourceFile, params);
                typeNode->ptrCount    = 1;
                typeNode->ptrFlags[0] = isConst ? AstTypeExpression::PTR_CONST : 0;
                typeNode->typeFlags |= isConst ? TYPEFLAG_ISCONST : 0;
                typeNode->typeFlags |= TYPEFLAG_ISSELF;
                typeNode->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeNode, this);
            }
            else
            {
                AstNode* typeExpr;
                SWAG_CHECK(doTypeExpression(params, &typeExpr));
                ((AstTypeExpression*) typeExpr)->typeFlags |= isConst ? TYPEFLAG_ISCONST : 0;
            }

            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen));
    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(node, &node->returnType));
    }

    if (token.id == TokenId::KwdThrow)
    {
        SWAG_CHECK(eatToken(TokenId::KwdThrow));
        node->specFlags |= AST_SPEC_TYPELAMBDA_CANTHROW;
    }

    return true;
}

bool SyntaxJob::convertExpressionListToTuple(AstNode* parent, AstNode** result, bool isConst, bool anonymousStruct, bool anonymousUnion)
{
    auto structNode = Ast::newStructDecl(sourceFile, parent, this);
    structNode->attributeFlags |= ATTRIBUTE_PRIVATE;
    structNode->originalParent = parent;
    structNode->allocateExtension();
    structNode->extension->semanticBeforeFct = SemanticJob::preResolveGeneratedStruct;

    if (anonymousStruct)
        structNode->structFlags |= STRUCTFLAG_ANONYMOUS;
    if (anonymousUnion)
        structNode->structFlags |= STRUCTFLAG_UNION;

    auto contentNode    = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode, this);
    structNode->content = contentNode;
    contentNode->allocateExtension();
    contentNode->extension->semanticBeforeFct = SemanticJob::preResolveStructContent;

    // Name
    Utf8 name = sourceFile->scopePrivate->name + "_tuple_";
    name += Utf8::format("%d", token.startLocation);
    structNode->token.text = move(name);

    // :SubDeclParent
    auto newParent = parent;
    while (newParent != sourceFile->astRoot && !(newParent->flags & AST_GLOBAL_NODE) && (newParent->kind != AstNodeKind::Namespace))
    {
        newParent = newParent->parent;
        SWAG_ASSERT(newParent);
    }

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->fromTests)
        rootScope = sourceFile->scopePrivate;
    else
        rootScope = newParent->ownerScope;
    structNode->allocateExtension();
    structNode->extension->alternativeScopes.push_back(currentScope);
    SWAG_ASSERT(parent);
    structNode->extension->alternativeNode = parent;

    auto newScope     = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope = newScope;

    {
        ScopedSelfStruct sf(this, parent->ownerStructScope);
        Scoped           sc(this, structNode->scope);
        ScopedStruct     ss(this, structNode->scope);

        // Content
        if (anonymousStruct)
        {
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
            while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
                SWAG_CHECK(doStructBody(contentNode, SyntaxStructType::Struct));
            SWAG_CHECK(eatToken(TokenId::SymRightCurly));
        }
        else
        {
            SWAG_CHECK(doStructBodyTuple(contentNode, false));
        }
    }

    // Reference to that struct
    auto identifier = Ast::newIdentifierRef(sourceFile, structNode->token.text, parent, this);
    if (result)
        *result = identifier;

    Ast::removeFromParent(structNode);
    Ast::addChildBack(newParent, structNode);
    structNode->inheritOwners(newParent);

    auto typeInfo        = allocType<TypeInfoStruct>();
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;
    typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
    structNode->typeInfo   = typeInfo;
    structNode->ownerScope = rootScope;
    rootScope->symTable.registerSymbolName(&context, structNode, SymbolKind::Struct);

    Ast::visit(structNode->content, [&](AstNode* n) {
        n->inheritOwners(structNode);
        n->ownerStructScope = newScope;
        n->ownerScope       = newScope;
    });

    return true;
}

bool SyntaxJob::doTypeExpression(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
    // Code
    if (token.id == TokenId::KwdCode)
    {
        auto node = Ast::newTypeExpression(sourceFile, parent, this);
        if (result)
            *result = node;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeInfo = g_TypeMgr.typeInfoCode;
        node->typeFlags |= TYPEFLAG_ISCODE;
        SWAG_CHECK(eatToken());
        return true;
    }

    // Alias
    if (token.id == TokenId::KwdAlias)
    {
        auto node = Ast::newTypeExpression(sourceFile, parent, this);
        if (result)
            *result = node;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeInfo = g_TypeMgr.typeInfoNameAlias;
        node->typeFlags |= TYPEFLAG_ISNAMEALIAS;
        SWAG_CHECK(eatToken());
        return true;
    }

    // This is a lambda
    if (token.id == TokenId::KwdFunc)
    {
        SWAG_CHECK(eatToken());
        return doTypeExpressionLambda(parent, result);
    }

    // retval
    if (token.id == TokenId::KwdRetVal)
    {
        auto node = Ast::newTypeExpression(sourceFile, parent, this);
        if (result)
            *result = node;
        node->semanticFct = SemanticJob::resolveRetVal;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeFlags |= TYPEFLAG_RETVAL;

        // retval type can be followed by structure initializer, like a normal struct
        // In that case, we want the identifier pipeline to be called on it (to check
        // parameters like a normal struct).
        // So we create an identifier, that will be matched with the type alias automatically
        // created in the function.
        SWAG_CHECK(eatToken());
        if (token.id == TokenId::SymLeftCurly)
        {
            node->identifier = Ast::newIdentifierRef(sourceFile, "retval", node, this);
            auto id          = CastAst<AstIdentifier>(node->identifier->childs.back(), AstNodeKind::Identifier);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(id, &id->callParameters, TokenId::SymRightCurly));
            id->flags |= AST_IN_TYPE_VAR_DECLARATION;
            id->callParameters->flags |= AST_CALL_FOR_STRUCT;
        }

        return true;
    }

    // Const keyword
    bool isConst    = false;
    bool isPtrConst = false;
    if (token.id == TokenId::KwdConst)
    {
        isConst    = true;
        isPtrConst = true;
        SWAG_CHECK(eatToken());
    }

    // Else this is a type expression
    auto node = Ast::newTypeExpression(sourceFile, parent, this);
    if (result)
        *result = node;
    node->flags |= AST_NO_BYTECODE_CHILDS;
    node->typeFlags |= isConst ? TYPEFLAG_ISCONST : 0;
    node->arrayDim = 0;
    node->ptrCount = 0;

    // Array
    if (token.id == TokenId::SymLeftSquare)
    {
        isPtrConst = false;

        SWAG_CHECK(eatToken());
        while (true)
        {
            // Size of array can be nothing
            if (token.id == TokenId::SymRightSquare)
            {
                node->arrayDim = UINT8_MAX;
                break;
            }

            // Slice
            if (token.id == TokenId::SymDotDot)
            {
                node->typeFlags |= TYPEFLAG_ISSLICE;
                SWAG_CHECK(eatToken());
                break;
            }

            if (node->arrayDim == 254)
                return error(token, Msg0338);
            node->arrayDim++;
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    // Const after array
    if (token.id == TokenId::KwdConst)
    {
        isPtrConst = true;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(token.id == TokenId::SymAsterisk, error(token, Msg0339));
    }

    // Pointers
    if (token.id == TokenId::SymAsterisk)
    {
        while (token.id == TokenId::SymAsterisk)
        {
            if (node->ptrCount == AstTypeExpression::MAX_PTR_COUNT)
                return error(token, Utf8::format(Msg0340, AstTypeExpression::MAX_PTR_COUNT));
            node->ptrFlags[node->ptrCount] = isPtrConst ? AstTypeExpression::PTR_CONST : 0;
            SWAG_CHECK(eatToken());
            isPtrConst = false;

            if (token.id == TokenId::KwdConst)
            {
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id == TokenId::SymAsterisk || token.id == TokenId::SymAmpersand, error(token, Msg0341));

                // Pointer to a const reference
                if (token.id == TokenId::SymAmpersand)
                {
                    SWAG_CHECK(eatToken());
                    node->ptrFlags[node->ptrCount] |= AstTypeExpression::PTR_REF | AstTypeExpression::PTR_CONST;
                }

                isPtrConst = true;
            }

            node->ptrCount++;
        }
    }

    // This is a @typeof
    if (token.id == TokenId::IntrinsicTypeOf || token.id == TokenId::IntrinsicKindOf)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier));
        auto typeNode = CastAst<AstIntrinsicProp>(node->identifier->childs.front(), AstNodeKind::IntrinsicProp);
        typeNode->specFlags |= AST_SPEC_INTRINSIC_TYPEOF_AS_TYPE;
        return true;
    }

    if (token.id == TokenId::NativeType)
    {
        node->token = move(token);
        SWAG_CHECK(eatToken());
        return true;
    }

    if (token.id == TokenId::Identifier)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier, IDENTIFIER_TYPE_DECL | IDENTIFIER_NO_ARRAY));
        if (inTypeVarDecl)
            node->identifier->childs.back()->flags |= AST_IN_TYPE_VAR_DECLARATION;
        return true;
    }
    else if (token.id == TokenId::KwdStruct)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(convertExpressionListToTuple(node, &node->identifier, isConst, true, false));
        return true;
    }
    else if (token.id == TokenId::KwdUnion)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(convertExpressionListToTuple(node, &node->identifier, isConst, true, true));
        return true;
    }
    else if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(convertExpressionListToTuple(node, &node->identifier, isConst, false, false));
        return true;
    }

    return error(token, Utf8::format(Msg0343, token.text.c_str()));
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::Cast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitCast;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'cast'"));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after type expression"));

    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE));
    return true;
}

bool SyntaxJob::doBitCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::BitCast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitBitCast;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'bitcast'"));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after type expression"));

    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE));
    return true;
}

bool SyntaxJob::doAutoCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::AutoCast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitAutoCast;
    if (result)
        *result = node;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE));
    return true;
}
