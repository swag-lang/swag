#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool Parser::doGenericFuncCallParameters(AstNode* parent, AstFuncCallParams** result)
{
    const auto callParams = Ast::newFuncCallGenParams(this, parent);
    *result               = callParams;

    bool        multi    = false;
    const auto& startLoc = tokenParse.token.startLocation;
    if (tokenParse.is(TokenId::SymLeftParen))
    {
        multi = true;
        callParams->addAstFlag(AST_EXPR_IN_PARENTS);
        SWAG_CHECK(eatToken());
    }

    while (tokenParse.isNot(TokenId::SymRightParen))
    {
        const auto param = Ast::newFuncCallParam(this, callParams);
        param->token     = tokenParse.token;
        switch (tokenParse.token.id)
        {
            case TokenId::Identifier:
                SWAG_CHECK(doIdentifierRef(param, &dummyResult, IDENTIFIER_NO_FCT_PARAMS));
                break;

            case TokenId::KwdTrue:
            case TokenId::KwdFalse:
            case TokenId::KwdNull:
            case TokenId::CompilerFile:
            case TokenId::CompilerModule:
            case TokenId::CompilerLine:
            case TokenId::CompilerBuildVersion:
            case TokenId::CompilerBuildRevision:
            case TokenId::CompilerBuildNum:
            case TokenId::LiteralNumber:
            case TokenId::LiteralString:
            case TokenId::LiteralCharacter:
                SWAG_CHECK(doLiteral(param, &dummyResult));
                break;

            case TokenId::CompilerLocation:
                SWAG_CHECK(doCompilerSpecialValue(param, &dummyResult));
                break;

            case TokenId::SymLeftSquare:
            {
                // This is ambiguous. Can be a literal array or an array type.
                // If parameters are inside parentheses, then this means that we can differentiate between the 2 cases
                // without the need of #type, as what follows a literal should be another parameter (,) or the closing parenthesis.
                // And this is a good idea to write Arr'([2] s32) instead of Arr'[2] s32 anyway. So this should remove some ambiguities.
                tokenizer.saveState(tokenParse);
                SWAG_CHECK(doExpressionListArray(param, &dummyResult));
                if (multi && tokenParse.isNot(TokenId::SymComma) && tokenParse.isNot(TokenId::SymRightParen))
                {
                    tokenizer.restoreState(tokenParse);
                    Ast::removeFromParent(param->lastChild());
                    SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_IN_GENERIC_PARAMS, &dummyResult));
                }
                break;
            }

            case TokenId::CompilerType:
            {
                SWAG_CHECK(eatToken());
                AstNode* resNode;
                SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_IN_GENERIC_PARAMS, &resNode));
                resNode->addSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE);
                break;
            }

            default:
                SWAG_CHECK(doTypeExpression(param, EXPR_FLAG_IN_GENERIC_PARAMS, &dummyResult));
                break;
        }

        if (!multi)
            break;
        if (tokenParse.is(TokenId::SymRightParen))
            break;
        SWAG_CHECK(eatToken(TokenId::SymComma, "to define another argument or ')' to end the list"));
    }

    if (multi)
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));

    return true;
}

bool Parser::doFuncCallArguments(AstNode* parent, AstFuncCallParams** result, TokenId closeToken)
{
    auto callParams = Ast::newFuncCallParams(this, parent);
    *result         = callParams;

    bool forAttrUse = false;
    if (callParams->getParent(3)->is(AstNodeKind::AttrUse))
        forAttrUse = true;

    // Capturing
    if (closeToken == TokenId::SymRightParen && tokenParse.is(TokenId::SymVertical))
    {
        SWAG_CHECK(eatToken());
        while (tokenParse.isNot(TokenId::SymVertical))
        {
            SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0643)));
            callParams->aliasNames.push_back(tokenParse.token);
            SWAG_CHECK(eatToken());
            if (tokenParse.is(TokenId::SymVertical))
                break;
            SWAG_CHECK(eatToken(TokenId::SymComma, "to declare another alias variable name"));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymVertical), error(tokenParse, toErr(Err0095)));
        }

        SWAG_CHECK(eatToken());
    }

    while (tokenParse.token.isNot(closeToken))
    {
        while (true)
        {
            const auto param   = Ast::newNode<AstFuncCallParam>(AstNodeKind::FuncCallParam, this, callParams);
            param->semanticFct = Semantic::resolveFuncCallParam;
            param->token       = tokenParse.token;
            AstNode* paramExpression;

            SWAG_CHECK(doExpression(param, EXPR_FLAG_PARAMETER | EXPR_FLAG_IN_CALL | EXPR_FLAG_NAMED_PARAM, &paramExpression));
            Ast::removeFromParent(paramExpression);

            // Name
            if (tokenParse.is(TokenId::SymColon))
            {
                if (paramExpression->isNot(AstNodeKind::IdentifierRef) || paramExpression->childCount() != 1)
                    return context->report({paramExpression, toErr(Err0645)});
                param->addExtraPointer(ExtraPointerKind::IsNamed, paramExpression->firstChild());
                param->allocateExtension(ExtensionKind::Owner);
                param->extOwner()->nodesToFree.push_back(paramExpression);
                SWAG_CHECK(eatToken());
                SWAG_CHECK(doExpression(param, EXPR_FLAG_PARAMETER, &dummyResult));
            }
            else
            {
                Ast::addChildBack(param, paramExpression);
            }

            param->token.endLocation = tokenParse.token.startLocation;
            if (tokenParse.is(closeToken))
                break;

            if (forAttrUse && tokenParse.is(TokenId::SymRightSquare))
                return error(tokenParse, toErr(Err0421));

            {
                PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [] { return toNte(Nte0042); });
                if (callParams->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
                    SWAG_CHECK(eatToken(TokenId::SymComma, "in the [[struct]] initialization arguments"));
                else if (forAttrUse)
                    SWAG_CHECK(eatToken(TokenId::SymComma, "in the attribute arguments"));
                else
                    SWAG_CHECK(eatToken(TokenId::SymComma, "in the function call arguments"));
            }

            // Accept ending comma in struct initialization
            if (closeToken == TokenId::SymRightCurly && tokenParse.is(closeToken))
                break;

            SWAG_VERIFY(tokenParse.isNot(closeToken), error(tokenParse, toErr(Err0096)));
        }
    }

    FormatAst::inheritFormatAfter(this, callParams, &tokenParse);
    if (callParams->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        SWAG_CHECK(eatToken(closeToken, "to close the struct initialization arguments"));
    else if (forAttrUse)
        SWAG_CHECK(eatToken(closeToken, "to close the attribute arguments"));
    else
        SWAG_CHECK(eatToken(closeToken, "to close the function call arguments"));

    return true;
}

