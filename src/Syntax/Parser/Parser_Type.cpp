#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

bool Parser::doLambdaClosureType(AstNode* parent, AstNode** result, bool inTypeVarDecl)
{
    const auto node   = Ast::newNode<AstTypeLambda>(AstNodeKind::TypeLambda, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveTypeLambdaClosure;

    if (inTypeVarDecl)
    {
        const auto             newScope = Ast::newScope(node, node->token.text, ScopeKind::TypeLambda, currentScope);
        ParserPushScope        scoped(this, newScope);
        ParserPushAstNodeFlags sf(this, AST_IN_TYPE_VAR_DECLARATION);
        SWAG_CHECK(doLambdaClosureType(node, inTypeVarDecl));
    }
    else
    {
        SWAG_CHECK(doLambdaClosureType(node, inTypeVarDecl));
    }

    return true;
}

bool Parser::doLambdaClosureParameters(AstTypeLambda* node, bool inTypeVarDecl, AstNode* params)
{
    if (!params)
    {
        params           = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, this, node);
        node->parameters = params;
    }

    AstVarDecl* lastParameter = nullptr;
    bool        lastWasAlone  = false;
    bool        thisIsAType   = false;
    while (true)
    {
        bool curIsAlone = true;
        bool isConst    = false;
        bool forceType  = false;

        if (tokenParse.is(TokenId::CompilerType))
        {
            forceType   = true;
            thisIsAType = true;
            curIsAlone  = false;
            SWAG_CHECK(eatToken());
            SWAG_CHECK(checkIsIdentifier(tokenParse, formErr(Err0527, tokenParse.token.c_str())));
        }

        Token constToken;
        if (tokenParse.is(TokenId::KwdConst))
        {
            curIsAlone = false;
            isConst    = true;
            constToken = tokenParse.token;
            SWAG_CHECK(eatToken());
        }

        // Accept a parameter name
        AstNode* namedParam = nullptr;
        if (!forceType && tokenParse.is(TokenId::Identifier))
        {
            auto tokenName = tokenParse;
            tokenizer.saveState(tokenParse);
            SWAG_CHECK(eatToken());
            if (tokenParse.isNot(TokenId::SymColon))
            {
                tokenizer.restoreState(tokenParse);
            }
            else if (tokenName.token.text == g_LangSpec->name_self)
            {
                Diagnostic err(sourceFile, tokenParse.token, toErr(Err0701));
                return context->report(err);
            }
            else
            {
                SWAG_VERIFY(inTypeVarDecl, error(tokenName.token, toErr(Err0690)));
                SWAG_VERIFY(!isConst, error(constToken, toErr(Err0662)));

                namedParam        = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, this, nullptr);
                namedParam->token = tokenName.token;
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
        if (tokenParse.token.text == g_LangSpec->name_self)
        {
            curIsAlone = false;
            SWAG_VERIFY(currentStructScope, error(tokenParse.token, toErr(Err0470)));
            typeExpr = Ast::newTypeExpression(nullptr, params);
            typeExpr->typeFlags.add(isConst ? TYPEFLAG_IS_CONST : 0);
            typeExpr->typeFlags.add(TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE);
            typeExpr->token.endLocation = tokenParse.token.endLocation;
            SWAG_CHECK(eatToken());
            typeExpr->identifier = Ast::newIdentifierRef(currentStructScope->name, this, typeExpr);
            if (tokenParse.is(TokenId::SymEqual))
                return error(tokenParse.token, toErr(Err0252));
        }
        // ...
        else if (tokenParse.is(TokenId::SymDotDotDot))
        {
            curIsAlone                  = false;
            typeExpr                    = Ast::newTypeExpression(nullptr, params);
            typeExpr->typeFromLiteral   = g_TypeMgr->typeInfoVariadic;
            typeExpr->token.endLocation = tokenParse.token.endLocation;
            SWAG_CHECK(eatToken());
            if (tokenParse.is(TokenId::SymEqual))
                return error(tokenParse.token, toErr(Err0678));
        }
        // cvarargs
        else if (tokenParse.is(TokenId::KwdCVarArgs))
        {
            curIsAlone                  = false;
            typeExpr                    = Ast::newTypeExpression(nullptr, params);
            typeExpr->typeFromLiteral   = g_TypeMgr->typeInfoCVariadic;
            typeExpr->token.endLocation = tokenParse.token.endLocation;
            SWAG_CHECK(eatToken());
            if (tokenParse.is(TokenId::SymEqual))
                return error(tokenParse.token, toErr(Err0678));
        }
        else
        {
            SWAG_CHECK(doTypeExpression(params, EXPR_FLAG_NONE, reinterpret_cast<AstNode**>(&typeExpr)));
            typeExpr->typeFlags.add(isConst ? TYPEFLAG_IS_CONST : 0);

            // type...
            if (tokenParse.is(TokenId::SymDotDotDot))
            {
                curIsAlone = false;
                Ast::removeFromParent(typeExpr);
                auto newTypeExpression               = Ast::newTypeExpression(nullptr, params);
                newTypeExpression->typeFromLiteral   = g_TypeMgr->typeInfoTypedVariadic;
                newTypeExpression->token.endLocation = tokenParse.token.endLocation;
                SWAG_CHECK(eatToken());
                Ast::addChildBack(newTypeExpression, typeExpr);
                if (tokenParse.is(TokenId::SymEqual))
                    return error(tokenParse.token, toErr(Err0678));
                typeExpr = newTypeExpression;
            }
            else if (typeExpr->identifier && !testIsSingleIdentifier(typeExpr->identifier))
            {
                thisIsAType = true;
                curIsAlone  = false;
            }
        }

        SWAG_VERIFY(tokenParse.isNot(TokenId::SymEqual) || inTypeVarDecl, error(tokenParse.token, toErr(Err0253)));

        // If we are in a type declaration, generate a variable and not just a type
        if (inTypeVarDecl)
        {
            auto        nameVar = namedParam ? namedParam->token.text : form("__%d", g_UniqueID.fetch_add(1));
            AstVarDecl* varDecl;
            {
                ParserPushFreezeFormat ff(this);
                varDecl = Ast::newVarDecl(nameVar, this, params, AstNodeKind::FuncDeclParam);
            }
            varDecl->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

            Ast::removeFromParent(typeExpr);
            Ast::addChildBack(varDecl, typeExpr);
            varDecl->type = typeExpr;
            if (namedParam)
                varDecl->inheritTokenLocation(namedParam->token);
            else
                varDecl->inheritTokenLocation(typeExpr->token);

            if (tokenParse.is(TokenId::SymEqual))
            {
                thisIsAType = false;
                curIsAlone  = false;
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doAssignmentExpression(varDecl, &varDecl->assignment));

                // Used to automatically solve enums
                typeExpr->allocateExtension(ExtensionKind::Semantic);
                typeExpr->extSemantic()->semanticAfterFct = Semantic::resolveVarDeclAfterType;

                // If we did not have specified a name, then this was not a type, but a name
                // ex: func(x = 1)
                if (!namedParam)
                {
                    varDecl->token.text = varDecl->type->token.text;
                    Ast::removeFromParent(varDecl->type);
                    varDecl->type = nullptr;
                }
            }
            else if (!namedParam)
            {
                varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_PRIVATE_NAME);
            }

            if (lastWasAlone && !curIsAlone && !thisIsAType)
            {
                Token tokenAmb         = varDecl->token;
                tokenAmb.startLocation = lastParameter->token.startLocation;
                tokenAmb.endLocation   = tokenParse.token.startLocation;

                Diagnostic err{sourceFile, tokenAmb, toErr(Err0022)};
                auto       note = Diagnostic::note(lastParameter, formNte(Nte0008, lastParameter->type->token.c_str()));
                note->hint      = formNte(Nte0189, lastParameter->type->token.c_str());
                err.addNote(note);
                return context->report(err);
            }

            lastWasAlone  = curIsAlone;
            lastParameter = varDecl;
        }
        else if (namedParam)
        {
            typeExpr->addExtraPointer(ExtraPointerKind::IsNamed, namedParam);
        }

        if (forceType)
            typeExpr->addSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE);

        if (tokenParse.isNot(TokenId::SymComma))
            break;

        SWAG_CHECK(eatToken());
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse.token, toErr(Err0131)));
    }

    FormatAst::inheritFormatAfter(this, params, &tokenParse);
    return true;
}

