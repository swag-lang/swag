#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool Parser::doAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstAlias>(this, AstNodeKind::Alias, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveUsing;

    *result = node;
    SWAG_CHECK(eatToken());

    SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0071), token.ctext())));
    node->inheritTokenName(token);
    node->inheritTokenLocation(token);
    SWAG_CHECK(checkIsValidUserName(node));

    SWAG_CHECK(eatToken());
    SWAG_CHECK(eatToken(TokenId::SymEqual));

    AstNode* expr;
    SWAG_CHECK(doPrimaryExpression(node, EXPR_FLAG_ALIAS, &expr));
    SWAG_CHECK(eatSemiCol("'alias' expression"));

    // This is a type alias
    if (expr->kind == AstNodeKind::TypeExpression || expr->kind == AstNodeKind::TypeLambda || expr->kind == AstNodeKind::TypeClosure)
    {
        node->allocateExtension(ExtensionKind::Semantic);
        node->extSemantic()->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
        node->semanticFct                      = SemanticJob::resolveTypeAlias;
        node->resolvedSymbolName               = currentScope->symTable.registerSymbolName(context, node, SymbolKind::TypeAlias);
    }
    else
    {
        node->semanticFct        = SemanticJob::resolveAlias;
        node->resolvedSymbolName = currentScope->symTable.registerSymbolName(context, node, SymbolKind::Alias);
    }

    return true;
}

bool Parser::doLambdaClosureType(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
    auto node         = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, parent);
    *result           = node;
    node->semanticFct = SemanticJob::resolveTypeLambdaClosure;

    if (inTypeVarDecl)
    {
        auto        newScope = Ast::newScope(node, node->token.text, ScopeKind::TypeLambda, currentScope);
        Scoped      scoped(this, newScope);
        ScopedFlags sf(this, AST_IN_TYPE_VAR_DECLARATION);
        SWAG_CHECK(doLambdaClosureTypePriv(node, result, inTypeVarDecl));
    }
    else
    {
        SWAG_CHECK(doLambdaClosureTypePriv(node, result, inTypeVarDecl));
    }

    return true;
}

