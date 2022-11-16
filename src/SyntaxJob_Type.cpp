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

    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Err0333), token.ctext())));
    node->inheritTokenName(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual));

    AstNode* expr;
    SWAG_CHECK(doPrimaryExpression(node, EXPR_FLAG_NONE, &expr));
    SWAG_CHECK(eatSemiCol("`alias` expression"));

    // This is a type alias
    if (expr->kind == AstNodeKind::TypeExpression || expr->kind == AstNodeKind::TypeLambda || expr->kind == AstNodeKind::TypeClosure)
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

bool SyntaxJob::doTypeExpressionLambdaClosure(AstNode* parent, AstNode** result)
{
    AstNodeKind kind;
    bool        isMethod  = false;
    bool        isMethodC = false;

    if (token.id == TokenId::KwdMethod)
    {
        PushErrHint eh(Hnt(Hnt0049));
        SWAG_VERIFY(currentStructScope, error(token, Err(Err0894)));
        isMethod = true;
        kind     = AstNodeKind::TypeLambda;
    }
    else if (token.id == TokenId::KwdConstMethod)
    {
        PushErrHint eh(Hnt(Hnt0050));
        SWAG_VERIFY(currentStructScope, error(token, Err(Err0894)));
        isMethodC = true;
        kind      = AstNodeKind::TypeLambda;
    }
    else if (token.id == TokenId::KwdFunc)
    {
        kind = AstNodeKind::TypeLambda;
    }
    else
    {
        kind = AstNodeKind::TypeClosure;
    }

    SWAG_CHECK(eatToken());

    auto node         = Ast::newNode<AstTypeLambda>(this, kind, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveTypeLambdaClosure;
    if (result)
        *result = node;

    AstNode* params = nullptr;

    // :ClosureForceFirstParam
    // A closure always has at least one parameter : the capture context
    if (kind == AstNodeKind::TypeClosure)
    {
        params           = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters = params;

        auto typeNode      = Ast::newTypeExpression(sourceFile, params);
        typeNode->typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
        typeNode->flags |= AST_NO_SEMANTIC | AST_GENERATED;
    }
    else if (isMethod)
    {
        params               = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters     = params;
        auto typeNode        = Ast::newTypeExpression(sourceFile, params);
        typeNode->ptrCount   = 1;
        typeNode->typeFlags  = TYPEFLAG_IS_SELF;
        typeNode->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeNode, this);
    }
    else if (isMethodC)
    {
        params                = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters      = params;
        auto typeNode         = Ast::newTypeExpression(sourceFile, params);
        typeNode->ptrCount    = 1;
        typeNode->ptrFlags[0] = AstTypeExpression::PTR_CONST;
        typeNode->typeFlags |= TYPEFLAG_IS_CONST;
        typeNode->typeFlags |= TYPEFLAG_IS_SELF;
        typeNode->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeNode, this);
    }

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        if (!params)
        {
            params           = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
            node->parameters = params;
        }

        while (true)
        {
            bool isConst = false;
            if (token.id == TokenId::KwdConst)
            {
                isConst = true;
                SWAG_CHECK(eatToken());
            }

            // Accept a parameter name
            if (token.id == TokenId::Identifier)
            {
                tokenizer.saveState(token);
                SWAG_CHECK(eatToken());
                if (token.id != TokenId::SymColon)
                    tokenizer.restoreState(token);
                else
                    SWAG_CHECK(eatToken());
            }

            if (token.text == g_LangSpec->name_self)
            {
                SWAG_VERIFY(currentStructScope, error(token, Err(Err0334)));
                SWAG_CHECK(eatToken());
                auto typeNode         = Ast::newTypeExpression(sourceFile, params);
                typeNode->ptrCount    = 1;
                typeNode->ptrFlags[0] = isConst ? AstTypeExpression::PTR_CONST : 0;
                typeNode->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
                typeNode->typeFlags |= TYPEFLAG_IS_SELF;
                typeNode->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeNode, this);
            }
            // ...
            else if (token.id == TokenId::SymDotDotDot)
            {
                auto typeExpr             = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                SWAG_CHECK(eatToken());
            }
            // cvarargs
            else if (token.id == TokenId::KwdCVarArgs)
            {
                auto typeExpr             = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoCVariadic;
                SWAG_CHECK(eatToken());
            }
            else
            {
                AstNode* typeExpr;
                SWAG_CHECK(doTypeExpression(params, &typeExpr));
                ((AstTypeExpression*) typeExpr)->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    Ast::removeFromParent(typeExpr);
                    auto newTypeExpression             = Ast::newTypeExpression(sourceFile, params);
                    newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(newTypeExpression, typeExpr);
                }
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
    structNode->flags |= AST_PRIVATE;
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
    Utf8 name = sourceFile->scopeFile->name + "_tuple_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
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
        rootScope = sourceFile->scopeFile;
    else
        rootScope = newParent->ownerScope;
    structNode->allocateExtension();
    structNode->addAlternativeScope(currentScope);
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

    identifier->childs.back()->flags |= AST_GENERATED;
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

    Ast::visit(structNode->content, [&](AstNode* n)
               {
                   n->inheritOwners(structNode);
                   n->ownerStructScope = newScope;
                   n->ownerScope       = newScope; });

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
        node->typeInfo = g_TypeMgr->typeInfoCode;
        node->typeFlags |= TYPEFLAG_IS_CODE;
        SWAG_CHECK(eatToken());
        return true;
    }

    // This is a lambda
    if (token.id == TokenId::KwdFunc || token.id == TokenId::KwdClosure || token.id == TokenId::KwdMethod || token.id == TokenId::KwdConstMethod)
    {
        return doTypeExpressionLambdaClosure(parent, result);
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
            node->identifier = Ast::newIdentifierRef(sourceFile, g_LangSpec->name_retval, node, this);
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
    node->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
    node->arrayDim = 0;
    node->ptrCount = 0;

    // Reference
    if (token.id == TokenId::KwdRef)
    {
        node->typeFlags |= TYPEFLAG_IS_REF;
        SWAG_CHECK(eatToken());
    }

    // Array
    if (token.id == TokenId::SymLeftSquare)
    {
        isPtrConst           = false;
        auto leftSquareToken = token;

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
                node->typeFlags |= TYPEFLAG_IS_SLICE;
                SWAG_CHECK(eatToken());
                break;
            }

            if (node->arrayDim == 254)
                return error(token, Err(Err0338));
            node->arrayDim++;
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        auto rightSquareToken = token;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        if (token.lastTokenIsEOL)
        {
            if (contextFlags & CONTEXT_FLAG_EXPRESSION)
            {
                if (inTypeVarDecl)
                    return sourceFile->report({sourceFile, rightSquareToken, Err(Err0561)});
                Diagnostic diag{sourceFile, rightSquareToken, Err(Err0561)};
                Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0002), DiagnosticLevel::Help};
                return sourceFile->report(diag, &note);
            }
            else
            {
                return error(rightSquareToken, Err(Err0561));
            }
        }

        if (token.id == TokenId::SymLeftSquare)
        {
            if (node->typeFlags & TYPEFLAG_IS_SLICE)
            {
                Diagnostic diag{sourceFile, token, Fmt(Err(Err0527), token.ctext())};
                Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0025), DiagnosticLevel::Help};
                return sourceFile->report(diag, &note);
            }
            else
            {
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymDotDot)
                {
                    node->typeFlags |= TYPEFLAG_IS_SLICE;
                    SWAG_CHECK(eatToken());
                    SWAG_CHECK(eatToken(TokenId::SymRightSquare));
                }
                else
                {
                    Diagnostic diag{sourceFile, token, Fmt(Err(Err0526), token.ctext())};
                    Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0024), DiagnosticLevel::Help};
                    return sourceFile->report(diag, &note);
                }
            }
        }

        if (g_TokenFlags[(int) token.id] & TOKEN_SYM &&
            token.id != TokenId::SymComma &&
            token.id != TokenId::SymLeftCurly &&
            token.id != TokenId::SymAsterisk &&
            token.id != TokenId::SymCircumflex)
        {
            if (inTypeVarDecl)
                return sourceFile->report({sourceFile, token, Fmt(Err(Err0343), token.ctext())});

            Diagnostic diag{sourceFile, token, Fmt(Err(Err0343), token.ctext())};
            Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0002), DiagnosticLevel::Help};
            return sourceFile->report(diag, &note);
        }

        if (token.id == TokenId::SymComma)
        {
            // Special error if inside a function call parameter
            auto callParam = node->findParent(AstNodeKind::FuncCallParam);
            if (callParam)
            {
                Diagnostic diag{sourceFile, token, Fmt(Err(Err0171), token.ctext())};
                Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0002), DiagnosticLevel::Help};
                return sourceFile->report(diag, &note);
            }
            else
            {
                return error(token, Fmt(Err(Err0171), token.ctext()));
            }
        }
    }

    // Const after array
    if (token.id == TokenId::KwdConst)
    {
        isPtrConst      = true;
        auto tokenConst = token;

        SWAG_CHECK(eatToken());

        if (token.id == TokenId::SymLeftSquare)
        {
            SWAG_CHECK(eatToken(TokenId::SymLeftSquare));
            SWAG_CHECK(eatToken(TokenId::SymDotDot));
            SWAG_CHECK(eatToken(TokenId::SymRightSquare));
            node->typeFlags |= TYPEFLAG_IS_CONST_SLICE | TYPEFLAG_IS_SLICE;
        }
        else
        {
            SWAG_VERIFY(token.id == TokenId::SymAsterisk, error(token, Err(Err0339)));
        }
    }

    // Pointers
    if (token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex)
    {
        while (token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex)
        {
            if (node->ptrCount == AstTypeExpression::MAX_PTR_COUNT)
                return error(token, Fmt(Err(Err0340), AstTypeExpression::MAX_PTR_COUNT));
            node->ptrFlags[node->ptrCount] = isPtrConst ? AstTypeExpression::PTR_CONST : 0;
            if (token.id == TokenId::SymCircumflex)
                node->ptrFlags[node->ptrCount] |= AstTypeExpression::PTR_ARITMETIC;
            SWAG_CHECK(eatToken());
            isPtrConst = false;

            if (token.id == TokenId::KwdConst)
            {
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex || token.id == TokenId::SymAmpersand, error(token, Err(Err0339)));
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
        node->token.literalType = token.literalType;
        node->token.endLocation = token.endLocation;
        SWAG_CHECK(eatToken());
        return true;
    }

    if (token.id == TokenId::Identifier)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier, IDENTIFIER_TYPE_DECL | IDENTIFIER_NO_ARRAY));
        if (inTypeVarDecl)
            node->identifier->childs.back()->flags |= AST_IN_TYPE_VAR_DECLARATION;
        node->token.endLocation = node->identifier->token.endLocation;
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
    else if (token.id == TokenId::KwdFunc || token.id == TokenId::KwdClosure || token.id == TokenId::KwdMethod || token.id == TokenId::KwdConstMethod)
    {
        // We generate a type alias, and make a reference to that type
        auto alias = Ast::newNode<AstAlias>(this, AstNodeKind::Alias, sourceFile, parent);
        alias->flags |= AST_PRIVATE | AST_GENERATED;
        alias->semanticFct = SemanticJob::resolveUsing;
        Utf8 name          = sourceFile->scopeFile->name + "_alias_";
        name += Fmt("%d", g_UniqueID.fetch_add(1));
        alias->token.text = move(name);
        alias->allocateExtension();
        alias->extension->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
        alias->semanticFct                  = SemanticJob::resolveTypeAlias;
        alias->resolvedSymbolName           = currentScope->symTable.registerSymbolName(&context, alias, SymbolKind::TypeAlias);
        node->identifier                    = Ast::newIdentifierRef(sourceFile, alias->token.text, node, this);
        SWAG_CHECK(doTypeExpressionLambdaClosure(alias));

        node->identifier->allocateExtension();
        node->identifier->extension->exportNode = alias->childs.front();
        return true;
    }

    // Specific error messages
    if (parent && parent->kind == AstNodeKind::TupleContent)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Err0202), token.ctext())};
        Diagnostic note{sourceFile, parent->token, Hlp(Hlp0003), DiagnosticLevel::Help};
        return sourceFile->report(diag, &note);
    }

    if (token.id == TokenId::SymLeftParen)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Err0343), token.ctext())};
        Diagnostic note{sourceFile, token, Hlp(Hlp0004), DiagnosticLevel::Help};
        return sourceFile->report(diag, &note);
    }

    if (Tokenizer::isSymbol(token.id))
        return error(token, Fmt(Err(Err0839), token.ctext()));

    // Generic error
    return error(token, Fmt(Err(Err0343), token.ctext()));
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