bool Parser::doFuncDeclParameterSelf(AstVarDecl* paramNode)
{
    bool isConst = false;
    if (tokenParse.is(TokenId::KwdConst))
    {
        const auto constToken = tokenParse;
        isConst               = true;
        SWAG_CHECK(eatToken());

        if (tokenParse.isNot(TokenId::Identifier) || tokenParse.token.isNot(g_LangSpec->name_self))
        {
            const Diagnostic err{sourceFile, tokenParse, toErr(Err0318)};
            return context->report(err);
        }

        paramNode->token.text = g_LangSpec->name_self;
    }

    SWAG_VERIFY(paramNode->ownerStructScope, error(tokenParse, toErr(Err0340)));
    SWAG_CHECK(eatToken());

    // For an enum, 'self' is replaced with the type itself, not a pointer to the type like for a struct
    if (paramNode->ownerStructScope->is(ScopeKind::Enum))
    {
        const auto typeNode = Ast::newTypeExpression(nullptr, paramNode);
        typeNode->typeFlags.add(TYPEFLAG_IS_SELF);
        if (paramNode->hasAstFlag(AST_DECL_USING))
            typeNode->typeFlags.add(TYPEFLAG_HAS_USING);
        typeNode->identifier = Ast::newIdentifierRef(paramNode->ownerStructScope->name, this, typeNode);
        paramNode->type      = typeNode;
    }
    else
    {
        SWAG_VERIFY(paramNode->ownerStructScope->is(ScopeKind::Struct), error(tokenParse, toErr(Err0340)));
        const auto typeNode = Ast::newTypeExpression(nullptr, paramNode);
        typeNode->typeFlags.add(isConst ? TYPEFLAG_IS_CONST : 0);
        typeNode->typeFlags.add(TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE);
        if (paramNode->hasAstFlag(AST_DECL_USING))
            typeNode->typeFlags.add(TYPEFLAG_HAS_USING);
        typeNode->identifier = Ast::newIdentifierRef(paramNode->ownerStructScope->name, this, typeNode);
        paramNode->type      = typeNode;
    }

    if (tokenParse.is(TokenId::SymEqual))
    {
        const Diagnostic err(paramNode, tokenParse.token, toErr(Err0379));
        return context->report(err);
    }

    if (tokenParse.is(TokenId::SymColon))
    {
        const Diagnostic err(paramNode, tokenParse.token, toErr(Err0684));
        return context->report(err);
    }

    return true;
}