bool Parser::doLambdaClosureTypePriv(AstTypeLambda* node, AstNode** result, bool inTypeVarDecl)
{
    AstTypeExpression* firstAddedType = nullptr;
    AstNode*           params         = nullptr;

    switch (token.id)
    {
    case TokenId::KwdClosure:
        node->kind = AstNodeKind::TypeClosure;
        SWAG_CHECK(eatToken());

        // :ClosureForceFirstParam
        // A closure always has at least one parameter : the capture context
        params                   = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters         = params;
        firstAddedType           = Ast::newTypeExpression(sourceFile, params);
        firstAddedType->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        firstAddedType->flags |= AST_NO_SEMANTIC | AST_GENERATED;
        break;

    case TokenId::KwdFunc:
        node->kind = AstNodeKind::TypeLambda;
        SWAG_CHECK(eatToken());
        break;

    default:
        SWAG_ASSERT(false);
        break;
    }

    // If we are in a type declaration, then this must be a FuncDeclParam and not a TypeExpression
    if (inTypeVarDecl && firstAddedType)
    {
        Utf8 nameVar;
        if (firstAddedType->typeFlags & TYPEFLAG_IS_SELF)
        {
            firstAddedType->token.text = g_LangSpec->name_self;
            nameVar                    = g_LangSpec->name_self;
        }
        else
        {
            nameVar = Fmt("__%d", g_UniqueID.fetch_add(1));
        }

        auto param = Ast::newVarDecl(sourceFile, nameVar, params, this, AstNodeKind::FuncDeclParam);
        if (firstAddedType->typeFlags & TYPEFLAG_IS_SELF)
            param->specFlags |= AstVarDecl::SPECFLAG_GENERATED_SELF;

        param->allocateExtension(ExtensionKind::Misc);
        param->extMisc()->exportNode = firstAddedType;

        param->flags |= AST_GENERATED | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

        Ast::removeFromParent(firstAddedType);
        Ast::addChildBack(param, firstAddedType);
        param->type = firstAddedType;
        param->inheritTokenLocation(firstAddedType);
    }

    SWAG_CHECK(eatToken(TokenId::SymLeftParen));
    if (token.id != TokenId::SymRightParen)
    {
        if (!params)
        {
            params           = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
            node->parameters = params;
        }

        AstVarDecl* lastParameter = nullptr;
        bool        lastWasAlone  = false;
        bool        curIsAlone    = false;
        bool        thisIsAType   = false;
        while (true)
        {
            curIsAlone = true;

            Token constToken;
            bool  isConst = false;
            if (token.id == TokenId::CompilerType)
            {
                thisIsAType = true;
                curIsAlone  = false;
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id == TokenId::Identifier, error(token, Fmt(Err(Syn0196), token.ctext())));
            }

            if (token.id == TokenId::KwdConst)
            {
                curIsAlone = false;
                constToken = token;
                isConst    = true;
                SWAG_CHECK(eatToken());
            }

            // Accept a parameter name
            AstNode* namedParam = nullptr;
            if (token.id == TokenId::Identifier)
            {
                auto tokenName = token;
                tokenizer.saveState(token);
                SWAG_CHECK(eatToken());
                if (token.id != TokenId::SymColon)
                {
                    tokenizer.restoreState(token);
                }
                else if (tokenName.text == g_LangSpec->name_self)
                {
                    Diagnostic diag(sourceFile, token, Err(Syn0197));
                    diag.hint = Hnt(Hnt0102);
                    return context->report(diag);
                }
                else
                {
                    SWAG_VERIFY(inTypeVarDecl, error(tokenName, Err(Syn0191)));
                    SWAG_VERIFY(!isConst, error(constToken, Err(Syn0192)));

                    namedParam        = Ast::newNode<AstIdentifier>(this, AstNodeKind::Identifier, sourceFile, nullptr);
                    namedParam->token = tokenName;
                    SWAG_CHECK(eatToken());
                    curIsAlone  = false;
                    thisIsAType = false;
                    params->allocateExtension(ExtensionKind::Owner);
                    params->extOwner()->nodesToFree.push_back(namedParam);
                }
            }
            else
            {
                thisIsAType = true;
                curIsAlone  = false;
            }

            AstTypeExpression* typeExpr = nullptr;

            // self
            if (token.text == g_LangSpec->name_self)
            {
                curIsAlone = false;
                SWAG_VERIFY(currentStructScope, error(token, Err(Syn0026)));
                SWAG_CHECK(eatToken());
                typeExpr = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
                typeExpr->typeFlags |= TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE;
                typeExpr->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeExpr, this);
                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Syn0193));
            }
            // ...
            else if (token.id == TokenId::SymDotDotDot)
            {
                curIsAlone                = false;
                typeExpr                  = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Err0685));
            }
            // cvarargs
            else if (token.id == TokenId::KwdCVarArgs)
            {
                curIsAlone                = false;
                typeExpr                  = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoCVariadic;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Err0685));
            }
            else
            {
                SWAG_CHECK(doTypeExpression(params, EXPR_FLAG_NONE, (AstNode**) &typeExpr));
                typeExpr->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    curIsAlone = false;
                    Ast::removeFromParent(typeExpr);
                    auto newTypeExpression             = Ast::newTypeExpression(sourceFile, params);
                    newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(newTypeExpression, typeExpr);
                    if (token.id == TokenId::SymEqual)
                        return error(token, Err(Err0685));
                    typeExpr = newTypeExpression;
                }
                else if (typeExpr->identifier && !testIsSingleIdentifier(typeExpr->identifier))
                {
                    thisIsAType = true;
                    curIsAlone  = false;
                }
            }

            // We have a name
            if (namedParam)
            {
                typeExpr->allocateExtension(ExtensionKind::Misc);
                typeExpr->extMisc()->isNamed = namedParam;
            }

            SWAG_VERIFY(token.id != TokenId::SymEqual || inTypeVarDecl, error(token, Err(Syn0194)));

            // If we are in a type declaration, generate a variable and not just a type
            if (inTypeVarDecl)
            {
                auto nameVar = namedParam ? namedParam->token.text : Fmt("__%d", g_UniqueID.fetch_add(1));
                auto param   = Ast::newVarDecl(sourceFile, nameVar, params, this, AstNodeKind::FuncDeclParam);

                param->allocateExtension(ExtensionKind::Misc);
                param->extMisc()->exportNode = typeExpr;
                param->flags |= AST_GENERATED | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

                Ast::removeFromParent(typeExpr);
                Ast::addChildBack(param, typeExpr);
                param->type = typeExpr;
                if (namedParam)
                    param->inheritTokenLocation(namedParam);
                else
                    param->inheritTokenLocation(typeExpr);

                if (namedParam)
                {
                    param->allocateExtension(ExtensionKind::Misc);
                    param->extMisc()->isNamed = namedParam;
                }

                if (token.id == TokenId::SymEqual)
                {
                    thisIsAType = false;
                    curIsAlone  = false;
                    SWAG_CHECK(eatToken());
                    SWAG_CHECK(doAssignmentExpression(param, &param->assignment));

                    // Used to automatically solve enums
                    typeExpr->allocateExtension(ExtensionKind::Semantic);
                    typeExpr->extSemantic()->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;

                    // If we did not have specified a name, then this was not a type, but a name
                    // ex: func(x = 1)
                    if (!namedParam)
                    {
                        param->token.text = param->type->token.text;
                        Ast::removeFromParent(param->type);
                        param->type = nullptr;
                    }
                }

                if (lastWasAlone && !curIsAlone && !thisIsAType)
                {
                    Token tokenAmb         = param->token;
                    tokenAmb.startLocation = lastParameter->token.startLocation;
                    tokenAmb.endLocation   = token.startLocation;

                    Diagnostic diag{sourceFile, tokenAmb, Err(Syn0195)};
                    auto       note = Diagnostic::note(lastParameter, Fmt(Nte(Nte0076), lastParameter->type->token.ctext()));
                    note->hint      = Fmt(Hnt(Hnt0101), lastParameter->type->token.ctext());
                    return context->report(diag, note);
                }

                lastWasAlone  = curIsAlone;
                lastParameter = param;
            }

            SWAG_VERIFY(token.id == TokenId::SymComma || token.id == TokenId::SymRightParen, error(token, Fmt(Err(Syn0199), token.ctext())));
            if (token.id == TokenId::SymRightParen)
                break;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen, "to close the lambda parameters"));

    if (token.id == TokenId::SymMinusGreat)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &node->returnType));
    }

    if (token.id == TokenId::KwdThrow)
    {
        SWAG_CHECK(eatToken(TokenId::KwdThrow));
        node->specFlags |= AstTypeLambda::SPECFLAG_CAN_THROW;
    }

    return true;
}

