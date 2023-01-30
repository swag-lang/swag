#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Scoped.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"

bool SyntaxJob::doAlias(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstAlias>(this, AstNodeKind::Alias, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveUsing;
    if (result)
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
        node->extension->semantic->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
        node->semanticFct                            = SemanticJob::resolveTypeAlias;
        node->resolvedSymbolName                     = currentScope->symTable.registerSymbolName(&context, node, SymbolKind::TypeAlias);
    }
    else
    {
        node->semanticFct        = SemanticJob::resolveAlias;
        node->resolvedSymbolName = currentScope->symTable.registerSymbolName(&context, node, SymbolKind::Alias);
    }

    return true;
}

bool SyntaxJob::doTypeExpressionLambdaClosure(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
    auto node         = Ast::newNode<AstTypeLambda>(this, AstNodeKind::TypeLambda, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveTypeLambdaClosure;
    if (result)
        *result = node;

    if (inTypeVarDecl)
    {
        auto        newScope = Ast::newScope(node, node->token.text, ScopeKind::TypeLambda, currentScope);
        Scoped      scoped(this, newScope);
        ScopedFlags sf(this, AST_IN_TYPE_VAR_DECLARATION);
        SWAG_CHECK(doTypeExpressionLambdaClosureTypeOrDecl(node, result, inTypeVarDecl));
    }
    else
    {
        SWAG_CHECK(doTypeExpressionLambdaClosureTypeOrDecl(node, result, inTypeVarDecl));
    }

    return true;
}

bool SyntaxJob::doTypeExpressionLambdaClosureTypeOrDecl(AstTypeLambda* node, AstNode** result, bool inTypeVarDecl)
{
    bool isMethod  = false;
    bool isMethodC = false;

    if (token.id == TokenId::KwdMethod)
    {
        SWAG_VERIFY(currentStructScope, error(token, Err(Syn0025), nullptr, Hnt(Hnt0049)));
        isMethod   = true;
        node->kind = AstNodeKind::TypeLambda;
    }
    else if (token.id == TokenId::KwdConstMethod)
    {
        SWAG_VERIFY(currentStructScope, error(token, Err(Syn0025), nullptr, Hnt(Hnt0050)));
        isMethodC  = true;
        node->kind = AstNodeKind::TypeLambda;
    }
    else if (token.id == TokenId::KwdFunc)
    {
        node->kind = AstNodeKind::TypeLambda;
    }
    else
    {
        node->kind = AstNodeKind::TypeClosure;
    }

    SWAG_CHECK(eatToken());

    AstNode* params = nullptr;

    // :ClosureForceFirstParam
    // A closure always has at least one parameter : the capture context
    AstTypeExpression* firstAddedType = nullptr;

    if (node->kind == AstNodeKind::TypeClosure)
    {
        params           = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters = params;

        firstAddedType           = Ast::newTypeExpression(sourceFile, params);
        firstAddedType->typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
        firstAddedType->flags |= AST_NO_SEMANTIC | AST_GENERATED;
    }
    else if (isMethod)
    {
        params                     = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters           = params;
        firstAddedType             = Ast::newTypeExpression(sourceFile, params);
        firstAddedType->ptrCount   = 1;
        firstAddedType->typeFlags  = TYPEFLAG_IS_SELF;
        firstAddedType->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, firstAddedType, this);
    }
    else if (isMethodC)
    {
        params                      = Ast::newNode<AstNode>(this, AstNodeKind::FuncDeclParams, sourceFile, node);
        node->parameters            = params;
        firstAddedType              = Ast::newTypeExpression(sourceFile, params);
        firstAddedType->ptrCount    = 1;
        firstAddedType->ptrFlags[0] = AstTypeExpression::PTR_CONST;
        firstAddedType->typeFlags |= TYPEFLAG_IS_CONST;
        firstAddedType->typeFlags |= TYPEFLAG_IS_SELF;
        firstAddedType->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, firstAddedType, this);
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
            param->specFlags |= AST_SPEC_DECLPARAM_GENERATED_SELF;

        param->allocateExtension(ExtensionKind::ExportNode);
        param->extension->misc->exportNode = firstAddedType;

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

        while (true)
        {
            Token constToken;
            bool  isConst = false;
            if (token.id == TokenId::KwdConst)
            {
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
                else
                {
                    SWAG_VERIFY(inTypeVarDecl, error(tokenName, Err(Syn0191)));
                    SWAG_VERIFY(!isConst, error(constToken, Err(Syn0192)));
                    namedParam        = Ast::newNode<AstNode>(this, AstNodeKind::Identifier, sourceFile, nullptr);
                    namedParam->token = tokenName;
                    SWAG_CHECK(eatToken());
                }
            }

            AstTypeExpression* typeExpr = nullptr;
            if (token.text == g_LangSpec->name_self)
            {
                SWAG_VERIFY(currentStructScope, error(token, Err(Syn0026)));
                SWAG_CHECK(eatToken());
                typeExpr              = Ast::newTypeExpression(sourceFile, params);
                typeExpr->ptrCount    = 1;
                typeExpr->ptrFlags[0] = isConst ? AstTypeExpression::PTR_CONST : 0;
                typeExpr->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;
                typeExpr->typeFlags |= TYPEFLAG_IS_SELF;
                typeExpr->identifier = Ast::newIdentifierRef(sourceFile, currentStructScope->name, typeExpr, this);

                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Syn0193));
            }
            // ...
            else if (token.id == TokenId::SymDotDotDot)
            {
                typeExpr                  = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Err0685));
            }
            // cvarargs
            else if (token.id == TokenId::KwdCVarArgs)
            {
                typeExpr                  = Ast::newTypeExpression(sourceFile, params);
                typeExpr->typeFromLiteral = g_TypeMgr->typeInfoCVariadic;
                SWAG_CHECK(eatToken());
                if (token.id == TokenId::SymEqual)
                    return error(token, Err(Err0685));
            }
            else
            {
                SWAG_CHECK(doTypeExpression(params, (AstNode**) &typeExpr));
                ((AstTypeExpression*) typeExpr)->typeFlags |= isConst ? TYPEFLAG_IS_CONST : 0;

                // type...
                if (token.id == TokenId::SymDotDotDot)
                {
                    Ast::removeFromParent(typeExpr);
                    auto newTypeExpression             = Ast::newTypeExpression(sourceFile, params);
                    newTypeExpression->typeFromLiteral = g_TypeMgr->typeInfoVariadic;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(newTypeExpression, typeExpr);
                    if (token.id == TokenId::SymEqual)
                        return error(token, Err(Err0685));
                    typeExpr = newTypeExpression;
                }
            }

            // We have a name
            if (namedParam)
            {
                typeExpr->allocateExtension(ExtensionKind::Misc);
                typeExpr->extension->misc->isNamed = namedParam;
            }

            SWAG_VERIFY(token.id != TokenId::SymEqual || inTypeVarDecl, error(token, Err(Syn0194)));

            // If we are in a type declaration, generate a variable and not just a type
            if (inTypeVarDecl)
            {
                auto nameVar = namedParam ? namedParam->token.text : Fmt("__%d", g_UniqueID.fetch_add(1));
                auto param   = Ast::newVarDecl(sourceFile, nameVar, params, this, AstNodeKind::FuncDeclParam);

                param->allocateExtension(ExtensionKind::ExportNode);
                param->extension->misc->exportNode = typeExpr;
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
                    param->extension->misc->isNamed = namedParam;
                }

                if (token.id == TokenId::SymEqual)
                {
                    SWAG_CHECK(eatToken());
                    SWAG_CHECK(doAssignmentExpression(param, &param->assignment));

                    // Used to automatically solve enums
                    typeExpr->allocateExtension(ExtensionKind::Semantic);
                    typeExpr->extension->semantic->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;

                    // If we did not have specified a name, then this was not a type, but a name
                    // ex: func(x = 1)
                    if (!namedParam)
                    {
                        param->token.text = param->type->token.text;
                        Ast::removeFromParent(param->type);
                        param->type       = nullptr;
                    }
                }
            }

            if (token.id != TokenId::SymComma)
                break;
            SWAG_CHECK(eatToken());
        }
    }

    SWAG_CHECK(eatToken(TokenId::SymRightParen, "to close the lambda parameters"));
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
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extension->semantic->semanticBeforeFct = SemanticJob::preResolveGeneratedStruct;

    if (anonymousStruct)
        structNode->structFlags |= STRUCTFLAG_ANONYMOUS;
    if (anonymousUnion)
        structNode->structFlags |= STRUCTFLAG_UNION;

    auto contentNode    = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode, this);
    structNode->content = contentNode;
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extension->semantic->semanticBeforeFct = SemanticJob::preResolveStructContent;

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
    structNode->allocateExtension(ExtensionKind::AltScopes);
    structNode->addAlternativeScope(currentScope);
    SWAG_ASSERT(parent);
    structNode->extension->misc->alternativeNode = parent;

    auto newScope     = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope = newScope;

    {
        ScopedSelfStruct sf(this, parent->ownerStructScope);
        Scoped           sc(this, structNode->scope);
        ScopedStruct     ss(this, structNode->scope);

        // Content
        if (anonymousStruct)
        {
            auto startLoc = token.startLocation;
            SWAG_CHECK(eatToken(TokenId::SymLeftCurly));
            while (token.id != TokenId::SymRightCurly && (token.id != TokenId::EndOfFile))
                SWAG_CHECK(doStructBody(contentNode, SyntaxStructType::Struct));
            SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, nullptr));
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
        return doTypeExpressionLambdaClosure(parent, result, inTypeVarDecl);
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
        if (!token.lastTokenIsEOL && token.id == TokenId::SymLeftCurly)
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
                return error(token, Err(Syn0132));
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
                    return Report::report({sourceFile, rightSquareToken, Err(Syn0024)});
                Diagnostic diag{sourceFile, rightSquareToken, Err(Syn0024)};
                return Report::report(diag);
            }
            else
            {
                return error(rightSquareToken, Err(Syn0024));
            }
        }

        if (token.id == TokenId::SymLeftSquare)
        {
            if (node->typeFlags & TYPEFLAG_IS_SLICE)
            {
                Diagnostic diag{sourceFile, token, Fmt(Err(Syn0095), token.ctext())};
                Diagnostic note{sourceFile, leftSquareToken, Hlp(Hlp0025), DiagnosticLevel::Help};
                return Report::report(diag, &note);
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
                    Diagnostic diag{sourceFile, token, Fmt(Err(Syn0088), token.ctext())};
                    Diagnostic note{Hlp(Hlp0024), DiagnosticLevel::Help};
                    return Report::report(diag, &note);
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
                return Report::report({sourceFile, token, Fmt(Err(Syn0066), token.ctext())});

            Diagnostic diag{sourceFile, token, Fmt(Err(Syn0066), token.ctext())};
            return Report::report(diag);
        }

        if (token.id == TokenId::SymComma)
        {
            // Special error if inside a function call parameter
            auto callParam = node->findParent(AstNodeKind::FuncCallParam);
            if (callParam)
            {
                Diagnostic diag{sourceFile, token, Fmt(Err(Syn0096), token.ctext())};
                return Report::report(diag);
            }
            else
            {
                return error(token, Fmt(Err(Syn0096), token.ctext()));
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
            SWAG_VERIFY(token.id == TokenId::SymAsterisk, error(token, Err(Syn0061)));
        }
    }

    // Pointers
    if (token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex)
    {
        while (token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex)
        {
            if (node->ptrCount == AstTypeExpression::MAX_PTR_COUNT)
                return error(token, Fmt(Err(Syn0133), AstTypeExpression::MAX_PTR_COUNT));
            node->ptrFlags[node->ptrCount] = isPtrConst ? AstTypeExpression::PTR_CONST : 0;
            if (token.id == TokenId::SymCircumflex)
                node->ptrFlags[node->ptrCount] |= AstTypeExpression::PTR_ARITMETIC;
            SWAG_CHECK(eatToken());
            isPtrConst = false;

            if (token.id == TokenId::KwdConst)
            {
                SWAG_CHECK(eatToken());
                SWAG_VERIFY(token.id == TokenId::SymAsterisk || token.id == TokenId::SymCircumflex || token.id == TokenId::SymAmpersand, error(token, Err(Syn0061)));
                isPtrConst = true;
            }

            node->ptrCount++;
        }
    }

    // This is a @typeof
    if (token.id == TokenId::IntrinsicMakeType)
    {
        SWAG_CHECK(doIdentifierRef(node, &node->identifier));
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
        alias->allocateExtension(ExtensionKind::Semantic);
        alias->extension->semantic->semanticBeforeFct = SemanticJob::resolveTypeAliasBefore;
        alias->semanticFct                            = SemanticJob::resolveTypeAlias;
        alias->resolvedSymbolName                     = currentScope->symTable.registerSymbolName(&context, alias, SymbolKind::TypeAlias);
        node->identifier                              = Ast::newIdentifierRef(sourceFile, alias->token.text, node, this);
        SWAG_CHECK(doTypeExpressionLambdaClosure(alias));

        node->identifier->allocateExtension(ExtensionKind::ExportNode);
        node->identifier->extension->misc->exportNode = alias->childs.front();
        return true;
    }

    // Specific error messages
    if (parent && parent->kind == AstNodeKind::TupleContent)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Syn0067), token.ctext())};
        return Report::report(diag);
    }

    if (token.id == TokenId::SymLeftParen)
    {
        Diagnostic diag{sourceFile, token, Fmt(Err(Syn0066), token.ctext())};
        Diagnostic note{sourceFile, token, Hlp(Hlp0004), DiagnosticLevel::Help};
        return Report::report(diag, &note);
    }

    if (Tokenizer::isSymbol(token.id))
        return error(token, Fmt(Err(Syn0172), token.ctext()));

    // Generic error
    return error(token, Fmt(Err(Syn0066), token.ctext()));
}

bool SyntaxJob::doCast(AstNode* parent, AstNode** result)
{
    auto node         = Ast::newNode<AstCast>(this, AstNodeKind::Cast, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveExplicitCast;
    if (result)
        *result = node;
    SWAG_CHECK(eatToken());

    // Cast modifiers
    uint32_t mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, mdfFlags));
    if (mdfFlags & MODIFIER_SAFE)
    {
        node->specFlags |= AST_SPEC_CAST_SAFE;
        node->attributeFlags |= ATTRIBUTE_SAFETY_OVERFLOW_OFF;
    }

    if (mdfFlags & MODIFIER_BIT)
    {
        node->specFlags |= AST_SPEC_CAST_BIT;
        node->semanticFct = SemanticJob::resolveExplicitBitCast;
    }

    if (mdfFlags & MODIFIER_BIT && mdfFlags & MODIFIER_SAFE)
    {
        return error(node, Err(Syn0186));
    }

    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after 'cast'"));
    SWAG_CHECK(doTypeExpression(node));
    SWAG_CHECK(eatToken(TokenId::SymRightParen, "after the type expression"));

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