bool Parser::doLambdaClosureType(AstTypeLambda* node, bool inTypeVarDecl)
{
    SWAG_ASSERT(tokenParse.token.id == TokenId::KwdClosure || tokenParse.token.id == TokenId::KwdFunc);

    AstTypeExpression* firstAddedType = nullptr;
    AstNode*           params         = nullptr;

    if (tokenParse.token.id == TokenId::KwdFunc)
    {
        node->kind = AstNodeKind::TypeLambda;
        SWAG_CHECK(eatToken());
    }
    else if (tokenParse.token.id == TokenId::KwdClosure)
    {
        node->kind = AstNodeKind::TypeClosure;
        SWAG_CHECK(eatToken());

        // :ClosureForceFirstParam
        // A closure always has at least one parameter : the capture context
        params                   = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, this, node);
        node->parameters         = params;
        firstAddedType           = Ast::newTypeExpression(nullptr, params);
        firstAddedType->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        firstAddedType->addAstFlag(AST_NO_SEMANTIC | AST_GENERATED);

        // If we are in a type declaration, then this must be a FuncDeclParam and not a TypeExpression
        if (inTypeVarDecl)
        {
            Utf8 nameVar;
            if (firstAddedType->typeFlags.has(TYPEFLAG_IS_SELF))
            {
                firstAddedType->token.text = g_LangSpec->name_self;
                nameVar                    = g_LangSpec->name_self;
            }
            else
            {
                nameVar = form("__%d", g_UniqueID.fetch_add(1));
            }

            const auto param = Ast::newVarDecl(nameVar, this, params, AstNodeKind::FuncDeclParam);
            if (firstAddedType->typeFlags.has(TYPEFLAG_IS_SELF))
                param->addSpecFlag(AstVarDecl::SPEC_FLAG_GENERATED_SELF);

            param->addExtraPointer(ExtraPointerKind::ExportNode, firstAddedType);
            param->addAstFlag(AST_GENERATED | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

            Ast::removeFromParent(firstAddedType);
            Ast::addChildBack(param, firstAddedType);
            param->type = firstAddedType;
            param->inheritTokenLocation(firstAddedType->token);
        }
    }

    // Parameters
    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "to start the list of parameters"));
    if (tokenParse.isNot(TokenId::SymRightParen))
        SWAG_CHECK(doLambdaClosureParameters(node, inTypeVarDecl, params));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the lambda parameters"));

    // Return type
    if (tokenParse.is(TokenId::SymMinusGreat))
    {
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &node->returnType));
    }

    // End
    if (tokenParse.is(TokenId::KwdThrow))
    {
        SWAG_CHECK(eatToken());
        node->addSpecFlag(AstTypeLambda::SPEC_FLAG_CAN_THROW);
    }

    return true;
}