bool Parser::doAnonymousStruct(AstNode* parent, AstNode** result, bool isConst, bool isUnion)
{
    auto structNode = Ast::newStructDecl(sourceFile, parent, this);
    structNode->flags |= AST_INTERNAL;
    structNode->originalParent = parent;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticBeforeFct = SemanticJob::preResolveGeneratedStruct;
    structNode->specFlags |= AstStruct::SPECFLAG_ANONYMOUS;
    if (isUnion)
        structNode->specFlags |= AstStruct::SPECFLAG_UNION;

    auto contentNode    = Ast::newNode<AstNode>(this, AstNodeKind::TupleContent, sourceFile, structNode);
    structNode->content = contentNode;
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = SemanticJob::preResolveStructContent;

    // Name
    Utf8 name = sourceFile->scopeFile->name + "_tuple_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = std::move(name);

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
    structNode->content->addAlternativeScope(currentScope);
    SWAG_ASSERT(parent);
    structNode->allocateExtension(ExtensionKind::Misc);
    structNode->extMisc()->alternativeNode = parent;

    auto newScope     = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope = newScope;

    {
        ScopedSelfStruct sf(this, parent->ownerStructScope);
        Scoped           sc(this, structNode->scope);
        ScopedStruct     ss(this, structNode->scope);

        auto startLoc = token.startLocation;
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
        while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(contentNode, SyntaxStructType::Struct, &dummyResult));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc));
    }

    // Reference to that struct
    auto identifier = Ast::newIdentifierRef(sourceFile, structNode->token.text, parent, this);
    *result         = identifier;

    identifier->childs.back()->flags |= AST_GENERATED;
    Ast::removeFromParent(structNode);
    Ast::addChildBack(newParent, structNode);
    structNode->inheritOwners(newParent);

    auto typeInfo        = makeType<TypeInfoStruct>();
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;
    typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
    structNode->typeInfo   = typeInfo;
    structNode->ownerScope = rootScope;
    rootScope->symTable.registerSymbolName(context, structNode, SymbolKind::Struct);

    Ast::visit(structNode->content, [&](AstNode* n)
               {
                   n->inheritOwners(structNode);
                   n->ownerStructScope = newScope;
                   n->ownerScope       = newScope; });

    return true;
}