bool Parser::doFuncDeclParameter(AstNode* parent, bool acceptMissingType, bool* hasMissingType)
{
    // Attribute
    AstAttrUse* attrUse = nullptr;
    if (tokenParse.is(TokenId::SymAttrStart))
    {
        SWAG_CHECK(doAttrUse(nullptr, reinterpret_cast<AstNode**>(&attrUse)));
    }

    const auto paramNode = Ast::newVarDecl("", this, parent, AstNodeKind::FuncDeclParam);

    if (tokenParse.is(TokenId::SymDotDotDot) || tokenParse.is(TokenId::NativeType))
        return context->report({sourceFile, tokenParse.token, formErr(Err0460, tokenParse.token.c_str())});

    // Using variable
    if (tokenParse.is(TokenId::KwdUsing))
    {
        SWAG_CHECK(eatToken());
        paramNode->addAstFlag(AST_DECL_USING);
    }

    // :QuestionAsParam
    // Unused parameter
    Vector<Token> unnamedTokens;
    if (tokenParse.is(TokenId::SymQuestion) && acceptMissingType)
    {
        tokenParse.token.id   = TokenId::Identifier;
        tokenParse.token.text = form("__%d", g_UniqueID.fetch_add(1));
        paramNode->addSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED);
        unnamedTokens.push_back(tokenParse.token);
    }

    if (tokenParse.isNot(TokenId::KwdConst))
        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0673)));
    paramNode->token.text = tokenParse.token.text;

    // 'self'
    if (tokenParse.is(TokenId::KwdConst) || paramNode->token.is(g_LangSpec->name_self))
    {
        SWAG_CHECK(doFuncDeclParameterSelf(paramNode));
    }
    else
    {
        // Multiple declaration
        VectorNative<AstVarDecl*> otherVariables;
        SWAG_CHECK(eatToken());

        if (tokenParse.is(TokenId::SymComma))
        {
            paramNode->multiNames.push_back(paramNode->token.text);
            while (tokenParse.is(TokenId::SymComma))
            {
                SWAG_CHECK(eatToken());
                AstVarDecl* otherVarNode = Ast::newVarDecl(tokenParse.token.text, this, parent, AstNodeKind::FuncDeclParam);
                otherVarNode->addAstFlag(AST_GENERATED);

                // :QuestionAsParam
                if (tokenParse.is(TokenId::SymQuestion) && acceptMissingType)
                {
                    tokenParse.token.id   = TokenId::Identifier;
                    tokenParse.token.text = form("__%d", g_UniqueID.fetch_add(1));
                    otherVarNode->addSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED);
                    unnamedTokens.push_back(tokenParse.token);
                }

                SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse, formErr(Err0100, tokenParse.token.c_str())));
                SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0673)));
                SWAG_CHECK(eatToken());
                otherVariables.push_back(otherVarNode);
                paramNode->multiNames.push_back(otherVarNode->token.text);
            }
        }

        bool hasType       = false;
        bool hasAssignment = false;

        // Type
        if (tokenParse.is(TokenId::SymColon))
        {
            if (unnamedTokens.size() == parent->childCount())
            {
                Diagnostic err{sourceFile, tokenParse, toErr(Err0685)};
                err.addNote(unnamedTokens.front(), toNte(Nte0212));
                for (uint32_t i = 1; i < unnamedTokens.size(); i++)
                    err.addNote(unnamedTokens[i], "");
                return context->report(err);
            }

            hasType = true;

            SWAG_CHECK(eatToken());

            // ...
            if (tokenParse.is(TokenId::SymDotDotDot))
            {
                const auto newTypeExpression         = Ast::newTypeExpression(nullptr, paramNode);
                paramNode->type                      = newTypeExpression;
                newTypeExpression->typeFromLiteral   = g_TypeMgr->typeInfoVariadic;
                newTypeExpression->token.endLocation = tokenParse.token.endLocation;
                SWAG_CHECK(eatToken());
            }
            // cvarargs
            else if (tokenParse.is(TokenId::KwdCVarArgs))
            {
                const auto newTypeExpression         = Ast::newTypeExpression(nullptr, paramNode);
                paramNode->type                      = newTypeExpression;
                newTypeExpression->typeFromLiteral   = g_TypeMgr->typeInfoCVariadic;
                newTypeExpression->token.endLocation = tokenParse.token.endLocation;
                SWAG_CHECK(eatToken());
            }
            else
            {
                AstNode* typeExpression;
                SWAG_CHECK(doTypeExpression(paramNode, EXPR_FLAG_NONE, &typeExpression));

                // type...
                if (tokenParse.is(TokenId::SymDotDotDot))
                {
                    Ast::removeFromParent(typeExpression);
                    const auto newTypeExpression         = Ast::newTypeExpression(nullptr, paramNode);
                    paramNode->type                      = newTypeExpression;
                    newTypeExpression->typeFromLiteral   = g_TypeMgr->typeInfoTypedVariadic;
                    newTypeExpression->token.endLocation = tokenParse.token.endLocation;
                    SWAG_CHECK(eatToken());
                    Ast::addChildBack(paramNode->type, typeExpression);
                    newTypeExpression->token.startLocation = typeExpression->token.startLocation;
                }
                else
                {
                    paramNode->type = typeExpression;
                }
            }
        }

        // Assignment
        if (tokenParse.is(TokenId::SymEqual))
        {
            if (unnamedTokens.size() == parent->childCount())
            {
                Diagnostic err{sourceFile, tokenParse, toErr(Err0354)};
                err.addNote(unnamedTokens.front(), toNte(Nte0172));
                for (uint32_t i = 1; i < unnamedTokens.size(); i++)
                    err.addNote(unnamedTokens[i], "");
                return context->report(err);
            }

            paramNode->assignToken = tokenParse.token;
            hasAssignment          = true;

            SWAG_CHECK(eatToken());
            SWAG_CHECK(doAssignmentExpression(paramNode, &paramNode->assignment));

            // Used to automatically solve enums
            if (paramNode->assignment && paramNode->type)
            {
                paramNode->type->allocateExtension(ExtensionKind::Semantic);
                paramNode->type->extSemantic()->semanticAfterFct = Semantic::resolveVarDeclAfterType;
            }
        }

        if (!hasType && !hasAssignment)
        {
            if (!acceptMissingType)
            {
                Diagnostic err{sourceFile, tokenParse, toErr(Err0461)};
                if (otherVariables.empty())
                    err.addNote(paramNode, toNte(Nte0201));
                else
                    err.addNote(sourceFile, paramNode->token.startLocation, otherVariables.back()->token.endLocation, toNte(Nte0199));
                return context->report(err);
            }

            if (hasMissingType)
                *hasMissingType = true;
        }

        // Add attribute as the last child, to avoid messing around with the first FuncDeclParam node.
        if (attrUse)
        {
            paramNode->attrUse          = attrUse;
            paramNode->attrUse->content = paramNode;
            Ast::addChildBack(paramNode, paramNode->attrUse);
        }

        // Propagate types and assignment to multiple declarations
        for (const auto one : otherVariables)
        {
            CloneContext cloneContext;
            cloneContext.parent = one;

            if (paramNode->type)
                one->type = castAst<AstTypeExpression>(paramNode->type->clone(cloneContext));
            if (paramNode->assignment)
                one->assignment = paramNode->assignment->clone(cloneContext);

            if (paramNode->attrUse)
            {
                one->attrUse          = castAst<AstAttrUse>(paramNode->attrUse->clone(cloneContext));
                one->attrUse->content = one;
            }
        }
    }

    return true;
}