bool Parser::doAnonymousStruct(AstNode* parent, AstNode** result, ExprFlags exprFlags, bool typeSpecified, bool isUnion)
{
    const auto structNode = Ast::newStructDecl(this, parent);
    structNode->addAstFlag(AST_INTERNAL | AST_GENERATED | AST_GENERATED_EXCEPT_EXPORT);
    structNode->originalParent = parent;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticBeforeFct = Semantic::preResolveGeneratedStruct;
    structNode->addSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS);

    if (typeSpecified)
        structNode->addSpecFlag(AstStruct::SPEC_FLAG_SPECIFIED_TYPE);
    if (isUnion)
        structNode->addSpecFlag(AstStruct::SPEC_FLAG_UNION);
    if (exprFlags.has(EXPR_FLAG_IN_GENERIC_PARAMS))
        structNode->addSpecFlag(AstStruct::SPEC_FLAG_GENERIC_PARAM);

    const auto contentNode = Ast::newNode<AstNode>(AstNodeKind::StructContent, this, structNode);
    structNode->content    = contentNode;
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;

    // Name
    Utf8 name = sourceFile->scopeFile->name + "_tpl3_";
    name += form("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = std::move(name);

    // :SubDeclParent
    auto newParent = parent;
    while (newParent != sourceFile->astRoot && !newParent->hasAstFlag(AST_GLOBAL_NODE) && newParent->isNot(AstNodeKind::Namespace))
    {
        newParent = newParent->parent;
        SWAG_ASSERT(newParent);
    }

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->hasFlag(FILE_FROM_TESTS))
        rootScope = sourceFile->scopeFile;
    else
        rootScope = newParent->ownerScope;
    structNode->content->addAlternativeScope(currentScope);
    SWAG_ASSERT(parent);
    structNode->addExtraPointer(ExtraPointerKind::AlternativeNode, parent);

    const auto newScope = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope   = newScope;

    {
        ParserPushSelfStructScope sf(this, parent->ownerStructScope);
        ParserPushScope           sc(this, structNode->scope);
        ParserPushStructScope     ss(this, structNode->scope);

        const auto startLoc = tokenParse.token.startLocation;
        SWAG_CHECK(eatToken(TokenId::SymLeftCurly, "to start the [[tuple]] body"));
        while (tokenParse.isNot(TokenId::SymRightCurly) && tokenParse.isNot(TokenId::EndOfFile))
            SWAG_CHECK(doStructBody(contentNode, SyntaxStructType::Struct, &dummyResult));
        SWAG_CHECK(eatFormat(contentNode));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightCurly, startLoc, "to end the [[tuple]] body"));
    }

    // Reference to that struct
    const auto idRef = Ast::newIdentifierRef(structNode->token.text, this, parent);
    *result          = idRef;
    idRef->addExtraPointer(ExtraPointerKind::ExportNode, structNode);
    idRef->addAlternativeScope(sourceFile->scopeFile);

    idRef->lastChild()->addAstFlag(AST_GENERATED);
    Ast::removeFromParent(structNode);
    Ast::addChildBack(newParent, structNode);
    structNode->inheritOwners(newParent);

    const auto typeInfo  = makeType<TypeInfoStruct>();
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;
    typeInfo->addFlag(TYPEINFO_STRUCT_IS_TUPLE);
    structNode->typeInfo   = typeInfo;
    structNode->ownerScope = rootScope;
    rootScope->symTable.registerSymbolName(context, structNode, SymbolKind::Struct);

    Ast::visit(structNode->content, [&](AstNode* n) {
        n->inheritOwners(structNode);
        n->ownerStructScope = newScope;
        n->ownerScope       = newScope;
    });

    return true;
}