bool Parser::doSingleTypeExpression(AstTypeExpression* node, AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    bool inTypeVarDecl = exprFlags & EXPR_FLAG_IN_VAR_DECL;

    // This is a @typeof
    if (token.id == TokenId::IntrinsicDeclType)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier));
        return true;
    }

    if (token.id == TokenId::NativeType)
    {
        node->literalType       = token.literalType;
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

    if (token.id == TokenId::KwdStruct)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doAnonymousStruct(node, &node->identifier, node->typeFlags & TYPEFLAG_IS_CONST, false));
        return true;
    }

    if (token.id == TokenId::KwdUnion)
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doAnonymousStruct(node, &node->identifier, node->typeFlags & TYPEFLAG_IS_CONST, true));
        return true;
    }

    if (token.id == TokenId::SymLeftCurly)
    {
        SWAG_CHECK(doAnonymousStruct(node, &node->identifier, node->typeFlags & TYPEFLAG_IS_CONST, false));
        return true;
    }

    if (token.id == TokenId::KwdFunc || token.id == TokenId::KwdClosure || token.id == TokenId::KwdMethod)
    {
        SWAG_VERIFY(token.id != TokenId::KwdMethod, context->report({sourceFile, token, Err(Syn0095)}));
        SWAG_CHECK(doLambdaClosureType(node, &node->identifier, inTypeVarDecl));
        return true;
    }

    // Specific error messages
    if (node->parent && node->parent->kind == AstNodeKind::TupleContent)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Syn0067), token.ctext())};
        return context->report(diag);
    }

    if (token.id == TokenId::SymLeftParen)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Syn0066), token.ctext())};
        auto       note = Diagnostic::help(sourceFile, token, Hlp(Hlp0004));
        return context->report(diag, note);
    }

    if (Tokenizer::isSymbol(token.id))
        return error(token, Fmt(Err(Syn0172), token.ctext()));

    // Generic error
    return error(token, Fmt(Err(Syn0066), token.ctext()));
}