bool Parser::doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType, bool isMethod, bool isConstMethod, bool isItfMethod)
{
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse, formErr(Err0443, parent->token.c_str())));

    // To avoid calling 'format' in case we know this is fine, otherwise it will be called each time, even when ok
    const auto startLoc = tokenParse.token.startLocation;
    if (tokenParse.isNot(TokenId::SymLeftParen))
    {
        if (parent->is(AstNodeKind::AttrDecl))
            return eatToken(TokenId::SymLeftParen, form("to declare the attribute parameters of [[%s]]", parent->token.c_str()));

        if (tokenParse.is(TokenId::Identifier) && getNextToken().is(TokenId::SymLeftParen))
        {
            PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [&]() { return formNte(Nte0216, tokenParse.token.c_str(), parent->token.c_str()); });
            return eatToken(TokenId::SymLeftParen, form("to declare the function parameters of [[%s]]", parent->token.c_str()));
        }

        return eatToken(TokenId::SymLeftParen, form("to declare the function parameters of [[%s]]", parent->token.c_str()));
    }

    SWAG_CHECK(eatToken());
    if (tokenParse.isNot(TokenId::SymRightParen) || isMethod || isConstMethod)
    {
        auto allParams = Ast::newFuncDeclParams(this, parent);
        *result        = allParams;

        // Add 'using self' as the first parameter in case of a method
        if (isMethod || isConstMethod)
        {
            const auto paramNode = Ast::newVarDecl("", this, allParams, AstNodeKind::FuncDeclParam);
            if (!isItfMethod)
                paramNode->addAstFlag(AST_DECL_USING);
            paramNode->addSpecFlag(AstVarDecl::SPEC_FLAG_GENERATED_SELF);
            paramNode->token.text = g_LangSpec->name_self;
            const auto typeNode   = Ast::newTypeExpression(nullptr, paramNode);
            typeNode->typeFlags.add(TYPEFLAG_IS_SELF | TYPEFLAG_IS_PTR | TYPEFLAG_IS_SUB_TYPE);
            if (!isItfMethod)
                typeNode->typeFlags.add(TYPEFLAG_HAS_USING);
            if (isConstMethod)
                typeNode->typeFlags.add(TYPEFLAG_IS_CONST);
            typeNode->identifier = Ast::newIdentifierRef(paramNode->ownerStructScope->name, this, typeNode);
            paramNode->type      = typeNode;
        }

        ParserPushAstNodeFlags sf(this, AST_IN_FUNC_DECL_PARAMS);
        bool                   oneParamDone = false;
        while (tokenParse.isNot(TokenId::SymRightParen))
        {
            bool missingTypes = false;
            SWAG_CHECK(doFuncDeclParameter(allParams, acceptMissingType, &missingTypes));

            // Do not mix deduced types and specified types in lambdas
            if (acceptMissingType)
            {
                SWAG_ASSERT(hasMissingType);
                if (oneParamDone && !*hasMissingType && missingTypes)
                    return error(allParams->lastChild(), toErr(Err0456));
                *hasMissingType = *hasMissingType || missingTypes;
            }

            if (tokenParse.is(TokenId::SymRightParen))
                break;

            oneParamDone    = true;
            auto tokenComma = tokenParse;
            SWAG_CHECK(eatToken(TokenId::SymComma, "to define another parameter or ')' to end the list"));

            if (tokenParse.is(TokenId::SymRightParen))
                return context->report({allParams, tokenComma.token, toErr(Err0100)});
        }
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doGenericDeclParameters(AstNode* parent, AstNode** result)
{
    const auto allParams = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, this, parent);
    *result              = allParams;

    SWAG_ASSERT(tokenParse.is(TokenId::SymLeftParen));
    const auto startLoc = tokenParse.token.startLocation;
    eatToken();
    SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse, toErr(Err0447)));

    while (tokenParse.isNot(TokenId::SymRightParen))
    {
        bool isConstant = false;
        bool isType     = false;

        const auto tokenForce = tokenParse.token;
        if (tokenParse.is(TokenId::KwdConst))
        {
            isConstant = true;
            SWAG_CHECK(eatToken());
        }
        else if (tokenParse.is(TokenId::KwdVar))
        {
            isType = true;
            SWAG_CHECK(eatToken());
        }

        SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0178)));

        auto oneParam = Ast::newVarDecl(tokenParse.token.text, this, allParams, AstNodeKind::FuncDeclParam);
        oneParam->addAstFlag(AST_GENERIC);
        if (isConstant)
            oneParam->addSpecFlag(AstVarDecl::SPEC_FLAG_FORCE_CONST);
        else if (isType)
            oneParam->addSpecFlag(AstVarDecl::SPEC_FLAG_FORCE_VAR);

        SWAG_CHECK(eatToken());

        if (tokenParse.is(TokenId::SymColon))
        {
            if (isType)
            {
                Diagnostic err{sourceFile, tokenParse, toErr(Err0179)};
                err.addNote(Diagnostic::note(oneParam, formNte(Nte0007, oneParam->token.c_str())));
                err.addNote(Diagnostic::note(sourceFile, tokenForce, toNte(Nte0043)));
                return context->report(err);
            }

            isConstant = true;

            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymLeftCurly), error(tokenParse, toErr(Err0614)));
            SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->type));
        }

        if (tokenParse.is(TokenId::SymEqual))
        {
            SWAG_CHECK(eatToken());
            if (isConstant)
            {
                SWAG_CHECK(doAssignmentExpression(oneParam, &oneParam->assignment));
            }
            else if (isType)
            {
                SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->assignment));
            }
            else if (!oneParam->type)
            {
                isType = true;
                PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::Note, [oneParam] { return formNte(Nte0040, oneParam->token.c_str()); }, true);
                SWAG_CHECK(doTypeExpression(oneParam, EXPR_FLAG_NONE, &oneParam->assignment));
            }
            else
            {
                SWAG_CHECK(doAssignmentExpression(oneParam, &oneParam->assignment));
            }
        }

        if (isType)
            oneParam->addSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_TYPE);
        else if (isConstant)
            oneParam->addSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT);

        if (isConstant && !oneParam->type && !oneParam->assignment)
            return error(tokenParse, toErr(Err0437));

        if (tokenParse.isNot(TokenId::SymComma))
            break;
        SWAG_CHECK(eatToken());
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(tokenParse, toErr(Err0100)));
    }

    SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    return true;
}