bool Parser::doSingleTypeExpression(AstTypeExpression* node, ExprFlags exprFlags)
{
    const bool inTypeVarDecl = exprFlags.has(EXPR_FLAG_IN_VAR_DECL);

    switch (tokenParse.token.id)
    {
        case TokenId::IntrinsicDeclType:
            SWAG_CHECK(doIdentifierRef(node, &node->identifier));
            return true;

        case TokenId::NativeType:
            node->literalType       = tokenParse.literalType;
            node->token.endLocation = tokenParse.token.endLocation;
            SWAG_CHECK(eatToken());
            return true;

        case TokenId::Identifier:
            SWAG_CHECK(doIdentifierRef(node, &node->identifier, IDENTIFIER_TYPE_DECL | IDENTIFIER_NO_ARRAY));
            if (inTypeVarDecl)
                node->identifier->lastChild()->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);
            node->token.endLocation = node->identifier->token.endLocation;
            return true;

        case TokenId::KwdStruct:
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAnonymousStruct(node, &node->identifier, exprFlags, true, false));
            return true;

        case TokenId::KwdUnion:
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAnonymousStruct(node, &node->identifier, exprFlags, true, true));
            return true;

        case TokenId::SymLeftCurly:
            SWAG_CHECK(doAnonymousStruct(node, &node->identifier, exprFlags, false, false));
            return true;

        case TokenId::KwdFunc:
        case TokenId::KwdClosure:
        case TokenId::KwdMethod:
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdMethod), context->report({sourceFile, tokenParse.token, toErr(Err0668)}));
            SWAG_CHECK(doLambdaClosureType(node, &node->identifier, inTypeVarDecl));
            return true;
    }

    // Specific error messages
    if (node->parent && node->parent->is(AstNodeKind::TupleContent))
    {
        const Diagnostic err{sourceFile, tokenParse.token, formErr(Err0401, tokenParse.token.c_str())};
        return context->report(err);
    }

    Diagnostic err{sourceFile, tokenParse.token, formErr(Err0400, tokenParse.token.c_str())};

    if (tokenParse.is(TokenId::SymLeftParen))
        err.addNote(toNte(Nte0084));
    else if (tokenParse.is(TokenId::SymDotDotDot))
        err.addNote(toNte(Nte0138));
    else if (Tokenizer::isKeyword(tokenParse.token.id))
        err.addNote(formNte(Nte0125, tokenParse.token.c_str()));
    else if (tokenParse.is(TokenId::IntrinsicTypeOf) || tokenParse.is(TokenId::IntrinsicKindOf))
        err.addNote(toNte(Nte0085));

    return context->report(err);
}