bool Parser::doSubTypeExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    bool inTypeVarDecl = exprFlags & EXPR_FLAG_IN_VAR_DECL;

    auto node = Ast::newTypeExpression(sourceFile, parent, this);
    *result   = node;
    node->flags |= AST_NO_BYTECODE_CHILDS;

    // Const keyword
    if (token.id == TokenId::KwdConst)
    {
        node->locConst = token.startLocation;
        node->typeFlags |= TYPEFLAG_IS_CONST | TYPEFLAG_HAS_LOC_CONST;
        SWAG_CHECK(eatToken());
    }

    // Reference
    if (token.id == TokenId::KwdRef)
    {
        node->typeFlags |= TYPEFLAG_IS_SUB_TYPE;
        node->typeFlags |= TYPEFLAG_IS_REF;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    // Array or slice of something
    if (token.id == TokenId::SymLeftSquare)
    {
        auto leftSquareToken = token;

        SWAG_CHECK(eatToken());
        while (true)
        {
            // Slice
            if (token.id == TokenId::SymDotDot)
            {
                node->typeFlags |= TYPEFLAG_IS_SLICE;
                SWAG_CHECK(eatToken());
                break;
            }

            node->typeFlags |= TYPEFLAG_IS_ARRAY;

            // Size of array can be nothing
            if (token.id == TokenId::SymRightSquare)
            {
                node->arrayDim = UINT8_MAX;
                break;
            }

            if (node->arrayDim == 254)
                return error(token, Err(Syn0132));
            node->arrayDim++;
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }

        auto rightSquareToken = token;
        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
        if (token.flags & TOKENPARSE_LAST_EOL)
        {
            if (exprFlags & EXPR_FLAG_TYPE_EXPR)
            {
                if (inTypeVarDecl)
                    return context->report({sourceFile, rightSquareToken, Err(Syn0024)});
                Diagnostic diag{sourceFile, rightSquareToken, Err(Syn0024)};
                return context->report(diag);
            }
            else
            {
                return error(rightSquareToken, Err(Syn0024));
            }
        }

        if (token.id == TokenId::SymComma)
        {
            Diagnostic diag{sourceFile, token, Fmt(Err(Syn0096), token.ctext())};
            return context->report(diag);
        }

        node->typeFlags |= TYPEFLAG_IS_SUB_TYPE;
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    // Pointers
    if (token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex)
    {
        node->typeFlags |= TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE;
        if (token.id == TokenId::SymCircumflex)
            node->typeFlags |= TYPEFLAG_IS_PTR_ARITHMETIC;
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    SWAG_CHECK(doSingleTypeExpression(node, parent, exprFlags, &dummyResult));
    return true;
}

bool Parser::doTypeExpression(AstNode* parent, uint32_t exprFlags, AstNode** result)
{
    // Code
    if (token.id == TokenId::KwdCode)
    {
        auto node = Ast::newTypeExpression(sourceFile, parent, this);
        *result   = node;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeInfo = g_TypeMgr->typeInfoCode;
        node->typeFlags |= TYPEFLAG_IS_CODE;
        SWAG_CHECK(eatToken());
        return true;
    }

    // retval
    if (token.id == TokenId::KwdRetVal)
    {
        auto node         = Ast::newTypeExpression(sourceFile, parent, this);
        *result           = node;
        node->semanticFct = SemanticJob::resolveRetVal;
        node->flags |= AST_NO_BYTECODE_CHILDS;
        node->typeFlags |= TYPEFLAG_IS_RETVAL;

        // retval type can be followed by structure initializer, like a normal struct
        // In that case, we want the identifier pipeline to be called on it (to check
        // parameters like a normal struct).
        // So we create an identifier, that will be matched with the type alias automatically
        // created in the function.
        SWAG_CHECK(eatToken());
        if (!(token.flags & TOKENPARSE_LAST_EOL) && token.id == TokenId::SymLeftCurly)
        {
            node->identifier = Ast::newIdentifierRef(sourceFile, g_LangSpec->name_retval, node, this);
            auto id          = CastAst<AstIdentifier>(node->identifier->childs.back(), AstNodeKind::Identifier);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(id, &id->callParameters, TokenId::SymRightCurly));
            id->flags |= AST_IN_TYPE_VAR_DECLARATION;
            id->callParameters->specFlags |= AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT;
        }

        return true;
    }

    // This is a lambda
    if (token.id == TokenId::KwdFunc || token.id == TokenId::KwdClosure || token.id == TokenId::KwdMethod)
    {
        SWAG_VERIFY(token.id != TokenId::KwdMethod, context->report({sourceFile, token, Err(Syn0095)}));
        SWAG_CHECK(doLambdaClosureType(parent, result, exprFlags & EXPR_FLAG_IN_VAR_DECL));
        return true;
    }

    // Move reference
    bool isMoveRef = false;
    if (token.id == TokenId::KwdMoveRef)
    {
        SWAG_CHECK(eatToken());
        isMoveRef = true;
    }

    SWAG_CHECK(doSubTypeExpression(parent, exprFlags, result));

    if (isMoveRef)
    {
        // TODO: moveref on ref
        auto typeNode = CastAst<AstTypeExpression>(*result, AstNodeKind::TypeExpression);
        typeNode->typeFlags |= TYPEFLAG_IS_REF | TYPEFLAG_IS_MOVE_REF;
    }

    return true;
}

bool Parser::doCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::Cast, sourceFile, parent);
    *result           = node;
    node->semanticFct = SemanticJob::resolveExplicitCast;
    SWAG_CHECK(eatToken());

    // Cast modifiers
    uint32_t mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->tokenId, mdfFlags));
    if (mdfFlags & MODIFIER_OVERFLOW)
    {
        node->specFlags |= AstCast::SPECFLAG_OVERFLOW;
        node->attributeFlags |= ATTRIBUTE_CAN_OVERFLOW_ON;
    }

    if (mdfFlags & MODIFIER_BIT)
    {
        node->specFlags |= AstCast::SPECFLAG_BIT;
        node->semanticFct = SemanticJob::resolveExplicitBitCast;
    }

    if ((mdfFlags & MODIFIER_BIT) && (mdfFlags & MODIFIER_OVERFLOW))
    {
        return error(node, Err(Syn0186));
    }

    if (mdfFlags & MODIFIER_UNCONST)
    {
        node->specFlags |= AstCast::SPECFLAG_UNCONST;
    }

    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'cast'"));
    SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after the type expression"));

    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doAutoCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::AutoCast, sourceFile, parent);
    *result           = node;
    node->semanticFct = SemanticJob::resolveExplicitAutoCast;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}