bool Parser::doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId, FuncDeclFlags flags)
{
    ParserPushTryCatchAssume sct(this, nullptr);

    auto funcNode         = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, this, parent);
    *result               = funcNode;
    funcNode->semanticFct = Semantic::resolveFuncDecl;
    funcNode->allocateExtension(ExtensionKind::Semantic);
    funcNode->extSemantic()->semanticBeforeFct = Semantic::preResolveFuncDecl;
    funcNode->extSemantic()->semanticAfterFct  = Semantic::sendCompilerMsgFuncDecl;

    bool isMethod = tokenParse.is(TokenId::KwdMethod);
    if (isMethod)
    {
        funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_METHOD);
        if (!funcNode->ownerStructScope)
            return error(tokenParse, toErr(Err0332));
        if (funcNode->ownerStructScope->is(ScopeKind::Enum))
            return error(tokenParse, toErr(Err0334));
        if (funcNode->ownerStructScope->isNot(ScopeKind::Struct))
            return error(tokenParse, toErr(Err0332));
    }

    if (typeFuncId == TokenId::Invalid)
    {
        typeFuncId = tokenParse.token.id;
        SWAG_CHECK(eatToken());
    }

    bool isConstMethod   = false;
    bool isIntrinsic     = false;
    auto funcForCompiler = g_TokenFlags[static_cast<int>(typeFuncId)].has(TOKEN_COMPILER_FUNC) || typeFuncId == TokenId::KwdWhere;

    // Name
    if (funcForCompiler)
    {
        uint32_t id = g_UniqueID.fetch_add(1);
        switch (typeFuncId)
        {
            case TokenId::CompilerFuncTest:
                funcNode->token.text = "__test" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_TEST_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerFuncInit:
                funcNode->token.text = "__init" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_INIT_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerFuncDrop:
                funcNode->token.text = "__drop" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_DROP_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerFuncPreMain:
                funcNode->token.text = "__premain" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerRun:
                funcNode->token.text = "__run" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_RUN_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerGeneratedRun:
                funcNode->token.text = "__run" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAstFlag(AST_GENERATED);
                funcNode->addAttribute(ATTRIBUTE_RUN_GENERATED_FUNC | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerGeneratedRunExp:
                funcNode->token.text = "__run" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAstFlag(AST_GENERATED);
                funcNode->addAttribute(ATTRIBUTE_RUN_GENERATED_EXP | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerFuncMain:
                funcNode->token.text = "__main" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::CompilerFuncMessage:
                funcNode->token.text = "__message" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAttribute(ATTRIBUTE_MESSAGE_FUNC | ATTRIBUTE_COMPILER | ATTRIBUTE_SHARP_FUNC);
                ++module->numCompilerFunctions;
                break;
            case TokenId::CompilerAst:
                funcNode->token.text = "__ast" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAstFlag(AST_GENERATED);
                funcNode->addAttribute(ATTRIBUTE_AST_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            case TokenId::KwdWhere:
                funcNode->token.text = "__where" + std::to_string(id);
                funcNode->tokenName  = funcNode->token;
                funcNode->addAstFlag(AST_GENERATED);
                funcNode->addAttribute(ATTRIBUTE_MATCH_WHERE_FUNC | ATTRIBUTE_CONSTEXPR | ATTRIBUTE_COMPILER | ATTRIBUTE_GENERATED_FUNC | ATTRIBUTE_SHARP_FUNC);
                break;
            default:
                break;
        }
    }
    else
    {
        // Generic parameters
        if (tokenParse.is(TokenId::SymLeftParen))
            SWAG_CHECK(doGenericDeclParameters(funcNode, &funcNode->genericParameters));

        // const
        if (tokenParse.is(TokenId::KwdConst))
        {
            SWAG_VERIFY(isMethod, error(tokenParse, toErr(Err0319)));
            isConstMethod = true;
            SWAG_CHECK(eatToken());
        }

        // Interface implementation function
        if (tokenParse.is(TokenId::KwdImpl))
        {
            funcNode->implLoc = tokenParse.token.startLocation;
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL);
            SWAG_CHECK(eatToken());
        }

        isIntrinsic = tokenParse.token.text[0] == '@';
        if (isIntrinsic)
            SWAG_VERIFY(sourceFile->acceptsInternalStuff(), error(tokenParse, formErr(Err0172, tokenParse.token.c_str())));
        else
            SWAG_CHECK(checkIsIdentifier(tokenParse, toErr(Err0173)));

        funcNode->tokenName = tokenParse.token;
        funcNode->inheritTokenName(tokenParse.token);
        SWAG_CHECK(checkIsValidUserName(funcNode, &tokenParse.token));

        SWAG_CHECK(eatToken());
    }

    // Register function name
    auto typeInfo         = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode    = funcNode;
    auto newScope         = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo    = typeInfo;
    funcNode->scope       = newScope;
    const auto symbolName = currentScope->symTable.registerSymbolName(context, funcNode, SymbolKind::Function);
    funcNode->setResolvedSymbolName(symbolName);

    // Store specific symbols for fast retrieve
    if (funcNode->token.is(g_LangSpec->name_opAffect))
        currentScope->symbolOpAffect = symbolName;
    else if (funcNode->token.is(g_LangSpec->name_opAffectLiteral))
        currentScope->symbolOpAffectSuffix = symbolName;
    else if (funcNode->token.is(g_LangSpec->name_opCast))
        currentScope->symbolOpCast = symbolName;

    // Count number of methods to resolve
    if (currentScope->is(ScopeKind::Struct) && !funcForCompiler)
    {
        auto typeStruct       = castTypeInfo<TypeInfoStruct>(currentScope->owner->typeInfo, TypeInfoKind::Struct);
        auto typeParam        = TypeManager::makeParam();
        typeParam->name       = funcNode->token.text;
        typeParam->typeInfo   = funcNode->typeInfo;
        typeParam->declNode   = funcNode;
        typeParam->declNode   = funcNode;
        funcNode->methodParam = typeParam;

        ScopedLock lk(typeStruct->mutex);
        typeStruct->cptRemainingMethods++;
        typeStruct->methods.push_back(typeParam);
        if (funcNode->hasOwnerCompilerIfBlock())
            funcNode->ownerCompilerIfBlock()->methodsCount.push_back({funcNode, typeStruct, typeStruct->methods.size() - 1});
        if (funcNode->isSpecialFunctionName())
            typeStruct->cptRemainingSpecialMethods++;
    }

    // Dispatch owners
    if (funcNode->genericParameters)
    {
        Ast::visit(funcNode->genericParameters, [&](AstNode* n) {
            n->ownerFct   = funcNode;
            n->ownerScope = newScope;
        });
    }

    // Parameters
    if (!funcForCompiler)
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, false, nullptr, isMethod, isConstMethod));
    }

    // #message has an expression has parameters
    else if (funcNode->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        auto            startLoc = tokenParse.token.startLocation;
        SWAG_CHECK(eatTokenError(TokenId::SymLeftParen, toErr(Err0419)));
        SWAG_VERIFY(tokenParse.isNot(TokenId::SymRightParen), error(funcNode, toErr(Err0414)));
        SWAG_CHECK(doExpression(funcNode, EXPR_FLAG_NONE, &funcNode->parameters));
        SWAG_CHECK(eatCloseToken(TokenId::SymRightParen, startLoc));
    }
    else if (tokenParse.is(TokenId::SymLeftParen))
    {
        Utf8 note;
        if (funcNode->hasAttribute(ATTRIBUTE_MAIN_FUNC))
            note = toNte(Nte0067);
        return error(tokenParse, formErr(Err0674, funcNode->getDisplayNameC()), note.c_str());
    }

    // Return type
    auto typeNode         = Ast::newNode<AstNode>(AstNodeKind::FuncDeclType, this, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = Semantic::resolveFuncDeclType;
    if (!funcForCompiler)
    {
        if (tokenParse.is(TokenId::SymMinusGreat))
        {
            typeNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED);
            ParserPushScope scoped(this, newScope);
            ParserPushFct   scopedFct(this, funcNode);
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdRetVal), error(tokenParse, toErr(Err0339)));
            AstNode* typeExpression;
            SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeExpression));
        }

        if (tokenParse.is(TokenId::KwdThrow))
        {
            SWAG_CHECK(eatToken());
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_THROW);
            funcNode->typeInfo->addFlag(TYPEINFO_CAN_THROW);
        }
        else if (tokenParse.is(TokenId::KwdAssume))
        {
            SWAG_CHECK(eatToken());
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME);
        }
    }
    else if (typeFuncId == TokenId::CompilerAst)
    {
        typeNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED);
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        auto            typeExpression  = Ast::newTypeExpression(this, typeNode);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoString;
    }
    else if (typeFuncId == TokenId::KwdWhere)
    {
        typeNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED);
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        auto            typeExpression  = Ast::newTypeExpression(this, typeNode);
        typeExpression->typeFromLiteral = g_TypeMgr->typeInfoBool;
    }

    funcNode->typeInfo->computeName();

    // 'where' block
    if (tokenParse.is(TokenId::KwdWhere))
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        SWAG_CHECK(doCompilerWhere(funcNode, &funcNode->whereExpression));
    }

    // If we have now a semicolon, then this is an empty function, like a forward decl in c++
    if (tokenParse.is(TokenId::SymSemiColon))
    {
        SWAG_VERIFY(!funcForCompiler, error(tokenParse, formErr(Err0061, funcNode->getDisplayNameC())));
        SWAG_CHECK(eatSemiCol("function declaration"));
        return true;
    }

    if (isIntrinsic)
        funcNode->addAstFlag(AST_DEFINED_INTRINSIC);

    // Content of function
    {
        newScope = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::FunctionBody, newScope);
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        AstNode*        resStmt = nullptr;

        // One single return expression
        if (tokenParse.is(TokenId::SymEqualGreater))
        {
            SWAG_CHECK(eatToken());
            SWAG_VERIFY(tokenParse.isNot(TokenId::KwdReturn), error(tokenParse, formErr(Err0336)));

            if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Try, this, funcNode);
                node->semanticFct = Semantic::resolveTryBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                funcNode->content = node;

                ParserPushTryCatchAssume sc(this, node);
                auto                     returnNode = Ast::newNode<AstReturn>(AstNodeKind::Return, this, node);
                returnNode->semanticFct             = Semantic::resolveReturn;
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
            }
            else if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Assume, this, funcNode);
                node->semanticFct = Semantic::resolveAssumeBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                funcNode->content = node;

                ParserPushTryCatchAssume sc(this, node);
                auto                     returnNode = Ast::newNode<AstReturn>(AstNodeKind::Return, this, node);
                returnNode->semanticFct             = Semantic::resolveReturn;
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
            }
            else
            {
                auto stmt               = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, funcNode);
                auto returnNode         = Ast::newNode<AstReturn>(AstNodeKind::Return, this, stmt);
                returnNode->semanticFct = Semantic::resolveReturn;
                funcNode->content       = returnNode;
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
                SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
            }

            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM);
            funcNode->content->token.endLocation = tokenParse.token.startLocation;
            resStmt                              = funcNode->content;
        }

        // One single statement
        else if (tokenParse.is(TokenId::SymEqual))
        {
            SWAG_CHECK(eatToken());

            if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Try, this, funcNode);
                node->semanticFct = Semantic::resolveTryBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                funcNode->content = node;

                auto stmt = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, node);

                ParserPushTryCatchAssume sc(this, node);
                SWAG_CHECK(doEmbeddedInstruction(stmt, &dummyResult));
            }
            else if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Assume, this, funcNode);
                node->semanticFct = Semantic::resolveAssumeBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                funcNode->content = node;

                auto stmt = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, node);

                ParserPushTryCatchAssume sc(this, node);
                SWAG_CHECK(doEmbeddedInstruction(stmt, &dummyResult));
            }
            else
            {
                auto stmt         = Ast::newNode<AstStatement>(AstNodeKind::Statement, this, funcNode);
                funcNode->content = stmt;

                SWAG_CHECK(doEmbeddedInstruction(stmt, &dummyResult));
            }

            funcNode->content->token.endLocation = tokenParse.token.startLocation;
            resStmt                              = funcNode->content;
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM);
        }

        // Normal curly statement
        else
        {
            if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Try, this, funcNode);
                funcNode->content = node; // :AutomaticTryContent
                node->semanticFct = Semantic::resolveTryBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                ParserPushTryCatchAssume sc(this, node);
                SWAG_CHECK(doFuncDeclBody(node, &resStmt, flags));
                node->token.endLocation = resStmt->token.endLocation;
            }
            else if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
            {
                auto node         = Ast::newNode<AstTryCatchAssume>(AstNodeKind::Assume, this, funcNode);
                funcNode->content = node; // :AutomaticTryContent
                node->semanticFct = Semantic::resolveAssumeBlock;
                node->addSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED | AstTryCatchAssume::SPEC_FLAG_BLOCK);
                ParserPushTryCatchAssume sc(this, node);
                SWAG_CHECK(doFuncDeclBody(node, &resStmt, flags));
                node->token.endLocation = resStmt->token.endLocation;
            }
            else
            {
                SWAG_CHECK(doFuncDeclBody(funcNode, &funcNode->content, flags));
                resStmt = funcNode->content;
            }
        }

        newScope->owner = resStmt;
        resStmt->allocateExtension(ExtensionKind::Semantic);
        resStmt->extSemantic()->semanticAfterFct = Semantic::resolveScopedStmtAfter;
        resStmt->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);
    }

    return true;
}