bool Parser::doSubTypeExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    const bool inTypeVarDecl = exprFlags.has(EXPR_FLAG_IN_VAR_DECL);

    const auto node = Ast::newTypeExpression(this, parent);
    *result         = node;
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN);

    // Const keyword
    if (tokenParse.is(TokenId::KwdConst))
    {
        node->locConst = tokenParse.token.startLocation;
        node->typeFlags.add(TYPEFLAG_IS_CONST | TYPEFLAG_HAS_LOC_CONST);
        SWAG_CHECK(eatToken());

        if (tokenParse.is(TokenId::SymAmpersandAmpersand))
        {
            const Diagnostic err{sourceFile, tokenParse.token, toErr(Err0247)};
            return context->report(err);
        }
    }

    // MoveRef
    if (tokenParse.is(TokenId::SymAmpersandAmpersand))
    {
        node->typeFlags.add(TYPEFLAG_IS_SUB_TYPE);
        node->typeFlags.add(TYPEFLAG_IS_REF);
        node->typeFlags.add(TYPEFLAG_IS_MOVE_REF);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    // Reference
    if (tokenParse.is(TokenId::SymAmpersand))
    {
        node->typeFlags.add(TYPEFLAG_IS_SUB_TYPE);
        node->typeFlags.add(TYPEFLAG_IS_REF);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    // Array or slice of something
    if (tokenParse.is(TokenId::SymLeftSquare))
    {
        const auto leftSquareToken = tokenParse;

        SWAG_CHECK(eatToken());
        while (true)
        {
            // Slice
            if (tokenParse.is(TokenId::SymDotDot))
            {
                node->typeFlags.add(TYPEFLAG_IS_SLICE);
                SWAG_CHECK(eatToken());
                break;
            }

            node->typeFlags.add(TYPEFLAG_IS_ARRAY);

            // Size of array can be nothing
            if (tokenParse.is(TokenId::SymRightSquare))
            {
                node->arrayDim = UINT8_MAX;
                break;
            }

            if (node->arrayDim == 254)
                return error(tokenParse.token, toErr(Err0603));
            node->arrayDim++;
            SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));
            if (tokenParse.isNot(TokenId::SymComma))
                break;
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightSquare), error(tokenParse.token, toErr(Err0122)));
        }

        auto rightSquareToken = tokenParse;
        SWAG_CHECK(eatCloseToken(TokenId::SymRightSquare, leftSquareToken.token.startLocation));
        if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
        {
            if (exprFlags.has(EXPR_FLAG_TYPE_EXPR))
            {
                if (inTypeVarDecl)
                    return context->report({sourceFile, rightSquareToken.token, toErr(Err0692)});
                const Diagnostic err{sourceFile, rightSquareToken.token, toErr(Err0692)};
                return context->report(err);
            }
            return error(rightSquareToken.token, toErr(Err0692));
        }

        if (tokenParse.is(TokenId::SymComma))
        {
            const Diagnostic err{sourceFile, tokenParse.token, formErr(Err0402, tokenParse.token.c_str())};
            return context->report(err);
        }

        node->typeFlags.add(TYPEFLAG_IS_SUB_TYPE);
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    // Pointers
    if (tokenParse.is(TokenId::SymAsterisk) || tokenParse.is(TokenId::SymCircumflex))
    {
        node->typeFlags.add(TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE);
        if (tokenParse.is(TokenId::SymCircumflex))
            node->typeFlags.add(TYPEFLAG_IS_PTR_ARITHMETIC);
        SWAG_CHECK(eatToken());
        SWAG_CHECK(doSubTypeExpression(node, exprFlags, &dummyResult));
        return true;
    }

    SWAG_CHECK(doSingleTypeExpression(node, exprFlags));
    return true;
}