bool Parser::doFuncDeclBody(AstNode* node, AstNode** result, FuncDeclFlags flags)
{
    if (tokenParse.isNot(TokenId::SymLeftCurly))
    {
        if (flags.has(FUNC_DECL_INTERFACE))
        {
            const Diagnostic err{sourceFile, tokenParse, formErr(Err0059, node->token.c_str())};
            return context->report(err);
        }

        if (Tokenizer::isSymbol(tokenParse.token.id))
        {
            const auto nextToken = getNextToken();
            if (nextToken.is(TokenId::NativeType))
            {
                const Diagnostic err{sourceFile, tokenParse, toErr(Err0676)};
                return context->report(err);
            }
        }

        if (tokenParse.is(TokenId::NativeType))
        {
            const Diagnostic err{sourceFile, tokenParse, toErr(Err0675)};
            return context->report(err);
        }

        const Diagnostic err{sourceFile, tokenParse, toErr(Err0060)};
        return context->report(err);
    }

    SWAG_CHECK(doCurlyStatement(node, result));
    return true;
}

bool Parser::doReturn(AstNode* parent, AstNode** result)
{
    const auto node   = Ast::newNode<AstReturn>(AstNodeKind::Return, this, parent);
    *result           = node;
    node->semanticFct = Semantic::resolveReturn;

    // Return value
    SWAG_CHECK(eatToken());
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
        return true;
    if (tokenParse.isNot(TokenId::SymSemiColon))
        SWAG_CHECK(doExpression(node, EXPR_FLAG_NONE, &dummyResult));

    return true;
}

bool Parser::doClosureCaptureBlock(TypeInfoFuncAttr* typeInfo, AstFuncCallParams* capture)
{
    if (tokenParse.is(TokenId::SymVerticalVertical))
    {
        SWAG_CHECK(eatToken());
    }
    else
    {
        {
            PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [] { return toNte(Nte0009); });
            SWAG_CHECK(eatToken(TokenId::SymVertical, "to start the [[closure]] capture block"));
        }

        while (tokenParse.isNot(TokenId::SymVertical))
        {
            auto parentId = castAst<AstNode>(capture);
            auto byRef    = false;

            // Capture by reference
            if (tokenParse.is(TokenId::SymAmpersand))
            {
                parentId              = Ast::newNode<AstMakePointer>(AstNodeKind::MakePointer, this, capture);
                parentId->semanticFct = Semantic::resolveMakePointer;
                parentId->addSpecFlag(AstMakePointer::SPEC_FLAG_TO_REF);
                eatToken();
                byRef = true;
            }

            AstNode* idRef = nullptr;
            SWAG_CHECK(doIdentifierRef(parentId, &idRef, IDENTIFIER_NO_PARAMS));

            if (byRef)
                setForceTakeAddress(idRef);

            if (tokenParse.is(TokenId::SymVertical))
                break;

            SWAG_CHECK(eatToken(TokenId::SymComma, "in capture block"));
            SWAG_VERIFY(tokenParse.isNot(TokenId::SymVertical), error(tokenParse, toErr(Err0424)));
        }

        capture->token.endLocation = tokenParse.token.endLocation;
        SWAG_CHECK(eatToken());
    }

    SWAG_VERIFY(tokenParse.is(TokenId::SymLeftParen), error(tokenParse, toErr(Err0420)));
    typeInfo->addFlag(TYPEINFO_CLOSURE);
    return true;
}

bool Parser::doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType, bool* hasMissingType)
{
    ParserPushTryCatchAssume sc(this, nullptr);
    const auto               funcNode = Ast::newNode<AstFuncDecl>(AstNodeKind::FuncDecl, this, parent);
    *result                           = funcNode;
    funcNode->semanticFct             = Semantic::resolveFuncDecl;
    funcNode->addAstFlag(AST_GENERATED);

    const uint32_t id    = g_UniqueID.fetch_add(1);
    funcNode->token.text = "__lambda" + std::to_string(id);
    funcNode->tokenName  = tokenParse.token;

    const auto typeInfo = makeType<TypeInfoFuncAttr>();
    typeInfo->declNode  = funcNode;

    auto newScope      = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::Function, currentScope);
    funcNode->typeInfo = typeInfo;
    funcNode->scope    = newScope;
    currentScope->symTable.registerSymbolName(context, funcNode, SymbolKind::Function);

    // Closure capture arguments
    if (tokenParse.is(TokenId::KwdClosure))
    {
        SWAG_CHECK(eatToken());

        // captureParameters will be solved with capture block, that's why we do NOT put it as a child
        // of the function.
        const auto capture = Ast::newFuncCallParams(this, funcNode);
        capture->ownerFct  = funcNode;
        Ast::removeFromParent(capture);
        capture->semanticFct        = Semantic::resolveCaptureFuncCallParams;
        funcNode->captureParameters = capture;

        SWAG_CHECK(doClosureCaptureBlock(typeInfo, capture));
    }
    else
    {
        SWAG_CHECK(eatToken());
    }

    // Lambda parameters
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        SWAG_CHECK(doFuncDeclParameters(funcNode, &funcNode->parameters, acceptMissingType, hasMissingType));
    }

    // :ClosureForceFirstParam
    // Closure first parameter is a void* pointer that will point to the context
    if (typeInfo->isClosure())
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);

        if (!funcNode->parameters)
            funcNode->parameters = Ast::newFuncDeclParams(this, funcNode);
        const auto v = Ast::newVarDecl("__captureCxt", this, funcNode->parameters, AstNodeKind::FuncDeclParam);
        v->addAstFlag(AST_GENERATED);
        Ast::removeFromParent(v);
        Ast::addChildFront(funcNode->parameters, v);
        v->type           = Ast::newTypeExpression(this, v);
        v->type->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        v->type->addAstFlag(AST_NO_SEMANTIC);
    }

    // Return type
    const auto typeNode   = Ast::newNode<AstNode>(AstNodeKind::FuncDeclType, this, funcNode);
    funcNode->returnType  = typeNode;
    typeNode->semanticFct = Semantic::resolveFuncDeclType;
    if (tokenParse.is(TokenId::SymMinusGreat))
    {
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);
        SWAG_CHECK(eatToken());
        AstNode* typeExpression;
        SWAG_CHECK(doTypeExpression(typeNode, EXPR_FLAG_NONE, &typeExpression));
        Ast::setForceConstType(typeExpression);
        typeNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED);
    }

    // Body
    {
        newScope = Ast::newScope(funcNode, funcNode->token.text, ScopeKind::FunctionBody, newScope);
        ParserPushScope scoped(this, newScope);
        ParserPushFct   scopedFct(this, funcNode);

        // One single return expression
        if (tokenParse.is(TokenId::SymEqualGreater))
        {
            SWAG_CHECK(eatToken());
            const auto returnNode   = Ast::newNode<AstReturn>(AstNodeKind::Return, this, funcNode);
            returnNode->semanticFct = Semantic::resolveReturn;
            funcNode->content       = returnNode;
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
            SWAG_CHECK(doExpression(returnNode, EXPR_FLAG_NONE, &dummyResult));
        }

        // Normal curly statement
        else
        {
            SWAG_CHECK(doCurlyStatement(funcNode, &funcNode->content));
            funcNode->content->token = tokenParse.token;
        }

        funcNode->content->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);
        newScope->owner = funcNode->content;
    }

    return true;
}