bool Parser::doTypeExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    // Code
    if (tokenParse.is(TokenId::KwdCode))
    {
        const auto node = Ast::newTypeExpression(this, parent);
        *result         = node;
        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
        node->typeInfo = g_TypeMgr->typeInfoCode;
        node->typeFlags.add(TYPEFLAG_IS_CODE);
        SWAG_CHECK(eatToken());
        return true;
    }

    // retval
    if (tokenParse.is(TokenId::KwdRetVal))
    {
        const auto node   = Ast::newTypeExpression(this, parent);
        *result           = node;
        node->semanticFct = Semantic::resolveRetVal;
        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
        node->typeFlags.add(TYPEFLAG_IS_RETVAL | TYPEFLAG_IS_RETVAL_TYPE);

        // retval type can be followed by structure initializer, like a normal struct
        // In that case, we want the identifier pipeline to be called on it (to check
        // parameters like a normal struct).
        // So we create an identifier, that will be matched with the type alias automatically
        // created in the function.
        SWAG_CHECK(eatToken());
        if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE) && tokenParse.is(TokenId::SymLeftCurly))
        {
            node->identifier = Ast::newIdentifierRef(g_LangSpec->name_retval, this, node);
            const auto id    = castAst<AstIdentifier>(node->identifier->lastChild(), AstNodeKind::Identifier);
            SWAG_CHECK(eatToken());
            SWAG_CHECK(doFuncCallParameters(id, &id->callParameters, TokenId::SymRightCurly));
            id->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);
            id->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
        }

        return true;
    }

    // This is a lambda
    if (tokenParse.is(TokenId::KwdFunc) || tokenParse.is(TokenId::KwdClosure) || tokenParse.is(TokenId::KwdMethod))
    {
        SWAG_VERIFY(tokenParse.isNot(TokenId::KwdMethod), context->report({sourceFile, tokenParse.token, toErr(Err0668)}));
        SWAG_CHECK(doLambdaClosureType(parent, result, exprFlags.has(EXPR_FLAG_IN_VAR_DECL)));
        return true;
    }

    SWAG_CHECK(doSubTypeExpression(parent, exprFlags, result));
    return true;
}

bool Parser::doCast(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstCast>(AstNodeKind::Cast, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveExplicitCast;
    SWAG_CHECK(eatToken());

    // Cast modifiers
    ModifierFlags mdfFlags = 0;
    SWAG_CHECK(doModifiers(node->token, node->token.id, mdfFlags));
    if (mdfFlags.has(MODIFIER_OVERFLOW))
    {
        node->addSpecFlag(AstCast::SPEC_FLAG_OVERFLOW);
        node->addAttribute(ATTRIBUTE_CAN_OVERFLOW_ON);
    }

    if (mdfFlags.has(MODIFIER_BIT))
    {
        node->addSpecFlag(AstCast::SPEC_FLAG_BIT);
        node->semanticFct = Semantic::resolveExplicitBitCast;
    }

    if (mdfFlags.has(MODIFIER_BIT) && mdfFlags.has(MODIFIER_OVERFLOW))
    {
        return error(node, formErr(Err0053, "bit", "over"));
    }

    if (mdfFlags.has(MODIFIER_UN_CONST))
    {
        node->addSpecFlag(AstCast::SPEC_FLAG_UN_CONST);
    }

    const auto startLoc = tokenParse.token.startLocation;
    SWAG_CHECK(eatToken(TokenId::SymLeftParen, "after [[cast]]"));
    SWAG_CHECK(doTypeExpression(node, EXPR_FLAG_NONE, &dummyResult));
    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc, "to end the [[cast]] type expression"));

    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}

bool Parser::doAutoCast(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstCast>(AstNodeKind::AutoCast, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveExplicitAutoCast;

    SWAG_CHECK(eatToken());
    SWAG_CHECK(doUnaryExpression(node, EXPR_FLAG_NONE, &dummyResult));
    return true;
}