bool Parser::doLambdaExpression(AstNode* parent, ExprFlags exprFlags, AstNode** result)
{
    bool acceptMissingType = false;
    bool deduceMissingType = false;

    // We accept missing types if lambda is in a function call, because we can deduce them from the called
    // function parameters
    if (exprFlags.has(EXPR_FLAG_IN_CALL))
        acceptMissingType = true;

    // We accept missing types if lambda is in an affectation, because we can deduce them from the
    // type on the left
    else if (parent->is(AstNodeKind::AffectOp))
    {
        acceptMissingType = true;
        deduceMissingType = true;
    }

    // We accept missing types if lambda is in a variable declaration with a type, because we can deduce
    // them from the type
    else if (exprFlags.has(EXPR_FLAG_IN_VAR_DECL_WITH_TYPE))
    {
        acceptMissingType = true;
        deduceMissingType = true;
    }

    const auto exprNode   = Ast::newNode<AstMakePointer>(AstNodeKind::MakePointerLambda, this, parent);
    exprNode->semanticFct = Semantic::resolveMakePointerLambda;

    AstNode* lambda         = nullptr;
    bool     hasMissingType = false;

    {
        ParserPushBreakable sb(this, nullptr);
        SWAG_CHECK(doLambdaFuncDecl(currentFct, &lambda, acceptMissingType, &hasMissingType));
    }

    SourceLocation start, end;
    lambda->computeLocation(start, end);
    lambda->token.startLocation = start;
    lambda->token.endLocation   = end;

    const auto lambdaDecl = castAst<AstFuncDecl>(lambda, AstNodeKind::FuncDecl);
    lambdaDecl->addSpecFlag(AstFuncDecl::SPEC_FLAG_IS_LAMBDA_EXPRESSION);
    if (!lambda->ownerFct && lambdaDecl->captureParameters)
        return error(lambdaDecl, toErr(Err0375));

    // Lambda sub function will be resolved by the owner function
    if (lambda->ownerFct)
        registerSubDecl(lambda);
    // If the lambda is created at global scope, register it as a normal function
    else
        Ast::addChildBack(sourceFile->astRoot, lambda);

    // Retrieve the pointer of the function
    exprNode->inheritTokenLocation(lambda->token);
    exprNode->lambda = lambdaDecl;

    ParserPushAstNodeFlags sc(this, AST_GENERATED);
    if (lambdaDecl->captureParameters)
    {
        // To solve captured parameters
        const auto cp = castAst<AstFuncCallParams>(lambdaDecl->captureParameters, AstNodeKind::FuncCallParams);
        Ast::addChildBack(exprNode, cp);
        cp->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

        // We want the lambda to be evaluated only once the captured block has been typed
        // See resolveCaptureFuncCallParams
        lambdaDecl->addAstFlag(AST_NO_SEMANTIC | AST_SPEC_SEMANTIC1);

        // Reference to the function
        AstNode* identifierRef = Ast::newIdentifierRef(lambda->token.text, this, exprNode);
        identifierRef->inheritTokenLocation(lambda->token);
        identifierRef->lastChild()->inheritTokenLocation(lambda->token);
        setForceTakeAddress(identifierRef);

        // Create the capture block (a tuple)
        const auto nameCaptureBlock = form(R"(__captureblock%d)", g_UniqueID.fetch_add(1));
        const auto block            = Ast::newVarDecl(nameCaptureBlock, this, exprNode);
        block->inheritTokenLocation(lambdaDecl->captureParameters->token);
        block->addAstFlag(AST_GENERATED);
        const auto exprList   = Ast::newNode<AstExpressionList>(AstNodeKind::ExpressionList, this, block);
        exprList->semanticFct = Semantic::resolveExpressionListTuple;
        exprList->addSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE | AstExpressionList::SPEC_FLAG_FOR_CAPTURE);
        exprList->inheritTokenLocation(lambdaDecl->captureParameters->token);
        block->assignment = exprList;
        Semantic::setVarDeclResolve(block);

        for (const auto c : lambdaDecl->captureParameters->children)
            (void) Ast::clone(c, exprList);

        // Reference to the captured block
        identifierRef = Ast::newIdentifierRef(nameCaptureBlock, this, exprNode);
        identifierRef->inheritTokenLocation(lambdaDecl->captureParameters->token);
        identifierRef->lastChild()->inheritTokenLocation(lambdaDecl->captureParameters->token);
        setForceTakeAddress(identifierRef);
    }
    else
    {
        // Reference to the function
        AstNode* identifierRef = Ast::newIdentifierRef(lambda->token.text, this, exprNode);
        identifierRef->inheritTokenLocation(lambda->token);
        identifierRef->lastChild()->inheritTokenLocation(lambda->token);
        setForceTakeAddress(identifierRef);
    }

    // :DeduceLambdaType
    if (deduceMissingType && acceptMissingType && hasMissingType)
    {
        exprNode->addSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE);
        lambdaDecl->makePointerLambda = exprNode;
        lambdaDecl->addAstFlag(AST_NO_SEMANTIC | AST_SPEC_SEMANTIC2);
    }

    *result = exprNode;
    return true;
}
